#include "scheduler.h"
#include "ntpclock.h"
#include "relay.h"

ScheduleEvent schedules[NUM_RELAYS][MAX_EVENTS];
int scheduleCounts[NUM_RELAYS] = {0};

void scheduler_setup() {
  // Inicialização se necessário
}

void scheduler_loop() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 500) {
    processSchedules();
    lastCheck = millis();
  }
}

void processSchedules() {
  time_t now = ntp_now();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  int dow = timeinfo.tm_wday; // 0=domingo
  int hora = timeinfo.tm_hour;
  int minu = timeinfo.tm_min;
  int sec = timeinfo.tm_sec;

  for(int i=0; i<NUM_RELAYS; i++) {
    bool liga = false;
    for(int j=0; j<scheduleCounts[i]; j++) {
      ScheduleEvent& ev = schedules[i][j];
      // Se evento é para todos dias (0) ou para o dia atual (ajustando indices)
      if (ev.dayOfWeek==0 || ev.dayOfWeek==((dow==0)?1:dow+1)) {
        bool after_on =
            (hora > ev.h_on) ||
            (hora == ev.h_on && minu > ev.m_on) ||
            (hora == ev.h_on && minu == ev.m_on && sec >= ev.s_on);
        bool before_off =
            (hora < ev.h_off) ||
            (hora == ev.h_off && minu < ev.m_off) ||
            (hora == ev.h_off && minu == ev.m_off && sec < ev.s_off);
        if (after_on && before_off) liga = true;
      }
    }
    // NOVO: só altera se NÃO estiver em modo manual
    if (!relayManual[i]) {
      relay_set(i, liga);
    }
  }
}
