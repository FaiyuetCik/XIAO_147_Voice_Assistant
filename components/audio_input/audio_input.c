#include "audio_input.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2s_pdm.h"

static const char *TAG = "audio_input";
static i2s_chan_handle_t s_rx_chan;

esp_err_t audio_input_init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 16;
    chan_cfg.dma_frame_num = 512;

    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &s_rx_chan);
    if (err != ESP_OK) return err;

    i2s_pdm_rx_config_t pdm_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = GPIO_NUM_1,
            .din = GPIO_NUM_2,
            .invert_flags = { .clk_inv = false },
        },
    };

    err = i2s_channel_init_pdm_rx_mode(s_rx_chan, &pdm_cfg);
    if (err != ESP_OK) return err;
    err = i2s_channel_enable(s_rx_chan);
    if (err == ESP_OK) ESP_LOGI(TAG, "PDM RX ready: 16 kHz, 16-bit, mono, D0/D1");
    return err;
}

esp_err_t audio_input_read(int16_t *samples, size_t sample_count, size_t *samples_read, uint32_t timeout_ms)
{
    if (!s_rx_chan || !samples || !samples_read) return ESP_ERR_INVALID_STATE;
    return i2s_channel_read(s_rx_chan, samples, sample_count * sizeof(int16_t), samples_read, timeout_ms);
}
