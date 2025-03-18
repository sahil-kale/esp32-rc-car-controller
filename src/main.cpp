#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESPHAL_Wifi.hpp"
#include "nvs_flash.h"

constexpr static char* TAG = "MAIN";
static ESPHAL_Wifi wifi;

extern "C" void app_main() {

    ESP_LOGI(TAG, "Starting ESP-IDF application...");

    // Initialize the NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "NVS initialized.");
    // Initialize the Wi-Fi
    wifi.init();
    ESP_LOGI(TAG, "Wi-Fi initialized.");

    while (true) {
        ESP_LOGI(TAG, "Running main loop...");
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
    }
}