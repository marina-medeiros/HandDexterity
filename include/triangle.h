#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "test.h"

#define P1_X 64
#define P1_Y 20

#define P2_X 88
#define P2_Y 46

#define P3_X 40
#define P3_Y 46

typedef enum {
    TRI_STATE_WAIT_START = 0,
    TRI_STATE_AT_P1,
    TRI_STATE_AT_P2,
    TRI_STATE_AT_P3,
    TRI_STATE_FINISHED
} TriTraceState;

float distance_to_triangle_perimeter(int px, int py);
TestResult triangle_test();

#endif