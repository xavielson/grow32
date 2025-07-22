#include <Arduino.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "webinterface.h"

void setup() {
  Serial.begin(115200);

  relay_setup();
  scheduler_setup();
  webinterface_setup();
  ntp_setup();
}

void loop() {
  webinterface_loop();
  scheduler_loop();
}
