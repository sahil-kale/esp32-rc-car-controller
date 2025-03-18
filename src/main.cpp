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

constexpr static uint32_t SERVO_PERIOD_US = 50 * 1000;
constexpr static uint32_t LEDC_TIMER_FREQ_HZ = 1000 * 1000;
constexpr static uint32_t LEDC_TIMER_MAX_DUTY = (1 << 13) - 1;  // 13-bit resolution (8191)

#define LEDC_SERVO_TIMER LEDC_TIMER_1

#define STEERING_SERVO_LEDC_CHANNEL LEDC_CHANNEL_0
#define STEERING_SERVO_LEDC_GPIO 18

#define THROTTLE_SERVO_LEDC_CHANNEL LEDC_CHANNEL_1
#define THROTTLE_SERVO_LEDC_GPIO 19

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
        udp_rx_cmd_t cmd = {0.0F, 0.0F};
        if(udp_server.receive_data(rxbuf_test, sizeof(rxbuf_test)))
        {            
            memcpy(&cmd, rxbuf_test, sizeof(cmd));
            
            ESP_LOGI(TAG, "Rx Command Received: %f steer angle, %f",cmd.steer_angle, cmd.throttle);
        }

        control_servo(LEDC_CHANNEL_SERVO, cmd.steer_angle);
        control_servo(LEDC_CHANNEL_THROTTLE, cmd.throttle);
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}