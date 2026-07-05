#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "include/display.h"
#include "include/joystick.h"
#include "FreeRTOS.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
#include "test.h"
#include "triangle.h"

Point tri_trajectory[MAX_SAMPLES];
int tri_sample_count;

static float point_to_segment_distance(int px, int py, int x1, int y1, int x2, int y2) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    float length_squared = dx * dx + dy * dy;
    
    if (length_squared == 0.0f) {
        float dpx = (float)(px - x1);
        float dpy = (float)(py - y1);
        return sqrtf(dpx * dpx + dpy * dpy);
    }
    
    float t = ((float)(px - x1) * dx + (float)(py - y1) * dy) / length_squared;
    
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    
    float closest_x = (float)x1 + t * dx;
    float closest_y = (float)y1 + t * dy;
    
    float diff_x = (float)px - closest_x;
    float diff_y = (float)py - closest_y;
    
    return sqrtf(diff_x * diff_x + diff_y * diff_y);
}

float distance_to_triangle_perimeter(int px, int py) {
    float dist1 = point_to_segment_distance(px, py, P1_X, P1_Y, P2_X, P2_Y);
    float dist2 = point_to_segment_distance(px, py, P2_X, P2_Y, P3_X, P3_Y);
    float dist3 = point_to_segment_distance(px, py, P3_X, P3_Y, P1_X, P1_Y);
    
    float min_dist = dist1;
    if (dist2 < min_dist) min_dist = dist2;
    if (dist3 < min_dist) min_dist = dist3;
    
    return min_dist;
}

TestResult triangle_test() {
    uint16_t adc_x_raw, adc_y_raw;
    TriTraceState current_state = TRI_STATE_WAIT_START;
    
    tri_sample_count = 0; 

    TickType_t start_tick = xTaskGetTickCount();

    const int center_x = ADC_MAX / 2;
    const int center_y = ADC_MAX / 2;
    
    const int screen_cx = (SCREEN_W - 2) / 2;
    const int screen_cy = (SCREEN_H - 2) / 2;

    int blink_timer = 0; 

    while (current_state != TRI_STATE_FINISHED && tri_sample_count < MAX_SAMPLES) {
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int delta_x = adc_x_raw - center_x;
        int delta_y = center_y - adc_y_raw;

        int sq_x = screen_cx + (int)(delta_x * sensibility * screen_cx / center_x);
        int sq_y = screen_cy + (int)(delta_y * sensibility * screen_cy / center_y);

        if (sq_x < 0) sq_x = 0;
        if (sq_x > SCREEN_W - 2) sq_x = SCREEN_W - 2;
        if (sq_y < 0) sq_y = 0;
        if (sq_y > SCREEN_H - 2) sq_y = SCREEN_H - 2;

        if (current_state != TRI_STATE_WAIT_START) {
            if (tri_sample_count < MAX_SAMPLES) {
                tri_trajectory[tri_sample_count].x = (uint8_t)sq_x;
                tri_trajectory[tri_sample_count].y = (uint8_t)sq_y;
                tri_sample_count++;
            }
        }

        switch (current_state) {
            case TRI_STATE_WAIT_START:
                if (is_in_checkpoint(sq_x, sq_y, P1_X, P1_Y)) current_state = TRI_STATE_AT_P1;
                break;
            case TRI_STATE_AT_P1:
                if (is_in_checkpoint(sq_x, sq_y, P2_X, P2_Y)) current_state = TRI_STATE_AT_P2;
                break;
            case TRI_STATE_AT_P2:
                if (is_in_checkpoint(sq_x, sq_y, P3_X, P3_Y)) current_state = TRI_STATE_AT_P3;
                break;
            case TRI_STATE_AT_P3:
                if (is_in_checkpoint(sq_x, sq_y, P1_X, P1_Y)) current_state = TRI_STATE_FINISHED;
                break;
            default: break;
        }

        ssd1306_clear(&disp);
        
        ssd1306_draw_line(&disp, P1_X, P1_Y, P2_X, P2_Y);
        ssd1306_draw_line(&disp, P3_X, P3_Y, P2_X, P2_Y);
        ssd1306_draw_line(&disp, P3_X, P3_Y, P1_X, P1_Y);
        
        blink_timer++;
        if (blink_timer % 10 < 5) {
            int blink_x = -1;
            int blink_y = -1;

            switch (current_state) {
                case TRI_STATE_WAIT_START:
                    blink_x = P1_X; blink_y = P1_Y;
                    break;
                case TRI_STATE_AT_P1:
                    blink_x = P2_X; blink_y = P2_Y;
                    break;
                case TRI_STATE_AT_P2:
                    blink_x = P3_X; blink_y = P3_Y;
                    break;
                case TRI_STATE_AT_P3:
                    blink_x = P1_X; blink_y = P1_Y;
                    break;
                case TRI_STATE_FINISHED:
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

    if (current_state == TRI_STATE_FINISHED) {
        result = calculate_result(tri_trajectory, tri_sample_count, Triangle, elapsed_ms);

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