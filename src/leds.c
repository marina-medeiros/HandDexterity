#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h"
#include "hardware/adc.h"

#define LED_B 12
#define LED_R 13
#define LED_G 11

void setup_leds(){
    gpio_init(LED_B);
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
}

void set_leds(int R, int G, int B){
    gpio_put(LED_R, R);   
    gpio_put(LED_G, G);   
    gpio_put(LED_B, B);
}