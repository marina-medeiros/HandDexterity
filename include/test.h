#ifndef TEST_H
#define TEST_H

#define SCREEN_W   120
#define SCREEN_H   60
#define ADC_MAX   4095

typedef struct{
    uint8_t x;
    uint8_t y;

} coord;

typedef struct{
    uint8_t time_start;
    uint8_t time_ent;
    coord sensor_readings[1000]; 

} test_data;

extern float sensibility;

void learn();
void test();
test_data test_block();
void main_test();

#endif