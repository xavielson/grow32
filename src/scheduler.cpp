#include "scheduler.h"
#include "ntpclock.h"
#include "relay.h"

// Matriz de eventos por relé
ScheduleEvent schedules[NUM_RELAYS][MAX_EVENTS];
int scheduleCounts[NUM_RELAYS] = {0};

/**
 * Inicialização do scheduler (vazio por padrão).
 */
void scheduler_setup() {}

/**
 * Executa o loop do scheduler a cada 500ms.
 */
void scheduler_loop() {
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 500) {
        processSchedules();
        lastCheck = millis();
    }
}

/**
 * Retorna o tempo atual em segundos desde 00:00.
 */
static int getCurrentTimeInSeconds() {
    time_t now = ntp_now();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
}

/**
 * Retorna o dia da semana atual no formato 1=Dom ... 7=Sab.
 */
static int getCurrentDayOfWeek() {
    time_t now = ntp_now();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    int dow = timeinfo.tm_wday; // 0=domingo
    return (dow == 0) ? 1 : dow + 1;
}

/**
 * Checa se o horário atual está dentro do intervalo programado.
 * Suporta intervalos que atravessam a meia-noite.
 */
static bool isWithinEventInterval(const ScheduleEvent& ev, int currentTimeSec) {
    int on = ev.h_on * 3600 + ev.m_on * 60 + ev.s_on;
    int off = ev.h_off * 3600 + ev.m_off * 60 + ev.s_off;

    if (on < off) {
        return (currentTimeSec >= on && currentTimeSec < off);
    } else if (on > off) {
        // Intervalo atravessa a meia-noite
        return (currentTimeSec >= on || currentTimeSec < off);
    } else {
        // on == off → ligado 24h
        return true;
    }
}

/**
 * Processa agendamento do relé tipo Wavemaker, retornando true se tratou.
 */
static bool processWavemaker(int relayIdx) {
    if (relays[relayIdx].type == "Wavemaker" && relays[relayIdx].wavemaker_mode >= 0 && !relayManual[relayIdx]) {
        time_t now = ntp_now();
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        int hora = timeinfo.tm_hour;
        int minu = timeinfo.tm_min;
        int sec = timeinfo.tm_sec;

        bool shouldTurnOn = false;
        switch (relays[relayIdx].wavemaker_mode) {
            case 0: shouldTurnOn = true; break;                          // Sempre ligado
            case 1: shouldTurnOn = (minu % 30) < 15; break;              // 15min ligado/15min desligado
            case 2: shouldTurnOn = (minu % 60) < 30; break;              // 30min ligado/30min desligado
            case 3: shouldTurnOn = (hora % 2) == 0; break;               // 1h ligado/1h desligado
            case 4: shouldTurnOn = (hora % 12) < 6; break;               // 6h ligado/6h desligado
            case 5: shouldTurnOn = (hora % 24) < 12; break;              // 12h ligado/12h desligado
            case 6: shouldTurnOn = (sec % 20) < 10; break;               // Debug: 10s ligado/10s desligado
            default: shouldTurnOn = false;
        }
        relay_set(relayIdx, shouldTurnOn);
        return true;
    }
    return false;
}

/**
 * Processa todos os agendamentos e atualiza os relés.
 */
void processSchedules() {
    int currentDay = getCurrentDayOfWeek();
    int currentTimeSec = getCurrentTimeInSeconds();

    for (int i = 0; i < NUM_RELAYS; i++) {
        // Se for Wavemaker, já trata e pula para próximo
        if (processWavemaker(i)) continue;

        bool shouldTurnOn = false;
        for (int j = 0; j < scheduleCounts[i]; j++) {
            ScheduleEvent& ev = schedules[i][j];

            // Se evento é para todos dias (0) ou para o dia atual (ajustando indices)
            if (ev.dayOfWeek == ALL_DAYS || ev.dayOfWeek == currentDay) {
                if (isWithinEventInterval(ev, currentTimeSec)) {
                    shouldTurnOn = true;
                    break; // Ativa se encontrar qualquer evento válido
                }
            }
        }
        // Só altera se NÃO estiver em modo manual
        if (!relayManual[i]) {
            relay_set(i, shouldTurnOn);
        }
    }
}

/**
 * Adiciona um evento de agendamento ao relé.
 * Retorna true se o evento foi adicionado com sucesso.
 */
bool addScheduleEvent(int relayIdx, const ScheduleEvent& event) {
    if (relayIdx < 0 || relayIdx >= NUM_RELAYS) return false;
    if (scheduleCounts[relayIdx] >= MAX_EVENTS) return false; // Protege overflow
    schedules[relayIdx][scheduleCounts[relayIdx]++] = event;
    return true;
}
