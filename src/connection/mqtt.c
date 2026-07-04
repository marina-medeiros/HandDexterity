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
    mqtt_setup(MQTT_CLIENT_ID, MQTT_BROKER_IP, MQTT_USER, MQTT_PASSWORD);
    printf("[SETUP] mqtt_setup retornou\n");
}

void publish_with_mqtt(TestResult results[2]){
    printf("[PUBLISH] Iniciando publish_with_mqtt...\n");
    printf("[PUBLISH] average_error = %f\n", results[0].average_error);

    char payload[32];
    snprintf(payload, sizeof(payload), "%.2f", results[0].average_error);
    printf("[PUBLISH] Payload formatado: '%s' (len=%zu)\n", payload, strlen(payload));

    int ok = mqtt_comm_publish("UFRN/embarcados/HandDexterity/rectangle", 
                        (const uint8_t *)payload, 
                        strlen(payload));

    printf("[PUBLISH] mqtt_comm_publish retornou: %d\n", ok);

    if(ok){
        set_leds(0, 1, 0);
    } else {
        printf("[PUBLISH] Publicação falhou, LED não foi setado\n");
    }
}