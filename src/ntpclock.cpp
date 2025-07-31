#include "ntpclock.h"
#include <time.h>
#include <Arduino.h>

/**
 * Fuso horário local (em segundos; -3*3600 para Brasília, ajuste conforme necessário).
 */
#define LOCAL_TZ_OFFSET_SECONDS (-3 * 3600)

/**
 * Threshold mínimo para considerar o relógio sincronizado (1970+4h).
 */
#define NTP_SYNC_THRESHOLD (8 * 3600 * 2)

/**
 * Inicializa NTP e aguarda sincronização.
 * Bloqueia até que o relógio esteja ajustado.
 */
void ntp_setup() {
    configTime(LOCAL_TZ_OFFSET_SECONDS, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Aguardando NTP...");
    time_t now = time(nullptr);
    while (now < NTP_SYNC_THRESHOLD) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\nNTP OK");
}

/**
 * Retorna horário local atual como epoch time.
 */
time_t ntp_now() {
    return time(nullptr);
}
