#include "include/display.h"
#include "include/joystick.h"
#include "include/test.h"
#include "rectangle.h"
#include "FreeRTOS.h"
#include "task.h"
#include "triangle.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

bool is_in_checkpoint(int user_x, int user_y, int target_x, int target_y) {
    return (abs(user_x - target_x) <= CHECKPOINT_RADIUS) && 
           (abs(user_y - target_y) <= CHECKPOINT_RADIUS);
}

void learn() {
    ssd1306_clear(&disp);
    uint16_t adc_x_raw, adc_y_raw;

    const int center_x = ADC_MAX / 2;
    const int center_y = ADC_MAX / 2;

    const int screen_cx = (SCREEN_W - 2) / 2;
    const int screen_cy = (SCREEN_H - 2) / 2;

    while (true) {
        ssd1306_clear(&disp);
        joystick_read_axis(&adc_x_raw, &adc_y_raw);

        int delta_x = adc_x_raw - center_x;
        int delta_y = adc_y_raw - center_y;

        int sq_x = screen_cx + (int)(delta_x * sensibility * screen_cx / center_x);
        int sq_y = screen_cy + (int)(delta_y * sensibility * screen_cy / center_y);

        if (sq_x < 0) sq_x = 0;
        if (sq_x > SCREEN_W - 2) sq_x = SCREEN_W - 2;
        if (sq_y < 0) sq_y = 0;
        if (sq_y > SCREEN_H - 2) sq_y = SCREEN_H - 2;

        print_tiny_square(sq_x, sq_y);
        sleep_ms(30);
    }
}

float distance_to_perimeter(int px, int py, Shape shape) {
    switch (shape) {
        case Rectangle:
            return distance_to_rectangle_perimeter(px, py);
            break;
        case Triangle:
            return distance_to_triangle_perimeter(px, py);
            break;
        }
}

TestResult calculate_result(Point* trajectory, int sample_count, Shape shape, uint32_t time_ms) {
    if (sample_count == 0) return (TestResult){0, 0, 0, 0, NULL, shape, 0};

    float total_squared_error = 0.0f;

    const float TOLERANCE = 2.0f;

    for (int i = 0; i < sample_count; i++) {
        float err = distance_to_perimeter(
            trajectory[i].x, trajectory[i].y, shape
        );

        err -= TOLERANCE;

        if (err < 0.0f) {
            err = 0.0f;
        }

        total_squared_error += (err * err);
    }

    float rms_error = sqrt(total_squared_error / sample_count);

    const float K_TUNING_ACC = 0.15f;
    float accuracy = 100.0f * expf(-K_TUNING_ACC * rms_error);
    float normalized_accuracy = accuracy / 100.0f;

    const float K_TUNING_SPEED = 0.03f;
    float average_speed = sample_count / (time_ms / 1000);
    float normalized_speed = 1 - expf(K_TUNING_SPEED * average_speed * -1);

    float score = 100.0f * powf(normalized_accuracy, 0.7) * powf(normalized_speed, 0.3);

    printf("\n--- RESULTS ---\n");
    printf("RMS Deviation: %.2f pixels\n", rms_error);
    printf("Normalized acc: %.2f\n", normalized_accuracy);
    printf("Final Accuracy: %.1f%%\n", accuracy);
    printf("Normalized speed: %.2f\n", normalized_speed);
    printf("Sample count: %d\n", sample_count);
    printf("Average speed: %.2f\n", average_speed);
    printf("Total Trajectory Points: %d\n", sample_count);
    printf("\n\n\n\n");

    TestResult result = {
        .time_ms = time_ms,
        .average_speed = average_speed,
        .accuracy = accuracy,
        .score = score,
        .trajectory = trajectory,
        .shape = shape,
        .sample_count = sample_count,
    };

    return result;
}

void test(TestResult results[2]) {
    results[0] = rectangle_test();
    results[1] = triangle_test();
}