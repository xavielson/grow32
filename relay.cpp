#include "relay.h"
#include <Arduino.h>

const int relayPins[NUM_PHYSICAL] = {33, 13, 14};
bool relayStates[NUM_RELAYS] = {0,0,0,0,0,0,0,0};
bool relayManual[NUM_RELAYS] = {0,0,0,0,0,0,0,0}; // Adicionado: modo manual para cada relé

RelayConfig relays[NUM_RELAYS] = {
  {"LED 1",       "Led"},
  {"Bomba 1",     "Rega"},
  {"Wavemaker 1", "Wavemaker"},
  {"Runoff 1",    "Runoff"},
  {"Saída 5",     "Led"},
  {"Saída 6",     "Led"},
  {"Saída 7",     "Led"},
  {"Saída 8",     "Led"}
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
