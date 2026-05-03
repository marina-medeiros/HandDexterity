#include "hardware/adc.h"
#include "joystick.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define BTN_A_PIN 5

void settup_button(){
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
}