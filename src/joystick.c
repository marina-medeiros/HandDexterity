#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "include/ssd1306.h"
#include "include/display.h"
#include "include/leds.h"
#include "FreeRTOS.h"
#include "task.h"

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

// Usada no módulo de aprendizagem
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(1);
    sleep_us(2);
    *vrx_value = adc_read();

    adc_select_input(0);
    sleep_us(2);
    *vry_value = adc_read();
}

uint8_t menu_control(int menu_type){ 
    uint countdown = 0; //verificar seleções para baixo do joystick
    uint countup = 2; //verificar seleções para cima do joystick
    uint pos_y=12; //inicialização de variável para ler posição do Y do Joystick
    uint posy_ant=12; 
    uint menu=1;

    print_menu(pos_y, menu_type);

    while (true) {
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        const uint bar_width = 40;                        // Escala de conversão
        const uint adc_max = (1 << 12) - 1;               // 2^12 - 1 = 4095
        uint bar_y_pos = adc_y_raw * bar_width / adc_max; // (0..4095) * 40 / 4095 -> valor normalizado em [0, 40]

        // 20 é o valor neutro 0 20 40
        if(bar_y_pos < 10 && countdown <2){
            pos_y+=12;
            countdown+=1;
            countup-=1;
            menu++;
        }else
            if(bar_y_pos > 30 && countup <2){
                pos_y-=12;
                countup+=1;
                countdown-=1;
                menu--;
        } 
        if(pos_y!=posy_ant){ 
            print_menu(pos_y, menu_type);
        }
        if(gpio_get(SW) == 0){
            if(menu > 3 || menu < 1){
                return 0;
            }
            else{
                return menu;
            }
       }
        vTaskDelay(pdMS_TO_TICKS(100));
        posy_ant=pos_y;

    }
}