#include "config.h"
#include <Arduino.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "webinterface.h"
#include "storage.h"
#include <SPIFFS.h>

// #define NUM_RELAYS 8

// RelayConfig relays[NUM_RELAYS];

void setup() {
  Serial.begin(9600);
  // if (SPIFFS.begin(true)) {
  //     Serial.println("Apagando SPIFFS...");
  //     SPIFFS.format(); // APAGA TUDO!
  //     Serial.println("SPIFFS apagado.");
  // } else {
  //     Serial.println("Erro ao iniciar SPIFFS");
  // }
  
  if (!storage_init()) {
      Serial.println("Falha ao inicializar SPIFFS!");
  }

  bool loaded = storage_load_all(relays, NUM_RELAYS);

  if (!loaded) {
      Serial.println("Nenhum dado salvo encontrado, inicializando listas vazias...");
  } else {
      updateRelayHasSchedule();
      Serial.println("Dados carregados com sucesso!");
  }

    relay_setup();
    scheduler_setup();
    webinterface_setup();
    ntp_setup();

  }

void loop() {

  webinterface_loop();
  scheduler_loop();
}
