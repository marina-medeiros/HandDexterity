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

// bool check_debounce_button() {
//     static uint64_t last_press_time = 0;
//     const uint debounce_time_ms = 50;

//     if (button_input) {
//         uint64_t current_time = to_ms_since_boot(get_absolute_time());
//         if (current_time - last_press_time > debounce_time_ms) {
//             last_press_time = current_time;
//             return true;
//         }
//     }
//     return false;
// }

// void wait_for_button_release() {
//     while (gpio_get(BTN_A_PIN) == 0) {
//         sleep_ms(10);
//     }
// }


// bool debounce_button(){
//     if (debounce_button()) {
//         wait_for_button_release();
//         return true;
//     }
//     return false;
// }