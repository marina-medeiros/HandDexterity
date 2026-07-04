#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/connection/wifi_conn.h"     
#include "include/connection/mqtt_comm.h"
#include "include/connection/mqtt.h"
#include "include/connection/secrets.h"
#include "include/test.h"
#include "include/leds.h"

void setup_connection(){
    connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);
    printf("Connected to Wi-Fi!\n");
    mqtt_setup(MQTT_CLIENT_ID, MQTT_BROKER_IP, MQTT_USER, MQTT_PASSWORD);
}

void publish_with_mqtt(TestResult results[2]){
    if (!mqtt_comm_is_connected()) {
        printf("MQTT ainda não conectado, publicação cancelada.\n");
        return;
    }
    char payload_rectangle[128];

    snprintf(payload_rectangle, sizeof(payload_rectangle),
        "{\"avg_error\":%.2f,\"accuracy\":%.2f,\"time_ms\":%lu}",
        results[0].average_error,
        results[0].accuracy,
        (unsigned long)results[0].time_ms);

    if(mqtt_comm_publish("UFRN/embarcados/HandDexterity/rectangle", 
                        (const uint8_t *)payload_rectangle, 
                        strlen(payload_rectangle))){
        set_leds(0, 1, 0);
    }

    char payload_triangle[128];

    snprintf(payload_triangle, sizeof(payload_triangle),
        "{\"avg_error\":%.2f,\"accuracy\":%.2f,\"time_ms\":%lu}",
        results[1].average_error,
        results[1].accuracy,
        (unsigned long)results[1].time_ms);

    if(mqtt_comm_publish("UFRN/embarcados/HandDexterity/triangle", 
                        (const uint8_t *)payload_triangle, 
                        strlen(payload_triangle))){
        set_leds(0, 1, 0);
    }
}
