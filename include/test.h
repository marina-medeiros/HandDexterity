#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdint.h>

#define SCREEN_W   120
#define SCREEN_H   60
#define ADC_MAX   4095

extern float sensibility;

#define MAX_SAMPLES 2000
#define CHECKPOINT_RADIUS 8

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

typedef enum {
    Rectangle,
    Triangle,
} Shape;

typedef struct {
    float average_error;
    float accuracy;
    Point* trajectory;
    int sample_count;
    Shape shape;
    uint32_t time_ms;
} TestResult;

bool is_in_checkpoint(int user_x, int user_y, int target_x, int target_y);
TestResult calculate_score(Point* trajectory, int sample_count, Shape shape, uint32_t time_ms);

void learn();
void test();

#endif