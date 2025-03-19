#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESPHAL_Wifi.hpp"
#include "nvs_flash.h"
#include "ESPHAL_UDPServer.hpp"
#include "actuators.h"


constexpr static char* TAG = "MAIN";
static ESPHAL_Wifi wifi;
static ESPHAL_UDPServer udp_server;

typedef struct __attribute__((packed)) {
    float steer_angle;
    float throttle;
} udp_rx_cmd_t;

constexpr static uint32_t MAX_PACKET_TIMEOUT = 200U;
static uint32_t last_packet_time = MAX_PACKET_TIMEOUT;

constexpr static udp_rx_cmd_t SAFE_CMD = {0.65F, 0.5F}; // Default safe command

static udp_rx_cmd_t cmd = SAFE_CMD;

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
    //init_pwm();
    init_pwm();

    while (true) {
        // Check for received data
        static uint8_t rxbuf_test[1024U] = {0};

        if(udp_server.receive_data(rxbuf_test, sizeof(rxbuf_test)))
        {            
            memcpy(&cmd, rxbuf_test, sizeof(cmd));
            
            ESP_LOGI(TAG, "Rx Command Received: %f steer angle, %f",cmd.steer_angle, cmd.throttle);
            last_packet_time = 0;
        }

        if (last_packet_time > MAX_PACKET_TIMEOUT) {
            // No packet received for a while, revert to safe command
            cmd = SAFE_CMD;
        }

        control_servo(LEDC_CHANNEL_SERVO, cmd.steer_angle);
        control_servo(LEDC_CHANNEL_THROTTLE, cmd.throttle);
        
        last_packet_time += 10U;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}