#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "include/display.h"
#include "include/joystick.h"
#include "include/test.h"
#include <stdio.h>

#define SCREEN_W   120
#define SCREEN_H   60
#define ADC_MAX   4095

void learn() {
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;

    while (true) {
        ssd1306_clear(&disp);
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int sq_x = (int)(adc_x_raw * (SCREEN_W - 2) / ADC_MAX);
        int sq_y = (int)(adc_y_raw * (SCREEN_H - 2) / ADC_MAX);

        printf("x: %u    y: %u \n", adc_x_raw, adc_y_raw);
        printf("x: %u    y: %u \n", sq_x, sq_y);

        print_tiny_square(sq_x, sq_y);

        sleep_ms(30);
    }
}

void test() {
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;

    while (true) {
        ssd1306_clear(&disp);
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int sq_x = (int)(adc_x_raw * (SCREEN_W - 2) / ADC_MAX);
        int sq_y = (int)(adc_y_raw * (SCREEN_H - 2) / ADC_MAX);

        printf("x: %u    y: %u \n", adc_x_raw, adc_y_raw);
        printf("x: %u    y: %u \n", sq_x, sq_y);

        print_tiny_square(sq_x, sq_y);
        print_tiny_square(sq_x+10, sq_y+10);

        sleep_ms(30);
    }
}