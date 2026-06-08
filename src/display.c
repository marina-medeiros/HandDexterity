#include "include/display.h"
#include "pico/stdlib.h"

#include <stdio.h>

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

void print_text(int x, int y, int tam, char * msg){
    ssd1306_draw_string(&disp, x, y, tam, msg);
    ssd1306_show(&disp);
}

void print_tiny_square(uint16_t x, uint16_t y){
    ssd1306_draw_square_inverted(&disp, x, y, 2, 2);
    ssd1306_show(&disp);
}

void print_rectangle(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);
}

void print_square(int x1, int y1, int x2, int y2){
    ssd1306_draw_square(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);
}

void print_menu(int pos_y, int menu_type){
    if(menu_type == 0){
        print_main_menu(pos_y);
    }else{
        print_settings(pos_y);
    }

}

void print_big_square(){
    ssd1306_draw_empty_square(&disp, 30, 7, 70, 50);
    ssd1306_show(&disp);
}


void print_big_triangle(){
    ssd1306_draw_empty_square(&disp, 25, 7, 32, 50);
    ssd1306_show(&disp);
}


void print_big_circle(){
    ssd1306_draw_empty_square(&disp, 10, 7, 70, 50);
    ssd1306_show(&disp);
}

void print_main_menu(int pos_y){
        ssd1306_clear(&disp);
        print_text(52, 2, 1.5, "Menu");
        print_rectangle(2,pos_y+2,120,12);
        print_text(6, 18, 1.9, "Hand Dexterity Test");
        print_text(6, 30, 1.9, "Learn");
        print_text(6, 42, 1.9, "Settings");
}

void print_settings(int pos_y){
        ssd1306_clear(&disp);
        print_text(5, 2, 1.5, "Joystick Sensibility");

        print_square(6, (1.5*pos_y+1), 14, 6);

        print_rectangle(2,15,20,12); 
        print_text(30, 15, 1.9, "High");

        print_rectangle(2,33,20,12);
        print_text(30, 33, 1.9, "Medium");

        print_rectangle(2,51,20,12);
        print_text(30, 51, 1.9, "Low");
}