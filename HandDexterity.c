#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "include/ssd1306.h"
#include "include/menu.h"
#include "include/leds.h"
#include "include/joystick.h"

#define I2C_PORT i2c1
#define I2C_SDA 15
#define I2C_SCL 14

void setup(){
    //inicialização do Oled
    stdio_init_all();
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    setup_display();

    //inicializa leds
    setup_leds();

    //inicialização do botão do joystick
    setup_joystick();
}
 

int main(){
    setup();
    menu_control();
}
