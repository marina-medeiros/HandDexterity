#include <stdint.h>
#include "lwip/apps/mqtt.h"
#include "../../include/connection/mqtt_comm.h"
#include "../../include/connection/lwipopts.h"
#include <pico/stdlib.h>

static mqtt_client_t *client;
static volatile bool mqtt_connected = false;

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    printf("[MQTT] Callback de conexão disparado. Status: %d\n", status);
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("[MQTT] Conectado ao broker com sucesso!\n");
        mqtt_connected = true;
    } else {
        printf("[MQTT] Falha ao conectar ao broker. Código de status: %d\n", status);
        mqtt_connected = false;
    }
}

bool mqtt_comm_is_connected(void) {
    return mqtt_connected;
}

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;

    printf("[MQTT] Iniciando mqtt_setup...\n");
    printf("[MQTT] client_id=%s broker_ip=%s user=%s\n", client_id, broker_ip, user);

    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("[MQTT] ERRO: broker_ip '%s' não é um IP numérico válido\n", broker_ip);
        return;
    }
    printf("[MQTT] IP do broker convertido com sucesso: %s\n", ip4addr_ntoa(&broker_addr));

    client = mqtt_client_new();
    if (client == NULL) {
        printf("[MQTT] ERRO: mqtt_client_new() retornou NULL (sem memória?)\n");
        return;
    }
    printf("[MQTT] Cliente MQTT criado. Ponteiro: %p\n", (void*)client);

    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,
        .client_user = user,
        .client_pass = pass
    };

    printf("[MQTT] Chamando mqtt_client_connect...\n");
    err_t connect_result = mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
    printf("[MQTT] mqtt_client_connect retornou: %d (ERR_OK=0)\n", connect_result);
}

static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("[MQTT] Publicação confirmada com sucesso!\n");
    } else {
        printf("[MQTT] ERRO ao confirmar publicação: %d\n", result);
    }
}

int mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    printf("[MQTT] Tentando publicar. Conectado? %s | topic=%s | len=%zu\n",
           mqtt_connected ? "SIM" : "NAO", topic, len);

    if (client == NULL) {
        printf("[MQTT] ERRO: client é NULL, mqtt_setup não foi chamado ou falhou\n");
        return 0;
    }

    err_t status = mqtt_publish(
        client, topic, data, len, 0, 1, mqtt_pub_request_cb, NULL
    );

    printf("[MQTT] mqtt_publish retornou: %d\n", status);

    if (status != ERR_OK) {
        printf("[MQTT] mqtt_publish falhou ao ser enviada: %d\n", status);
        return 0;
    }

    return 1;
}