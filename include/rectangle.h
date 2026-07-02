#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "test.h"

#define TARGET_X 39
#define TARGET_Y 14
#define TARGET_W 50
#define TARGET_H 32

#define TL_X (TARGET_X)
#define TL_Y (TARGET_Y)
#define TR_X (TARGET_X + TARGET_W)
#define TR_Y (TARGET_Y)
#define BR_X (TARGET_X + TARGET_W)
#define BR_Y (TARGET_Y + TARGET_H)
#define BL_X (TARGET_X)
#define BL_Y (TARGET_Y + TARGET_H)

typedef enum {
    RECT_STATE_WAIT_START = 0,
    RECT_STATE_AT_TL,
    RECT_STATE_AT_TR,
    RECT_STATE_AT_BR,
    RECT_STATE_AT_BL,
    RECT_STATE_FINISHED
} RectTraceState;

float distance_to_rectangle_perimeter(int px, int py);
TestResult rectangle_test();

#endif