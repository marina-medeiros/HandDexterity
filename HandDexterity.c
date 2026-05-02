#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "include/ssd1306.h"
#include "include/menu.h"
#include "include/leds.h"
#include "hardware/adc.h"

#define I2C_PORT i2c1
#define I2C_SDA 15
#define I2C_SCL 14
#define SW 22  //Pino do Botão do Joystick
#define VRY 26 //Porta ADC de variação do Y do Joystick
#define VRX 27 //Porta ADC de variação do X do joystick


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
    gpio_init(SW);
    gpio_set_dir(SW, GPIO_IN);
    gpio_pull_up(SW);

    //Inicializar ADC do joystick
    adc_init();
    adc_gpio_init(VRY);
    adc_gpio_init(VRX);


}
 

int main()
{
    setup();
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
