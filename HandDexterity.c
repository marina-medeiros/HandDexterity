#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

#include "include/ssd1306.h"
#include "include/display.h"
#include "include/leds.h"
#include "include/joystick.h"
#include "include/learn.h"
#include "include/button.h"

volatile TaskHandle_t xActiveTaskHandle = NULL;
volatile TaskHandle_t xMenuTaskHandle   = NULL;

void setup(){
    stdio_init_all();
    setup_display();
    setup_leds();
    setup_joystick();
    setup_button();
}

void vExitTask(void *pvParameters){
    while(true){
        if(button_input()){
            if (xActiveTaskHandle != NULL) {
                vTaskSuspend(xActiveTaskHandle);
                vTaskDelete(xActiveTaskHandle); 
                xActiveTaskHandle = NULL;
                set_leds(0, 0, 0);


                if (xMenuTaskHandle != NULL) {
                    vTaskResume(xMenuTaskHandle);
                }
            }
            while (button_input()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void vLearnTask(void *pvParameters) {
    xActiveTaskHandle = xTaskGetCurrentTaskHandle();

    while (true) {
        learn();
    }
}

void vMenuTask(void *pvParameters) {
    xMenuTaskHandle = xTaskGetCurrentTaskHandle();
    while (true) {
        uint8_t selection = menu_control();

        switch (selection) {
            case 1:
                set_leds(1, 0, 0);
                //xTaskCreate(vTestTask, "Test", 256, NULL, 1, NULL);
                break;
            case 2:
                set_leds(0, 1, 0);
                xTaskCreate(vLearnTask, "Learn", 256, NULL, 1, NULL);
                break;
            case 3:
                set_leds(0, 0, 1);
                //xTaskCreate(vSettingsTask, "Settings", 256, NULL, 1, NULL);
                break;
        }

        vTaskSuspend(NULL);
    }
}


int main(){
    setup();

    xTaskCreate(vExitTask, "Exit", 256, NULL, 2, NULL);
    xTaskCreate(vMenuTask,   "Menu",   512, NULL, 1, NULL);

    vTaskStartScheduler();
    while (true); 
}
