#pragma once

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

esp_err_t audio_input_init(void);
esp_err_t audio_input_read(int16_t *samples, size_t sample_count, size_t *samples_read, uint32_t timeout_ms);
