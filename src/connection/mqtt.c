#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/connection/wifi_conn.h"     
#include "include/connection/mqtt_comm.h"
#include "include/connection/mqtt.h"
#include "include/test.h"
#include "include/leds.h"
#include "include/connection/secrets.h"


void setup_connection(){
    printf("[SETUP] Iniciando setup_connection...\n");

    printf("[SETUP] Chamando connect_to_wifi...\n");
    bool wifi_ok = connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);

    if (!wifi_ok) {
        printf("[SETUP] Wi-Fi falhou. Abortando setup_connection sem chamar mqtt_setup.\n");
        return;
    }

    printf("[SETUP] Wi-Fi OK. Chamando mqtt_setup...\n");
    mqtt_setup(MQTT_CLIENT_ID, MQTT_BROKER_IP, NULL, NULL);
    printf("[SETUP] mqtt_setup retornou\n");
}

int publish_trajectory(TestResult result, uint32_t test_counter){
    char traj_payload[900];
    int offset = 0;
    offset += snprintf(traj_payload + offset, sizeof(traj_payload) - offset, "[");

    int step = (result.sample_count > 50) ? result.sample_count / 50 : 1;
    for (int i = 0; i < result.sample_count && offset < sizeof(traj_payload) - 20; i += step) {
        offset += snprintf(traj_payload + offset, sizeof(traj_payload) - offset,
                            "%s[%u,%u]", (i == 0) ? "" : ",", result.trajectory[i].x, result.trajectory[i].y);
    }
    offset += snprintf(traj_payload + offset, sizeof(traj_payload) - offset, "]");

    char traj_topic[64];
    const char *shape_name = (result.shape == Rectangle) ? "rectangle" : "triangle";
    snprintf(traj_topic, sizeof(traj_topic), "DIMAP/embarcados/HandDexterity/test%lu/%s/trajectory", (unsigned long)test_counter, shape_name);
    int ok = mqtt_comm_publish(traj_topic, (const uint8_t *)traj_payload, strlen(traj_payload));

    return ok;
}

void publish_with_mqtt(TestResult result, uint32_t test_counter){
    printf("[PUBLISH] Iniciando publish_with_mqtt...\n");
    printf("[PUBLISH] time_ms = %lu\n", (unsigned long)result.time_ms);
    printf("[PUBLISH] average_speed = %f\n", result.average_speed);
    printf("[PUBLISH] accuracy = %f\n", result.accuracy);
    printf("[PUBLISH] score = %f\n", result.score);
    printf("[PUBLISH] sample_count = %d\n", result.sample_count);
    printf("[PUBLISH] shape = %s\n", (result.shape == Rectangle) ? "rectangle" : "triangle");

    char payload[128];
    char topic[50];

    snprintf(payload, sizeof(payload),
        "{\"time_ms\":%lu,\"average_speed\":%.2f,\"accuracy\":%.2f,\"score\":%.2f}",
        (unsigned long)result.time_ms,
        result.average_speed,
        result.accuracy,
        result.score);

    printf("[PUBLISH] Payload formatado: '%s' (len=%zu)\n", payload, strlen(payload));

    const char *shape_name = (result.shape == Rectangle) ? "rectangle" : "triangle";
    snprintf(topic, sizeof(topic), "DIMAP/HandDexterity/test%lu/%s/data", 
             (unsigned long)test_counter, shape_name);

    int ok = mqtt_comm_publish(topic, 
                                (const uint8_t *)payload, 
                                strlen(payload));


    if(ok){
        printf("[PUBLISH] mqtt_comm_publish bem sucedido");
        set_leds(0, 1, 0);
    } else {
        printf("[PUBLISH] Publicação falhou, LED não foi setado\n");
    }

    int ok_trajectory = publish_trajectory(result, test_counter);

    printf("[PUBLISH] publish_trajectory retornou: %d\n", ok);

    if(ok){
        set_leds(0, 1, 0);
    } else {
        printf("[PUBLISH] Publicação  da tragetória falhou, LED não foi setado\n");
    }
}