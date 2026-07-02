#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_psram.h"

#include "nvm_flash.h"

static const char *TAG = "MAIN";

#define VIDEO_BUFFER_SIZE ( 832 * 1024 )

static uint8_t *video_buffer = NULL;

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "=======================================");
    ESP_LOGI(TAG, "CH341A Video Flash Emulator");
    ESP_LOGI(TAG, "ESP-IDF %s", esp_get_idf_version());
    ESP_LOGI(TAG, "=======================================");

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||) {

    }

}
