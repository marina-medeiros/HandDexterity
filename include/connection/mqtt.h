#ifndef MQTT_H
#define MQTT_H

#include "include/test.h"

#define MAX_LINE 256

void setup_connection();
void publish_with_mqtt(TestResult results[2]);

#endif