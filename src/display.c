#include "include/display.h"
#include "pico/stdlib.h"

ssd1306_t disp;

void setup_display(){
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&disp);
}

void print_texto(int x, int y, int tam, char * msg){
    ssd1306_draw_string(&disp, x, y, tam, msg);
    ssd1306_show(&disp);
}

void print_tiny_square(uint16_t x, uint16_t y){
    ssd1306_draw_square_inverted(&disp, x, y, 2, 2);
    ssd1306_show(&disp);
}

void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);
}

void print_menu(int pos){
        ssd1306_clear(&disp);
        print_texto(52, 2, 1.5, "Menu");
        print_retangulo(2,pos+2,120,12);
        print_texto(6, 18, 1.9, "Hand Dexterity Test");
        print_texto(6, 30, 1.9, "Learn");
        print_texto(6, 42, 1.9, "Settings");
}

void print_settings(int pos){
        ssd1306_clear(&disp);
        print_texto(10, 2, 1.5, "Joystick Sensibility");
        print_retangulo(2,pos+2,120,12);
        print_texto(30, 18, 1.9, "High");
        print_texto(30, 30, 1.9, "Medium");
        print_texto(30, 42, 1.9, "Low");
}