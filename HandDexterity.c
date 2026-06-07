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
#include "include/test.h"
#include "include/button.h"

volatile TaskHandle_t xActiveTaskHandle = NULL;
volatile TaskHandle_t xMenuTaskHandle   = NULL;

float sensibility = 1.5f;

enum MenuType{
    MENU, 
    SETTINGS
};

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

void vTestTask(void *pvParameters) {
    xActiveTaskHandle = xTaskGetCurrentTaskHandle();

    while (true) {
        test();
    }
}

void vSettingsTask(void *pvParameters){
    xActiveTaskHandle = xTaskGetCurrentTaskHandle();
    while (true) {
        uint8_t selection = menu_control(SETTINGS);

        switch (selection) {
            case 1:
                sensibility = 3.0f;
                break;
            case 2:
                sensibility = 1.5f;
                break;
            case 3:
                sensibility = 0.7f;
                break;
            default:
                sensibility = 1.5f;
        }
    }
}

void vMenuTask(void *pvParameters) {
    xMenuTaskHandle = xTaskGetCurrentTaskHandle();
    while (true) {
        uint8_t selection = menu_control(MENU);

        switch (selection) {
            case 1:
                xTaskCreate(vTestTask, "Test", 256, NULL, 1, NULL);
                break;
            case 2:
                xTaskCreate(vLearnTask, "Learn", 256, NULL, 1, NULL);
                break;
            case 3:
                xTaskCreate(vSettingsTask, "Settings", 256, NULL, 1, NULL);
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
