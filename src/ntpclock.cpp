#include "ntpclock.h"
#include <time.h>
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

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

void ntp_or_rtc_setup() {
    configTime(LOCAL_TZ_OFFSET_SECONDS, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Aguardando NTP...");
    time_t now = time(nullptr);
    int tentativas = 0;
    const int maxTentativas = 12; // Ex: 12x500ms = 6s

    while (now < NTP_SYNC_THRESHOLD && tentativas < maxTentativas) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
        tentativas++;
    }
    if (now >= NTP_SYNC_THRESHOLD) {
        Serial.println("\nNTP OK");
        // Atualiza o RTC com o horário de NTP:
        rtc.adjust(DateTime(now));
    } else {
        Serial.println("\nNTP falhou, lendo RTC...");
        DateTime rtcNow = rtc.now();
        now = rtcNow.unixtime();
        struct timeval tv = { .tv_sec = now, .tv_usec = 0 };
        settimeofday(&tv, NULL);
        Serial.printf("Horário ajustado pelo RTC: %02d/%02d/%04d %02d:%02d:%02d\n",
            rtcNow.day(), rtcNow.month(), rtcNow.year(), rtcNow.hour(), rtcNow.minute(), rtcNow.second());
    }
}


/**
 * Retorna horário local atual como epoch time.
 */
time_t ntp_now() {
    return time(nullptr);
}
