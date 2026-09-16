#pragma once
#include "esp_err.h"

// IDs match the small vocabulary registered in main.c.
typedef enum { CMD_RED = 1, CMD_GREEN, CMD_BLUE, CMD_NEXT, CMD_HOME } voice_command_t;
esp_err_t ui_init(void);
void ui_show_status(const char *text);
void ui_execute_command(voice_command_t command, const char *text);
