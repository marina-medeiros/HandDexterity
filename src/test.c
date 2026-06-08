#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "include/display.h"
#include "include/joystick.h"
#include "include/test.h"
#include <stdio.h>

void learn() {
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;

    const int center_x = ADC_MAX / 2;
    const int center_y = ADC_MAX / 2;

    const int screen_cx = (SCREEN_W - 2) / 2;
    const int screen_cy = (SCREEN_H - 2) / 2;

    while (true) {
        ssd1306_clear(&disp);
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int delta_x = adc_x_raw - center_x;
        int delta_y = adc_y_raw - center_y;

        int sq_x = screen_cx + (int)(delta_x * sensibility * screen_cx / center_x);
        int sq_y = screen_cy + (int)(delta_y * sensibility * screen_cy / center_y);

        if (sq_x < 0) sq_x = 0;
        if (sq_x > SCREEN_W - 2) sq_x = SCREEN_W - 2;
        if (sq_y < 0) sq_y = 0;
        if (sq_y > SCREEN_H - 2) sq_y = SCREEN_H - 2;

        print_tiny_square(sq_x, sq_y);
        sleep_ms(30);
    }
}

/*
main_test():
    shape_data []
    for(shape de 1 a 3):
        shape_data.push_back(test_block(shape))

função test_block:
-> printa a forma (shape)
-> ler o movimento e guardar na struct (?)
-> aguardar botão ser pressionado
-> quando o botão for pressionado, retorne a struct
*/

test_data test_block(){
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;
    test_data record;
    

    while (true) {
        ssd1306_clear(&disp);
        print_big_square();
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

void main_test(){
    
}

void test() {
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;

    while (true) {
        ssd1306_clear(&disp);
        print_big_square();
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