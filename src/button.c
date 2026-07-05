#include "hardware/adc.h"
#include "include/joystick.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define BTN_A_PIN 5

void setup_button(){
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
}

int button_input(){
    if(gpio_get(BTN_A_PIN) == 0){
        return 1;
    }
    return 0;
}