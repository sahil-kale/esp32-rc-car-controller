#include "actuators.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL_SERVO      LEDC_CHANNEL_0
#define LEDC_CHANNEL_THROTTLE   LEDC_CHANNEL_1
#define LEDC_FREQUENCY          50 // Typical for servos (50Hz, 20ms period)
#define SERVO_MIN_PULSE_US      1000 // 1ms
#define SERVO_MAX_PULSE_US      2000 // 2ms
#define SERVO_PERIOD_US         20000 // 20ms

static const char *TAG = "ACTUATORS";

void init_pwm(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t servo_channel = {
        .gpio_num = OUTPUT_IO_STEER_ANGLE_SERVO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_SERVO,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&servo_channel));

    ledc_channel_config_t throttle_channel = {
        .gpio_num = OUTPUT_IO_THROTTLE,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_THROTTLE,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&throttle_channel));
}

void control_servo(ledc_channel_t channel, float ontime_percent) {
    if (ontime_percent < 0.0f || ontime_percent > 1.0f) {
        ESP_LOGE(TAG, "Invalid PWM duty cycle: %f", ontime_percent);
        return;
    }

    uint32_t pulse_width_us = SERVO_MIN_PULSE_US + (uint32_t)(ontime_percent * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US));
    uint32_t duty = (pulse_width_us * ((1 << 16) - 1)) / SERVO_PERIOD_US; // Convert to 16-bit duty cycle

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, channel));

    ESP_LOGI(TAG, "Set channel %d duty to %ld (%.2fms)", channel, duty, pulse_width_us / 1000.0);
}