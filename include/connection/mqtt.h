#ifndef MQTT_H
#define MQTT_H

#include "include/test.h"

#define MAX_LINE 256

void setup_connection();
void publish_with_mqtt(TestResult result[2], float sensibility, uint32_t test_counter);
int publish_trajectory(TestResult result, uint32_t test_counter);

#endif