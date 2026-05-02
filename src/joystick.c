#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "include/ssd1306.h"
#include "include/display.h"
#include "include/leds.h"

#define SW 22  //Pino do Botão do Joystick
#define VRY 26 //Porta ADC de variação do Y do Joystick
#define VRX 27 //Porta ADC de variação do X do joystick


void setup_joystick(){
    //inicialização do botão do joystick
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);

    //Inicializar ADC do joystick
    adc_init();
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);
}

void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(0);
    sleep_us(2);
    *vrx_value = adc_read();

    adc_select_input(1);
    sleep_us(2);
    *vry_value = adc_read();
}


void menu_control(){ 
    uint countdown = 0; //verificar seleções para baixo do joystick
    uint countup = 2; //verificar seleções para cima do joystick
    uint pos_y=12; //inicialização de variável para ler posição do Y do Joystick
    uint posy_ant=12; 
    uint menu=1;
    
    print_menu(pos_y);

    while (true) {
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max; 

        if(bar_y_pos < 20 && countdown <2){
            pos_y+=12;
            countdown+=1;
            countup-=1;
            menu++;
        }else
            if(bar_y_pos > 20 && countup <2){
                pos_y-=12;
                countup+=1;
                countdown-=1;
                menu--;
        } 
        if(pos_y!=posy_ant){ 
            print_menu(pos_y);
        }
        if(gpio_get(SW) == 0){
            switch (menu){
            case 1:
                set_leds(1,0,0);
            break;
            case 2:
                set_leds(0,1,0);
            break;
            case 3:
                set_leds(0,0,1);
            break;
            default:
                set_leds(0,0,0);
            break;
            }
       }
        sleep_ms(100);
        posy_ant=pos_y;

    }
}