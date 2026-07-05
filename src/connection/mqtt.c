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

void publish_with_mqtt(TestResult result){
    printf("[PUBLISH] Iniciando publish_with_mqtt...\n");
    printf("[PUBLISH] average_error = %f\n", result.average_error);

    char payload[128];
    char topic[50];

    snprintf(payload, sizeof(payload),
        "{\"avg_error\":%.2f,\"accuracy\":%.2f,\"time_ms\":%lu}",
        result.average_error,
        result.accuracy,
        (unsigned long)result.time_ms);

    printf("[PUBLISH] Payload formatado: '%s' (len=%zu)\n", payload, strlen(payload));

    const char *shape_name = (result.shape == Rectangle) ? "rectangle" : "triangle";
    snprintf(topic, sizeof(topic), "UFRN/embarcados/HandDexterity/%s", shape_name);

    int ok = mqtt_comm_publish(topic, 
                                (const uint8_t *)payload, 
                                strlen(payload));

    printf("[PUBLISH] mqtt_comm_publish retornou: %d\n", ok);

    if(ok){
        set_leds(0, 1, 0);
    } else {
        printf("[PUBLISH] Publicação falhou, LED não foi setado\n");
    }
}