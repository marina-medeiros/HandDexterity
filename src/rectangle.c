#include <math.h>
#include <stdio.h>

#include "include/display.h"
#include "include/joystick.h"
#include "FreeRTOS.h"
#include "projdefs.h"
#include "task.h"
#include "test.h"
#include "rectangle.h"

Point rect_trajectory[MAX_SAMPLES];
int rect_sample_count;

float distance_to_rectangle_perimeter(int px, int py) {
    int x_min = TARGET_X;
    int x_max = TARGET_X + TARGET_W;
    int y_min = TARGET_Y;
    int y_max = TARGET_Y + TARGET_H;

    int dx = 0;
    if (px < x_min) dx = x_min - px;
    else if (px > x_max) dx = px - x_max;

    int dy = 0;
    if (py < y_min) dy = y_min - py;
    else if (py > y_max) dy = py - y_max;

    if (dx > 0 || dy > 0) {
        return sqrtf((float)(dx * dx + dy * dy));
    } else {
        int dist_left = px - x_min;
        int dist_right = x_max - px;
        int dist_top = py - y_min;
        int dist_bottom = y_max - py;

        int min_x = (dist_left < dist_right) ? dist_left : dist_right;
        int min_y = (dist_top < dist_bottom) ? dist_top : dist_bottom;

        return (float)((min_x < min_y) ? min_x : min_y);
    }
}

TestResult rectangle_test() {
    uint16_t adc_x_raw, adc_y_raw;
    RectTraceState current_state = RECT_STATE_WAIT_START;
    
    rect_sample_count = 0;

    TickType_t start_tick = xTaskGetTickCount();

    const int center_x = ADC_MAX / 2;
    const int center_y = ADC_MAX / 2;
    
    const int screen_cx = (SCREEN_W - 2) / 2;
    const int screen_cy = (SCREEN_H - 2) / 2;

    int blink_timer = 0; 

    while (current_state != RECT_STATE_FINISHED && rect_sample_count < MAX_SAMPLES) {
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int delta_x = adc_x_raw - center_x;
        int delta_y = center_y - adc_y_raw;

        int sq_x = screen_cx + (int)(delta_x * sensibility * screen_cx / center_x);
        int sq_y = screen_cy + (int)(delta_y * sensibility * screen_cy / center_y);

        if (sq_x < 0) sq_x = 0;
        if (sq_x > SCREEN_W - 2) sq_x = SCREEN_W - 2;
        if (sq_y < 0) sq_y = 0;
        if (sq_y > SCREEN_H - 2) sq_y = SCREEN_H - 2;

        if (current_state != RECT_STATE_WAIT_START) {
            if (rect_sample_count < MAX_SAMPLES) {
                rect_trajectory[rect_sample_count].x = (uint8_t)sq_x;
                rect_trajectory[rect_sample_count].y = (uint8_t)sq_y;
                rect_sample_count++;
            }
        }

        switch (current_state) {
            case RECT_STATE_WAIT_START:
                if (is_in_checkpoint(sq_x, sq_y, TL_X, TL_Y)) current_state = RECT_STATE_AT_TL;
                break;
            case RECT_STATE_AT_TL:
                if (is_in_checkpoint(sq_x, sq_y, TR_X, TR_Y)) current_state = RECT_STATE_AT_TR;
                break;
            case RECT_STATE_AT_TR:
                if (is_in_checkpoint(sq_x, sq_y, BR_X, BR_Y)) current_state = RECT_STATE_AT_BR;
                break;
            case RECT_STATE_AT_BR:
                if (is_in_checkpoint(sq_x, sq_y, BL_X, BL_Y)) current_state = RECT_STATE_AT_BL;
                break;
            case RECT_STATE_AT_BL:
                if (is_in_checkpoint(sq_x, sq_y, TL_X, TL_Y)) current_state = RECT_STATE_FINISHED;
                break;
            default: break;
        }

        ssd1306_clear(&disp);
        ssd1306_draw_empty_square(&disp, TARGET_X, TARGET_Y, TARGET_W, TARGET_H);
        
        
        blink_timer++;
        if (blink_timer % 10 < 5) {
            int blink_x = -1;
            int blink_y = -1;

            switch (current_state) {
                case RECT_STATE_WAIT_START:
                    blink_x = TL_X; blink_y = TL_Y;
                    break;
                case RECT_STATE_AT_TL:
                    blink_x = TR_X; blink_y = TR_Y;
                    break;
                case RECT_STATE_AT_TR:
                    blink_x = BR_X; blink_y = BR_Y;
                    break;
                case RECT_STATE_AT_BR:
                    blink_x = BL_X; blink_y = BL_Y;
                    break;
                case RECT_STATE_AT_BL:
                    blink_x = TL_X; blink_y = TL_Y;
                case RECT_STATE_FINISHED:
                    break;
            }

            if (blink_x != -1) {
                ssd1306_draw_empty_square(&disp, blink_x - CHECKPOINT_RADIUS, blink_y - CHECKPOINT_RADIUS, CHECKPOINT_RADIUS * 2, CHECKPOINT_RADIUS * 2);
            }
        }
        

        ssd1306_draw_empty_square(&disp, sq_x, sq_y, 3, 3);
        ssd1306_show(&disp); 

        vTaskDelay(pdMS_TO_TICKS(20));
    }

    TickType_t end_tick = xTaskGetTickCount();
    uint32_t elapsed_ms = (end_tick - start_tick) * portTICK_PERIOD_MS;

    TestResult result;

    char msg_result[20];
    char msg_time[20];

    ssd1306_clear(&disp);

    if (current_state == RECT_STATE_FINISHED) {
        result = calculate_result(rect_trajectory, rect_sample_count, Rectangle, elapsed_ms);

        sprintf(msg_result, "result: %.1f%%", result.accuracy);
        sprintf(msg_time, "Time: %.1fs", result.time_ms / 1000.f);

        print_texto(15, 30, 1, msg_result);
        print_texto(15, 40, 1, msg_time);
    } else {
        sprintf(msg_result, "Time out!");
        print_texto(30, 30, 1, msg_result);
    }

    ssd1306_show(&disp);

    vTaskDelay(pdMS_TO_TICKS(2000));

    return result;
}