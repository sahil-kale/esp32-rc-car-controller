#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESPHAL_Wifi.hpp"
#include "nvs_flash.h"
#include "ESPHAL_UDPServer.hpp"

constexpr static char* TAG = "MAIN";
static ESPHAL_Wifi wifi;
static ESPHAL_UDPServer udp_server;

typedef struct __attribute__((packed)) {
    float steer_angle;
    float throttle;
} udp_rx_cmd_t;

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
        // Check for received data
        static uint8_t rxbuf_test[1024U] = {0};
        if(udp_server.receive_data(rxbuf_test, sizeof(rxbuf_test)))
        {            
            udp_rx_cmd_t* cmd = (udp_rx_cmd_t*)rxbuf_test; 
            
            ESP_LOGI(TAG, "Rx Command Received: %f steer angle, %f",cmd->steer_angle, cmd->throttle);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}