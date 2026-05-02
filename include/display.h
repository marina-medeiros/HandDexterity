#ifndef DISPLAY_H
#define DISPLAY_H

#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA  15
#define I2C_SCL  14

extern ssd1306_t disp;

void setup_display();
void print_texto(int x, int y, int tam, char * msg);
void print_retangulo(int x1, int y1, int x2, int y2);
void print_menu(int pos);
void clear_screen();
void draw_dot(int x, int y);

#endif