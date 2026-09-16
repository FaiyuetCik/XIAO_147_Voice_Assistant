#include "ui.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define W 172
#define H 320
#define BG 0x1082
#define WHITE 0xffff
#define CYAN 0x07ff
#define GREY 0xad55
static const char *TAG = "ui";
static spi_device_handle_t lcd;
static uint16_t *pixels;
static QueueHandle_t updates;
typedef struct { int page; uint16_t color; unsigned count; char status[32]; char last[32]; } view_t;
static view_t view = {.color = 0xf800, .status = "Starting", .last = "None yet"};

// Original compact 5x7 uppercase font; rows use the low five bits.
static const uint8_t font[][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
 {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
 {31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {31,4,4,4,4,4,31},{7,2,2,2,18,18,12},
 {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
 {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
 {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},
 {17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},
 {14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14}
};
static uint16_t wire_color(uint16_t c) { return (c << 8) | (c >> 8); }
static void rect(int x, int y, int w, int h, uint16_t c)
{
    for (int yy = y; yy < y+h && yy < H; ++yy)
        for (int xx = x; xx < x+w && xx < W; ++xx)
            if (xx >= 0 && yy >= 0) pixels[yy*W+xx] = wire_color(c);
}
static void text(int x, int y, const char *s, int scale, uint16_t c)
{
    for (; *s; ++s, x += 6*scale) {
        int ch = toupper((unsigned char)*s);
        int index = ch >= 'A' && ch <= 'Z' ? ch-'A' : ch >= '0' && ch <= '9' ? ch-'0'+26 : -1;
        if (index < 0) continue;
        for (int row=0; row<7; ++row)
            for (int col=0; col<5; ++col)
                if (font[index][row] & (1 << (4-col))) rect(x+col*scale,y+row*scale,scale,scale,c);
    }
}
static void center(int y, const char *s, int scale, uint16_t c)
{ text((W-(int)strlen(s)*6*scale+scale)/2,y,s,scale,c); }
static esp_err_t command(uint8_t c, const uint8_t *data, size_t len)
{
    ESP_RETURN_ON_ERROR(gpio_set_level(4,0), TAG, "DC");
    spi_transaction_t t = {.length=8, .tx_buffer=&c};
    ESP_RETURN_ON_ERROR(spi_device_polling_transmit(lcd,&t),TAG,"command");
    if (len) {
        ESP_RETURN_ON_ERROR(gpio_set_level(4,1), TAG, "DC");
        t.length=len*8; t.tx_buffer=data;
        ESP_RETURN_ON_ERROR(spi_device_polling_transmit(lcd,&t),TAG,"data");
    }
    return ESP_OK;
}
static esp_err_t flush(void)
{
    const uint8_t xs[] = {0,34,0,205}, ys[] = {0,0,1,63};
    ESP_RETURN_ON_ERROR(command(0x2a,xs,4),TAG,"columns");
    ESP_RETURN_ON_ERROR(command(0x2b,ys,4),TAG,"rows");
    ESP_RETURN_ON_ERROR(command(0x2c,NULL,0),TAG,"pixels");
    gpio_set_level(4,1);
    // Internal DMA buffer, synchronous transfer; framebuffer is never reused in flight.
    for (int y=0;y<H;y+=8) {
        spi_transaction_t t = {.length=W*8*16, .tx_buffer=pixels+y*W};
        ESP_RETURN_ON_ERROR(spi_device_polling_transmit(lcd,&t),TAG,"frame");
    }
    return ESP_OK;
}
static void render(const view_t *v)
{
    rect(0,0,W,H,BG);
    rect(0,0,W,38,0x2124); center(12,"XIAO VOICE",2,CYAN);
    center(52,v->status,strlen(v->status)>14?1:2,WHITE);
    if (v->page == 0) {
        center(93,strcmp(v->status,"Listening")==0?"SAY COMMAND":"SAY HI ESP",2,CYAN);
        const char *items[]={"SHOW RED","SHOW GREEN","SHOW BLUE","NEXT PAGE","GO HOME"};
        for (int i=0;i<5;++i) text(18,133+i*23,items[i],2,WHITE);
    } else if (v->page == 1) {
        rect(12,91,148,151,v->color);
        const char *name = v->color==0xf800?"RED":v->color==0x07e0?"GREEN":"BLUE";
        center(154,name,3,v->color==0x07e0?0:WHITE);
    } else {
        center(98,"OFFLINE",3,CYAN);
        center(138,"WAKENET 9",2,WHITE);
        center(166,"MULTINET 7",2,WHITE);
        char count[24]; snprintf(count,sizeof(count),"COMMANDS %u",v->count);
        center(216,count,1,GREY);
    }
    rect(8,263,156,1,GREY);
    center(274,"LAST COMMAND",1,GREY);
    center(289,v->last,2,CYAN);
    if (flush()!=ESP_OK) ESP_LOGE(TAG,"LCD refresh failed");
}
static void display_task(void *arg)
{
    view_t next;
    while (true) if (xQueueReceive(updates,&next,portMAX_DELAY)==pdTRUE) render(&next);
}
esp_err_t ui_init(void)
{
    gpio_config_t io={.pin_bit_mask=(1ULL<<4)|(1ULL<<12)|(1ULL<<13)|(1ULL<<43),.mode=GPIO_MODE_OUTPUT};
    ESP_RETURN_ON_ERROR(gpio_config(&io),TAG,"GPIO");
    gpio_set_level(43,1); // D6: keep SD card deselected on the shared SPI bus.
    gpio_set_level(12,0);
    gpio_set_level(13,1); vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(13,0); vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(13,1); vTaskDelay(pdMS_TO_TICKS(120));
    spi_bus_config_t bus={.mosi_io_num=9,.miso_io_num=-1,.sclk_io_num=7,.quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=W*8*2};
    ESP_RETURN_ON_ERROR(spi_bus_initialize(SPI2_HOST,&bus,SPI_DMA_CH_AUTO),TAG,"SPI bus");
    spi_device_interface_config_t dev={.clock_speed_hz=10000000,.mode=0,.spics_io_num=3,.queue_size=1};
    ESP_RETURN_ON_ERROR(spi_bus_add_device(SPI2_HOST,&dev,&lcd),TAG,"SPI LCD");
    ESP_RETURN_ON_ERROR(command(0x11,NULL,0),TAG,"sleep out");
    vTaskDelay(pdMS_TO_TICKS(120));
    // JD9853A compatible sequence and MADCTL/offsets from installed Seeed_GFX2.
    static const struct {uint8_t cmd,n,data[14];} init[]={
        {0x13,0,{0}}, {0x36,1,{0x48}}, {0xb6,2,{0x0a,0x82}},
        {0xb0,2,{0,0xe0}}, {0x3a,1,{0x55}}, {0xb2,5,{12,12,0,0x33,0x33}},
        {0xb7,1,{0x35}}, {0xbb,1,{0x28}}, {0xc0,1,{12}}, {0xc2,2,{1,255}},
        {0xc3,1,{0x10}}, {0xc4,1,{0x20}}, {0xc6,1,{15}}, {0xd0,2,{0xa4,0xa1}},
        {0xe0,14,{0xd0,0,2,7,10,0x28,0x32,0x44,0x42,6,14,18,20,23}},
        {0xe1,14,{0xd0,0,2,7,10,0x28,0x31,0x54,0x47,14,28,23,27,30}},
        {0x20,0,{0}}, {0x29,0,{0}}
    };
    for (unsigned i=0;i<sizeof(init)/sizeof(init[0]);++i)
        ESP_RETURN_ON_ERROR(command(init[i].cmd,init[i].data,init[i].n),TAG,"LCD init");
    vTaskDelay(pdMS_TO_TICKS(120));
    pixels=heap_caps_malloc(W*H*2,MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
    if (!pixels) return ESP_ERR_NO_MEM;
    updates=xQueueCreate(1,sizeof(view_t));
    if (!updates) {free(pixels); pixels=NULL; return ESP_ERR_NO_MEM;}
    render(&view);
    gpio_set_level(12,1);
    if (xTaskCreatePinnedToCore(display_task,"lcd",4096,NULL,2,NULL,0)!=pdPASS) return ESP_ERR_NO_MEM;
    ESP_LOGI(TAG,"JD9853A ready: 172x320, CS3 DC4 SCK7 MOSI9 RST13 BL12");
    return ESP_OK;
}
// Called by app_main during startup, then exclusively by the recognition task.
void ui_show_status(const char *s)
{
    snprintf(view.status,sizeof(view.status),"%s",s);
    if (updates) xQueueOverwrite(updates,&view);
    ESP_LOGI(TAG,"STATUS: %s",s);
}
void ui_execute_command(voice_command_t id, const char *s)
{
    if (id>=CMD_RED && id<=CMD_BLUE) {
        const uint16_t colors[]={0xf800,0x07e0,0x001f};
        view.color=colors[id-1]; view.page=1;
    } else if (id==CMD_NEXT) view.page=(view.page+1)%3;
    else if (id==CMD_HOME) view.page=0;
    else return;
    ++view.count;
    snprintf(view.last,sizeof(view.last),"%s",s);
    ui_show_status("Listening");
    ESP_LOGI(TAG,"ACTION: %s page=%d",s,view.page);
}
