#include "relay.h"
#include <Arduino.h>

const int relayPins[NUM_PHYSICAL] = {16, 17, 18, 19, 21, 22, 23, 25};
bool relayStates[NUM_RELAYS] = {0,0,0,0,0,0,0,0};
bool relayManual[NUM_RELAYS] = {0,0,0,0,0,0,0,0}; // Adicionado: modo manual para cada relé

RelayConfig relays[NUM_RELAYS] = {
  {"Saída 1", "Clique editar para configurar saída"},
  {"Saída 2", "Clique editar para configurar saída"},
  {"Saída 3", "Clique editar para configurar saída"},
  {"Saída 4", "Clique editar para configurar saída"},
  {"Saída 5", "Clique editar para configurar saída"},
  {"Saída 6", "Clique editar para configurar saída"},
  {"Saída 7", "Clique editar para configurar saída"},
  {"Saída 8", "Clique editar para configurar saída"}
};


void relay_setup() {
  for (int i = 0; i < NUM_PHYSICAL; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
  }
}

void relay_set(int idx, bool state) {
  if(idx < 0 || idx >= NUM_RELAYS) return;
  relayStates[idx] = state;
  if(idx < NUM_PHYSICAL) {
    digitalWrite(relayPins[idx], state ? LOW : HIGH);
  }
}

void relay_toggle(int idx) {
    Serial.print("[RELAY_TOGGLE] Recebido idx=");
    Serial.println(idx);
    if (idx < 0 || idx >= NUM_RELAYS) {
        Serial.println("[RELAY_TOGGLE] IDX fora do range!");
        return;
    }
    relayStates[idx] = !relayStates[idx];
    relayManual[idx] = true; // NOVO: ao acionar manualmente, ativa modo manual
    Serial.print("[RELAY_TOGGLE] Estado apos toggle: ");
    Serial.println(relayStates[idx] ? "ON" : "OFF");
    relay_set(idx, relayStates[idx]);
}
