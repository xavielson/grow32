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

void processSchedules() { // Loop de atualização
  time_t now = ntp_now();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  int dow = timeinfo.tm_wday; // 0=domingo
  int hora = timeinfo.tm_hour;
  int minu = timeinfo.tm_min;
  int sec = timeinfo.tm_sec;

  int agora = hora * 3600 + minu * 60 + sec;

  for(int i=0; i<NUM_RELAYS; i++) {
    if (relays[i].type == "Wavemaker" && relays[i].wavemaker_mode >= 0) {
      if (!relayManual[i]) {
        // Executa a lógica do modo selecionado:
        bool liga = false;
        switch (relays[i].wavemaker_mode) {
          case 0: // Sempre ligado
            liga = true; break;
          case 1: // 15min ligado/15min desligado
            liga = (minu % 30) < 15; break;
          case 2: // 30min ligado/30min desligado
            liga = (minu % 60) < 30; break;
          case 3: // 1h ligado/1h desligado
            liga = (hora % 2) == 0; break;
          case 4: // 6h ligado/6h desligado
            liga = (hora % 12) < 6; break;
          case 5: // 12h ligado/12h desligado
            liga = (hora % 24) < 12; break;
          case 6: // Debug: 10s ligado/10s desligado
            liga = (sec % 20) < 10; break;
          default:
            liga = false;
        }
        relay_set(i, liga);
      }
      continue; // Pula o processamento padrão
    }
    bool liga = false;
    for(int j=0; j<scheduleCounts[i]; j++) {
      ScheduleEvent& ev = schedules[i][j];
      // Se evento é para todos dias (0) ou para o dia atual (ajustando indices)
      if (ev.dayOfWeek==0 || ev.dayOfWeek==((dow==0)?1:dow+1)) {
        int on = ev.h_on * 3600 + ev.m_on * 60 + ev.s_on;
        int off = ev.h_off * 3600 + ev.m_off * 60 + ev.s_off;
        if (on < off) {
          // Intervalo normal no mesmo dia
          if (agora >= on && agora < off) liga = true;
        } else if (on > off) {
          // Intervalo atravessa a meia-noite
          if (agora >= on || agora < off) liga = true;
        } else if (on == off) {
          // Se on == off, considere 24h ligado!
          liga = true;
        }
      }
    }
    // Só altera se NÃO estiver em modo manual
    if (!relayManual[i]) {
      relay_set(i, liga);
    }
  }
}


