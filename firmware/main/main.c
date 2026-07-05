// main.c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_flash.h"

#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";

#define VIDEO_PARTITION_NAME "video"
#define USB_TASK_STACK       8192
#define USB_TASK_PRIORITY    5

// Globals
static const esp_partition_t *video_partition = NULL;

// Forward Declarations
static esp_err_t init_nvs(void);
static esp_err_t locate_video_partition(void);
static void print_chip_info(void);
static void print_partition_info(void);
static void usb_task(void *arg);

extern esp_err_t usb_protocol_init(void);
extern bool usb_protocol_available(void);
extern int usb_prorocol_receive(
    uint8_t *buffer,
    size_t length);
extern int usb_protocol_send(
    const uint8_t *buffer,
    size_t length);
extern esp_err_t partition_read(
    uint32_t address,
    void *buffer,
    size_t length);
extern esp_err_t partition_write(
    uint32_t address,
    const void *buffer,
    size_t length);
extern esp_err_t partition_erase(void);

static esp_err_t init_nvs(void);
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        esp == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}

static void print_chip_information(void)
{
    esp_chip_info_t chip;

    esp_chip_info(&chip);

    uint32_t flash_size = 0;

    esp_flash_get_size(NULL, &flash_size);

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "ESP-IDF      : %s", esp_get_idf_version());
    ESP_LOGI(TAG, "CPU Cores    : %d", chip.cores);
    ESP_LOGI(TAG, "Revision     : %d", chip.revision);
    ESP_LOGI(TAG, "Flash Size   : %" PRIu32 " MB",
             flash_size / (1024 * 1024));
    ESP_LOGI(TAG, "========================================");
}

static esp_err_t locate_video_partition(void)
{
    video_partition =
        esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA,
            0x40,
            VIDEO_PARTITION_NAME);

    if (video_partition == NULL)
    {
        ESP_LOGE(TAG,
                "Partition '%s' not found!",
                VIDEO_PARTITION_NAME);

        return ESP_FAIL;
    }
    return ESP_OK;
}

static void print_partition_information(void)
{
    ESP_LOGI(TAG, "Video parition");

    ESP_LOGI(TAG,
            "Address : 0x%08" PRIX32,
            video_partition->address);

    ESP_LOGI(TAG,
            "Size   : %" PRIu32 " bytes",
            video_partition->size);
}

void app_main(void);
{
    ESP_ERROR_CHECK(init_nvs());

    print_chip_info();

    ESP_ERROR_CHECK(locate_video_partition());

    print_partition_information();

    ESP_LOGI(TAG, "Initialising USB protocol...");

    ESP_ERROR_CHECK(usb_protocol_init());

    xTaskCreate(
        usb_task,
        "usb_task",
        USB_TASK_STACK,
        NULL,
        USB_TASK_PRIORITY,
        NULL);

    ESP_LOGI(TAG, "Firmware ready.");

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
