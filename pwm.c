#include "pico/stdlib.h"
#include "hardware/pwm.h"
#define BUTTON_A 5

void set_pwm_duty(uint slice_num, uint channel, uint duty_cycle) {
    pwm_set_chan_level(slice_num, channel, duty_cycle);
}