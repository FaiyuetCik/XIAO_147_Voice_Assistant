#include "esp_log.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_mn_models.h"
#include "esp_mn_speech_commands.h"
#include "model_path.h"
#include "audio_input.h"
#include "ui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdlib.h>

static const char *TAG="voice_assistant";
static const char *commands[]={"show red","show green","show blue","next page","go home"};
static const esp_wn_iface_t *wn;
static esp_mn_iface_t *mn;
static model_iface_data_t *wake_data,*command_data;
static int wake_chunk,command_chunk;
static const int64_t idle_timeout_us=15000000;

static void recognition_task(void *arg)
{
    int max_chunk=wake_chunk>command_chunk?wake_chunk:command_chunk;
    int16_t *samples=heap_caps_malloc(max_chunk*sizeof(int16_t),MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!samples) {ui_show_status("Audio memory error"); vTaskDelete(NULL); return;}
    bool listening=false;
    int64_t deadline=0,cooldown=0;
    unsigned wake_count=0,command_count=0;
    int64_t last_health=esp_timer_get_time();
    unsigned slow_frames=0,frames=0;
    while (true) {
        int chunk=listening?command_chunk:wake_chunk;
        size_t filled=0;
        // Preserve partial I2S reads; never pass incomplete frames to a model.
        while (filled<(size_t)chunk*sizeof(int16_t)) {
            size_t got=0;
            esp_err_t err=audio_input_read(samples+filled/2,chunk-filled/2,&got,1000);
            if (err!=ESP_OK || !got || got%2) {
                ESP_LOGW(TAG,"Audio read failed: %s bytes=%u",esp_err_to_name(err),(unsigned)got);
                filled=0; mn->clean(command_data);
                listening=false; ui_show_status("Mic read error");
                vTaskDelay(pdMS_TO_TICKS(20)); break;
            }
            filled+=got;
        }
        if (filled!=(size_t)chunk*2) continue;
        int64_t now=esp_timer_get_time();
        // Keep WakeNet fed continuously; its v2.5.3 clean callback crashes on this model.
        wakenet_state_t wake_state=wn->detect(wake_data,samples);
        if (now<cooldown) continue;
        int64_t started=now;
        if (!listening) {
            if (wake_state==WAKENET_DETECTED) {
                ++wake_count; mn->clean(command_data);
                listening=true; deadline=esp_timer_get_time()+idle_timeout_us;
                ESP_LOGI(TAG,"Wake word detected: Hi ESP (#%u), continuous commands, idle timeout 15 seconds",wake_count);
                ui_show_status("Listening");
            }
        } else {
            esp_mn_state_t state=mn->detect(command_data,samples);
            if (state==ESP_MN_STATE_DETECTED) {
                esp_mn_results_t *r=mn->get_results(command_data);
                if (r && r->num>0 && r->command_id[0]>=CMD_RED && r->command_id[0]<=CMD_HOME) {
                    int id=r->command_id[0]; ++command_count;
                    ESP_LOGI(TAG,"COMMAND: id=%d text=%s probability=%.3f (#%u)",id,commands[id-1],r->prob[0],command_count);
                    ui_execute_command((voice_command_t)id,commands[id-1]);
                    deadline=esp_timer_get_time()+idle_timeout_us;
                    cooldown=esp_timer_get_time()+500000;
                    mn->clean(command_data);
                }
            }
            // Decoder timeouts do not end the continuous session.
            if (state==ESP_MN_STATE_TIMEOUT) mn->clean(command_data);
            if (listening && esp_timer_get_time()>=deadline) {
                ESP_LOGI(TAG,"Command timeout; waiting for Hi ESP");
                listening=false; mn->clean(command_data);
                ui_show_status("Say Hi ESP");
            }
        }
        if (esp_timer_get_time()-started>(int64_t)chunk*1000000/16000) ++slow_frames;
        ++frames;
        if (now-last_health>30000000) {
            ESP_LOGI(TAG,"HEALTH: wakes=%u commands=%u slow_frames=%u/%u free_heap=%u",
                     wake_count,command_count,slow_frames,frames,(unsigned)esp_get_free_heap_size());
            frames=slow_frames=0; last_health=now;
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG,"XIAO offline voice commands v1.1 continuous");
    if (ui_init()!=ESP_OK) {ESP_LOGE(TAG,"LCD initialization failed"); return;}
    ui_show_status("Loading models");
    srmodel_list_t *models=esp_srmodel_init("model");
    if (!models) {ui_show_status("Model error"); return;}
    char *wake_name=esp_srmodel_filter(models,ESP_WN_PREFIX,"hiesp");
    char *command_name=esp_srmodel_filter(models,ESP_MN_PREFIX,ESP_MN_ENGLISH);
    if (!wake_name || !command_name) {ui_show_status("Missing model"); return;}
    wn=esp_wn_handle_from_name(wake_name); mn=esp_mn_handle_from_name(command_name);
    if (!wn || !mn) {ui_show_status("Model API error"); return;}
    wake_data=wn->create(wake_name,DET_MODE_95);
    command_data=mn->create(command_name,6000);
    if (!wake_data || !command_data) {ui_show_status("Model memory error"); return;}
    if (esp_mn_commands_alloc(mn,command_data)!=ESP_OK) {ui_show_status("Vocabulary error"); return;}
    for (unsigned i=0;i<sizeof(commands)/sizeof(commands[0]);++i) {
        if (esp_mn_commands_add(i+1,commands[i])!=ESP_OK) {
            ESP_LOGE(TAG,"Cannot register %s",commands[i]); ui_show_status("Command error"); return;
        }
    }
    esp_mn_error_t *errors=esp_mn_commands_update();
    if (errors && errors->num>0) {ui_show_status("Vocabulary error"); return;}
    esp_mn_active_commands_print();
    wake_chunk=wn->get_samp_chunksize(wake_data);
    command_chunk=mn->get_samp_chunksize(command_data);
    if (wake_chunk<=0 || command_chunk<=0 || wake_chunk!=command_chunk || mn->get_samp_rate(command_data)!=16000) {
        ui_show_status("Audio format error"); return;
    }
    // Start microphone after loading the models, avoiding stale startup audio.
    if (audio_input_init()!=ESP_OK) {ui_show_status("Mic error"); return;}
    ESP_LOGI(TAG,"READY: wake=%s chunk=%d command=%s chunk=%d, 5 commands, heap=%u",
             wake_name,wake_chunk,command_name,command_chunk,(unsigned)esp_get_free_heap_size());
    ui_show_status("Say Hi ESP");
    if (xTaskCreatePinnedToCore(recognition_task,"speech",16384,NULL,5,NULL,1)!=pdPASS)
        ui_show_status("Task memory error");
}
