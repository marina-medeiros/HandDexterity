#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/connection/wifi_conn.h"     
#include "include/connection/mqtt_comm.h"
#include "include/connection/mqtt.h"
#include "include/connection/secrets.h"
#include "include/test.h"
#include "include/leds.h"
#include "FreeRTOS.h"
#include "task.h"


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
    snprintf(traj_topic, sizeof(traj_topic), "UFRN/HandDexterity/test%lu/%s/trajectory", (unsigned long)test_counter, shape_name);
    int ok = mqtt_comm_publish(traj_topic, (const uint8_t *)traj_payload, strlen(traj_payload));

    return ok;
}

void check_publish(int publish_validation){
    if(publish_validation){
        printf("--[PUBLISH] envio de data bem sucedido\n");
        set_leds(0, 1, 0);
    } else {
        printf("--[PUBLISH] Publicação falhou, LED não foi setado\n");
    }
    vTaskDelay(pdMS_TO_TICKS(300));
}


void publish_with_mqtt(TestResult result[2], float sensibility, uint32_t test_counter){
    printf("[PUBLISH] Iniciando publish_with_mqtt...\n");

    char payload[128];
    char topic[50];
    char sensibility_payload[50];
    char sensibility_topic[50];

    snprintf(sensibility_topic, sizeof(sensibility_topic), "UFRN/HandDexterity/test%lu/sensibility", 
            (unsigned long)test_counter);
    snprintf(sensibility_payload, sizeof(sensibility_payload),
            "%.2f", sensibility);

    check_publish(mqtt_comm_publish(sensibility_topic, 
                (const uint8_t *)sensibility_payload, strlen(sensibility_payload)));

    for(int ii = 0; ii < 2; ii++){
        snprintf(payload, sizeof(payload),
            "{\"time_ms\":%lu,\"average_speed\":%.2f,\"accuracy\":%.2f,\"score\":%.2f}",
            (unsigned long)result[ii].time_ms,
            result[ii].average_speed,
            result[ii].accuracy,
            result[ii].score);

        printf("[PUBLISH] Payload formatado: '%s' (len=%zu)\n", payload, strlen(payload));

        const char *shape_name = (result[ii].shape == Rectangle) ? "rectangle" : "triangle";
        snprintf(topic, sizeof(topic), "UFRN/HandDexterity/test%lu/%s/data", 
                (unsigned long)test_counter, shape_name);

        check_publish(mqtt_comm_publish(topic, (const uint8_t *)payload,strlen(payload)));

        check_publish(publish_trajectory(result[ii], test_counter));
    }
    
}