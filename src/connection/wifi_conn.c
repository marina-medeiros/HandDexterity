#include "../../include/connection/wifi_conn.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

bool connect_to_wifi(const char *ssid, const char *password) {
    printf("[WIFI] Iniciando cyw43_arch_init...\n");

    if (cyw43_arch_init()) {
        printf("[WIFI] ERRO: cyw43_arch_init falhou\n");
        return false;
    }
    printf("[WIFI] cyw43_arch_init OK\n");

    cyw43_arch_enable_sta_mode();
    printf("[WIFI] Modo STA habilitado\n");

    printf("[WIFI] Tentando conectar a SSID='%s' (timeout 30000ms)...\n", ssid);

    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000);

    printf("[WIFI] cyw43_arch_wifi_connect_timeout_ms retornou: %d (0 = sucesso)\n", result);

    if (result != 0) {
        printf("[WIFI] ERRO ao conectar. Código: %d\n", result);
        return false;
    }

    printf("[WIFI] Conectado ao Wi-Fi com sucesso!\n");

    // Log extra: mostra o IP obtido via DHCP, útil para confirmar que a rede está de fato operacional
    struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];
    printf("[WIFI] IP obtido: %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));

    return true;
}