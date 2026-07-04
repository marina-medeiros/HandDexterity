#ifndef MQTT_COMM_H
#define MQTT_COMM_H
#include <stdint.h>
#include <stdbool.h>
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
int mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
bool mqtt_comm_is_connected(void);
#endif