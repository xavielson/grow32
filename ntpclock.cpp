#include "ntpclock.h"
#include <time.h>
#include <Arduino.h>

void ntp_setup() {
  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Aguardando NTP...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nNTP OK");
}

time_t ntp_now() {
  return time(nullptr);
}
