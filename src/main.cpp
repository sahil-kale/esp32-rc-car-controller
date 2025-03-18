#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESPHAL_Wifi.hpp"
#include "nvs_flash.h"
#include "ESPHAL_UDPServer.hpp"
#include "driver/ledc.h"


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

void control_servo(ledc_channel_t channel, float ontime_percent)
{
    constexpr static uint32_t MIN_SERVO_DUTY_US = 1000;  // 1ms pulse width (0°)
    constexpr static uint32_t MAX_SERVO_DUTY_US = 2000;  // 2ms pulse width (180°)
    constexpr static uint32_t SERVO_DUTY_US_RANGE = MAX_SERVO_DUTY_US - MIN_SERVO_DUTY_US;

    // Convert percentage to pulse width (microseconds)
    uint32_t duty_us = MIN_SERVO_DUTY_US + (ontime_percent * SERVO_DUTY_US_RANGE);
    const float duty_ontime_in_servo_period = static_cast<float>(duty_us) / static_cast<float>(SERVO_PERIOD_US);

    // Convert microseconds to duty cycle based on 1MHz timer frequency
    uint32_t duty = (duty_ontime_in_servo_period * LEDC_TIMER_MAX_DUTY) / LEDC_TIMER_FREQ_HZ;

    // Set duty cycle
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, channel));
}

void init_pwm(void)
{
    ledc_timer_config_t timer_conf;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num = LEDC_SERVO_TIMER;
    timer_conf.duty_resolution = LEDC_TIMER_13_BIT;
    timer_conf.freq_hz = LEDC_TIMER_FREQ_HZ;
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t channel_conf;
    channel_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    channel_conf.channel = STEERING_SERVO_LEDC_CHANNEL;
    channel_conf.intr_type = LEDC_INTR_DISABLE;
    channel_conf.timer_sel = LEDC_SERVO_TIMER;
    channel_conf.gpio_num = STEERING_SERVO_LEDC_GPIO;
    channel_conf.duty = 0;
    channel_conf.hpoint = 0;
    channel_conf.sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD;

    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));
    channel_conf.channel = THROTTLE_SERVO_LEDC_CHANNEL;
    channel_conf.gpio_num = THROTTLE_SERVO_LEDC_GPIO;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    ESP_LOGI(TAG, "PWM initialized.");
}

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
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}