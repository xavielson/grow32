#include "webinterface.h"
#include <WiFi.h>
#include "web_routes.h"
#include "config.h" // Sugestão: mover SSID/senha para config.h

// Caso queira manter hardcoded, ok. Mas melhor em config.h:
#ifndef WIFI_SSID
#define WIFI_SSID "MLPX2"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "plx54321"
#endif

/**
 * Inicializa WiFi e backend HTTP (bloqueante até conectar).
 */
void webinterface_setup() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Conectando ao WiFi...");
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 60) { // Tenta por até 30s
        delay(500);
        Serial.print(".");
        retry++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado!");
        Serial.print("Endereço IP: ");
        Serial.println(WiFi.localIP());
        setupWebRoutes();  // Configura todas as rotas HTTP
    } else {
        Serial.println("\nFalha ao conectar no WiFi!");
        // Aqui poderia implementar fallback (ex: AP Mode)
    }
}

/**
 * Executa o loop do WebServer (chamar frequentemente no loop principal).
 */
void webinterface_loop() {
    handleWebServer();
}
