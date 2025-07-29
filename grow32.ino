#include "config.h"
#include <Arduino.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "webinterface.h"
#include "storage.h"
// #define NUM_RELAYS 8

// RelayConfig relays[NUM_RELAYS];

void setup() {
  Serial.begin(9600);
  
  if (!storage_init()) {
      Serial.println("Falha ao inicializar SPIFFS!");
  }

  bool relays_loaded = storage_load_relays(relays, NUM_RELAYS);
  bool schedules_loaded = storage_load_schedules();

  if (!relays_loaded) {
      Serial.println("Nenhum dado salvo dos relés, inicializando padrão...");
      for (int i = 0; i < NUM_RELAYS; i++) {
          relays[i].name = "Relay " + String(i + 1);
          relays[i].type = "Led";
          relays[i].wavemaker_mode = -1;
      }
      storage_save_relays(relays, NUM_RELAYS);
  }

  if (!schedules_loaded) {
      Serial.println("Nenhum agendamento salvo, limpando agendamentos...");
      for (int i = 0; i < NUM_RELAYS; i++) {
          scheduleCounts[i] = 0;
          // Opcional: pode também zerar os próprios objetos ScheduleEvent se quiser garantir memória limpa.
          // for (int j = 0; j < MAX_EVENTS; j++) {
          //     schedules[i][j] = {0}; // C++11: zera todos os campos
          // }
      }
      storage_save_schedules();
  }

  if (relays_loaded && schedules_loaded) {
            
      updateRelayHasSchedule();      
      Serial.println("Dados carregados com sucesso de relés e agendamentos!");
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
