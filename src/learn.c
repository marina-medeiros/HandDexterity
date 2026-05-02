#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "include/display.h"
#include "include/joystick.h"
#include "include/learn.h"

#define SQUARE_SIZE 8
#define SCREEN_W    128
#define SCREEN_H    64

void learn(void) {
    ssd1306_clear(&disp);
    int sq_x = (SCREEN_W - SQUARE_SIZE) / 2;
    int sq_y = (SCREEN_H - SQUARE_SIZE) / 2;

    while (true) {
        // Lê eixo X (ADC1 = GPIO27)
        adc_select_input(1);
        uint adc_x_raw = adc_read();

        // Lê eixo Y (ADC0 = GPIO26)
        adc_select_input(0);
        uint adc_y_raw = adc_read();

        // Centro ~2048; zona morta de ±300 para evitar deriva
        const int centro = 2048;
        const int zona_morta = 300;
        const int velocidade = 2;

        if (adc_x_raw < centro - zona_morta) sq_x -= velocidade;
        if (adc_x_raw > centro + zona_morta) sq_x += velocidade;
        if (adc_y_raw < centro - zona_morta) sq_y -= velocidade;
        if (adc_y_raw > centro + zona_morta) sq_y += velocidade;

        // Limita às bordas do display
        if (sq_x < 0) sq_x = 0;
        if (sq_y < 0) sq_y = 0;
        if (sq_x > SCREEN_W - SQUARE_SIZE) sq_x = SCREEN_W - SQUARE_SIZE;
        if (sq_y > SCREEN_H - SQUARE_SIZE) sq_y = SCREEN_H - SQUARE_SIZE;

        // Redesenha
        ssd1306_clear(&disp);
        print_retangulo(sq_x, sq_y,
                        sq_x + SQUARE_SIZE,
                        sq_y + SQUARE_SIZE);

        sleep_ms(30);
    }
}