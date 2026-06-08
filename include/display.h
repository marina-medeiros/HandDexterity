#ifndef DISPLAY_H
#define DISPLAY_H

#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA  15
#define I2C_SCL  14

extern ssd1306_t disp;

void setup_display();
void print_text(int x, int y, int tam, char * msg);
void print_tiny_square(uint16_t x, uint16_t y);
void print_rectangle(int x1, int y1, int x2, int y2);
void print_menu(int pos_y, int menu_type);
void print_main_menu(int pos_y);
void print_settings(int pos_y);

#endif