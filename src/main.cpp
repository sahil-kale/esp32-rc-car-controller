#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESPHAL_Wifi.hpp"
#include "nvs_flash.h"
#include "ESPHAL_UDPServer.hpp"

constexpr static char* TAG = "MAIN";
static ESPHAL_Wifi wifi;
static ESPHAL_UDPServer udp_server;

extern "C" void app_main() {

    ESP_LOGI(TAG, "Starting ESP-IDF application...");

    // Initialize the NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "NVS initialized.");
    // Initialize the Wi-Fi
    wifi.init();
    ESP_LOGI(TAG, "Wi-Fi initialized.");
    udp_server.init();
    ESP_LOGI(TAG, "UDP server initialized.");

    while (true) {
        ESP_LOGI(TAG, "Running main loop...");
        // Check for received data
        static uint8_t rxbuf_test[1024U] = {0};
        udp_server.receive_data(rxbuf_test, sizeof(rxbuf_test));
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
    }
}