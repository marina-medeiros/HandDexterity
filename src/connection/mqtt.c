#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/connection/wifi_conn.h"     
#include "include/connection/mqtt_comm.h"
#include "include/connection/mqtt.h"

int load_config(const char *filename, AppConfig *prefs) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '#') {
            continue;
        }

        line[strcspn(line, "\r\n")] = 0;

        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (key != NULL && value != NULL) {
            if (strcmp(key, "wifi_ssid") == 0) {
                strncpy(prefs->wifi_ssid, value, sizeof(prefs->wifi_ssid) - 1);
            }
            else if (strcmp(key, "wifi_password") == 0) {
                strncpy(prefs->wifi_password, value, sizeof(prefs->wifi_password) - 1);
            } 
            else if (strcmp(key, "client_id") == 0) {
                strncpy(prefs->client_id, value, sizeof(prefs->client_id) - 1);
            } 
            else if (strcmp(key, "broker_ip") == 0) {
                strncpy(prefs->broker_ip, value, sizeof(prefs->broker_ip) - 1);
            } 
            else if (strcmp(key, "mqtt_user") == 0) {
                strncpy(prefs->mqtt_user, value, sizeof(prefs->mqtt_user) - 1);
            } 
            else if (strcmp(key, "mqtt_password") == 0) {
                strncpy(prefs->mqtt_password, value, sizeof(prefs->mqtt_password) - 1);
            } 
        }
    }

    fclose(file);
    return 1;
}

void setup_connection(){
    AppConfig config = {"wifi_ssid", 
                        "wifi_password", 
                        "client_id", 
                        "broker_ip", 
                        "mqtt_user",
                        "mqtt_password"}; 

    if (load_config("config.txt", &config)) {
        printf("Config loaded.\n");
    } else {
        printf("Could not open config.txt. Using defaults.\n");
    }

    connect_to_wifi(config.wifi_ssid, config.wifi_password);
    mqtt_setup(config.client_id, 
               config.broker_ip, 
               config.mqtt_user, 
               config.mqtt_password);
}

void publish_with_mqtt(){
    int a = 0;
}