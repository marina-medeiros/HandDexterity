#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "include/ssd1306.h"
#include "include/menu.h"
#include "include/leds.h"
#include "include/joystick.h"

void setup(){
    stdio_init_all();
    setup_display();
    setup_leds();
    setup_joystick();
}

int main(){
    setup();
    menu_control();
}
