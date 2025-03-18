#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "driver/ledc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_CHANNEL_SERVO      LEDC_CHANNEL_0
#define LEDC_CHANNEL_THROTTLE   LEDC_CHANNEL_1
#define OUTPUT_IO_STEER_ANGLE_SERVO (26)
#define OUTPUT_IO_THROTTLE (25)


void init_pwm(void);
void control_servo(ledc_channel_t channel, float ontime_percent);

#ifdef __cplusplus
}
#endif

#endif // PWM_H