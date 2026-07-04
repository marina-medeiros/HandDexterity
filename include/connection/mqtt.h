#ifndef MQTT_H
#define MQTT_H

#define MAX_LINE 256

typedef struct {
    char wifi_ssid[32];
    char wifi_password[64];
    char client_id[50];
    char broker_ip[16];
    char mqtt_user[50];
    char mqtt_password[50];
} AppConfig;

int load_config(const char *filename, AppConfig *prefs);
void setup_connection();
void publish_with_mqtt();

#endif