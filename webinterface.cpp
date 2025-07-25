#include "webinterface.h"
#include <WiFi.h>
#include "web_routes.h"

const char* ssid = "MLPX2";
const char* password = "plx54321";

void webinterface_setup() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  setupWebRoutes();  // Configura todas as rotas HTTP
}

void webinterface_loop() {
  handleWebServer(); // Loop do WebServer
}
