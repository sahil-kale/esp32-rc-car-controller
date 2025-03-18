#ifndef ACTUATORS_H
#define ACTUATORS_H

#ifdef __cplusplus
extern "C" {
#endif
#include "driver/ledc.h"


void init_pwm(void);
void set_pwm_duty(int channel, int duty);

#ifdef __cplusplus
}
#endif

#endif // PWM_H