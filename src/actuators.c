#include "actuators.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO_STEER_ANGLE_SERVO (18)
#define LEDC_CHANNEL_STEER_ANGLE            LEDC_CHANNEL_0
#define LEDC_OUTPUT_IO_THROTTLE (19)
#define LEDC_CHANNEL_THROTTLE LEDC_CHANNEL_1
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
#define LEDC_MAX_DUTY           (1 << 13) - 1
#define LEDC_FREQUENCY          (1000 * 10) 

void init_pwm(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_STEER_ANGLE,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO_STEER_ANGLE_SERVO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ledc_channel.channel = LEDC_CHANNEL_THROTTLE;
    ledc_channel.gpio_num = LEDC_OUTPUT_IO_THROTTLE;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void control_servo(ledc_channel_t channel, float ontime_percent)
{
    const uint32_t MIN_SERVO_DUTY_US = 1000;  // 1ms pulse width (0°)
    const uint32_t MAX_SERVO_DUTY_US = 2000;  // 2ms pulse width (180°)
    const uint32_t SERVO_DUTY_US_RANGE = MAX_SERVO_DUTY_US - MIN_SERVO_DUTY_US;
    const uint32_t SERVO_PERIOD_US = 50 * 1000;

    // Convert percentage to pulse width (microseconds)
    uint32_t duty_us = MIN_SERVO_DUTY_US + (ontime_percent * SERVO_DUTY_US_RANGE);
    const float duty_ontime_in_servo_period = (float)(duty_us) / (float)(SERVO_PERIOD_US);

    // Convert microseconds to duty cycle based on 1MHz timer frequency
    uint32_t duty = (duty_ontime_in_servo_period * LEDC_MAX_DUTY) / LEDC_FREQUENCY;

    // Set duty cycle
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, channel));
}
