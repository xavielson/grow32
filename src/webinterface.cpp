#include "webinterface.h"
#include <WiFi.h>
#include "web_routes.h"
#include "config.h" 


void startAP() {
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

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
        startAP(); // Fallback AP Mode
        setupWebRoutes();
    }
}

/**
 * Executa o loop do WebServer (chamar frequentemente no loop principal).
 */
void webinterface_loop() {
    handleWebServer();
}
