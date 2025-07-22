#include "relay.h"
#include <Arduino.h>

const int relayPins[NUM_PHYSICAL] = {33, 13, 14};
bool relayStates[NUM_RELAYS] = {0,0,0,0,0,0,0,0};

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
  if(idx < NUM_PHYSICAL) {
    relayStates[idx] = state;
    digitalWrite(relayPins[idx], state ? LOW : HIGH);
  } else if(idx < NUM_RELAYS) {
    relayStates[idx] = state;
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
    Serial.print("[RELAY_TOGGLE] Estado apos toggle: ");
    Serial.println(relayStates[idx] ? "ON" : "OFF");
    // ... resto da sua função ...
}
