#include "config.h"
#include <Arduino.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "webinterface.h"
#include "storage.h"
#include <SPIFFS.h>

void setup() {
  Serial.begin(9600);
  Wire.begin(21,22);
  rtc.begin();


  
  //Formata storage no boot para iniciar vazio se true (config.h)  
  if(CLEAN_START) {
    format_storage;
  }

  storage_load(relays, NUM_RELAYS);
  relay_setup();
  scheduler_setup();
  webinterface_setup();
  Wire.begin(21,22);
  rtc.begin();
  ntp_or_rtc_setup();

  }

void loop() {

  webinterface_loop();
  scheduler_loop();
}
