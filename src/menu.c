#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h"
#include "hardware/adc.h"

ssd1306_t disp; 
#define I2C_PORT i2c1

void setup_display(){
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&disp);
}

void print_texto(int x, int y, int tam, char * msg){
    ssd1306_draw_string(&disp, x, y, tam, msg);
    ssd1306_show(&disp);
}
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);
}
void print_menu(int pos){
        ssd1306_clear(&disp);//Limpa a tela
        print_texto(52, 2, 1, "Menu");
        print_retangulo(2,pos+2,120,12);
        print_texto(6, 18, 1.5,"Programa 01");
        print_texto(6, 30, 1.5, "Programa 02");
        print_texto(6, 42, 1.5, "Programa 03");
}