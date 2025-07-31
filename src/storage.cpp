#include "storage.h"
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "scheduler.h"

// Ajuste o tamanho do JSON se necessário para mais campos
#define STORAGE_JSON_SIZE 24596

bool storage_init() {
    return SPIFFS.begin(true);
}

// bool storage_save_relays(RelayConfig relays[], size_t num_relays) {
//     StaticJsonDocument<STORAGE_JSON_SIZE> doc;
//     JsonArray arr = doc.createNestedArray("relays");
//     for (size_t i = 0; i < num_relays; i++) {
//         JsonObject o = arr.createNestedObject();
//         o["name"] = relays[i].name;
//         o["type"] = relays[i].type;
//         o["wavemaker_mode"] = relays[i].wavemaker_mode;
//     }
//     File f = SPIFFS.open("/relays.json", "w");
//     if (!f) return false;
//     serializeJson(doc, f);
//     f.close();
//     return true;
// }

// bool storage_load_relays(RelayConfig relays[], size_t num_relays) {
//     File f = SPIFFS.open("/relays.json", "r");
//     if (!f) return false;
//     StaticJsonDocument<STORAGE_JSON_SIZE> doc;
//     DeserializationError err = deserializeJson(doc, f);
//     if (err) {
//         f.close();
//         return false;
//     }
//     JsonArray arr = doc["relays"];
//     for (size_t i = 0; i < arr.size() && i < num_relays; i++) {
//         JsonObject o = arr[i];
//         relays[i].name = String((const char*)o["name"]);
//         relays[i].type = String((const char*)o["type"]);
//         relays[i].wavemaker_mode = o["wavemaker_mode"] | -1; // fallback para -1 caso não exista
//         Serial.print(relays[i].wavemaker_mode);
//     }
//     f.close();
//     return true;
// }

// bool storage_save_schedules() {
//     StaticJsonDocument<16384> doc;  // Aumente se necessário!
//     JsonArray arr_relays = doc.createNestedArray("relays");
//     for (int i = 0; i < NUM_RELAYS; i++) {
//         JsonArray arr_sched = arr_relays.createNestedArray();
//         for (int j = 0; j < scheduleCounts[i]; j++) {
//             JsonObject so = arr_sched.createNestedObject();
//             so["dayOfWeek"] = schedules[i][j].dayOfWeek;
//             so["h_on"] = schedules[i][j].h_on;
//             so["m_on"] = schedules[i][j].m_on;
//             so["s_on"] = schedules[i][j].s_on;
//             so["h_off"] = schedules[i][j].h_off;
//             so["m_off"] = schedules[i][j].m_off;
//             so["s_off"] = schedules[i][j].s_off;
//         }
//     }
//     File f = SPIFFS.open("/schedules.json", "w");
//     if (!f) return false;
//     serializeJson(doc, f);
//     f.close();
//     return true;
// }

// bool storage_load_schedules() {
//     File f = SPIFFS.open("/schedules.json", "r");
//     if (!f) return false;
//     StaticJsonDocument<16384> doc;
//     DeserializationError err = deserializeJson(doc, f);
//     if (err) {
//         f.close();
//         return false;
//     }
//     JsonArray arr_relays = doc["relays"];
//     for (int i = 0; i < arr_relays.size() && i < NUM_RELAYS; i++) {
//         JsonArray arr_sched = arr_relays[i];
//         scheduleCounts[i] = 0;
//         for (JsonObject so : arr_sched) {
//             if (scheduleCounts[i] < MAX_EVENTS) {
//                 ScheduleEvent& s = schedules[i][scheduleCounts[i]++];
//                 s.dayOfWeek = so["dayOfWeek"] | 0;
//                 s.h_on = so["h_on"] | 0;
//                 s.m_on = so["m_on"] | 0;
//                 s.s_on = so["s_on"] | 0;
//                 s.h_off = so["h_off"] | 0;
//                 s.m_off = so["m_off"] | 0;
//                 s.s_off = so["s_off"] | 0;
//             }
//         }
//     }
//     f.close();
//     return true;
// }

bool storage_save_all(RelayConfig relays[], size_t num_relays) {
    StaticJsonDocument<STORAGE_JSON_SIZE> doc;

    // Salva relays
    JsonArray arrRelays = doc.createNestedArray("relays");
    for (size_t i = 0; i < num_relays; i++) {
        JsonObject o = arrRelays.createNestedObject();
        o["name"] = relays[i].name;
        o["type"] = relays[i].type;
        o["wavemaker_mode"] = relays[i].wavemaker_mode;
    }

    // Salva schedules
    JsonArray arrSchedules = doc.createNestedArray("schedules");
    for (int i = 0; i < NUM_RELAYS; i++) {
        JsonArray arrSched = arrSchedules.createNestedArray();
        for (int j = 0; j < scheduleCounts[i]; j++) {
            JsonObject so = arrSched.createNestedObject();
            so["dayOfWeek"] = schedules[i][j].dayOfWeek;
            so["h_on"] = schedules[i][j].h_on;
            so["m_on"] = schedules[i][j].m_on;
            so["s_on"] = schedules[i][j].s_on;
            so["h_off"] = schedules[i][j].h_off;
            so["m_off"] = schedules[i][j].m_off;
            so["s_off"] = schedules[i][j].s_off;
        }
    }

    File f = SPIFFS.open("/grow32.json", "w");
    if (!f) return false;
    serializeJson(doc, f);
    f.close();
    return true;
}

bool storage_load_all(RelayConfig relays[], size_t num_relays) {
    File f = SPIFFS.open("/grow32.json", "r");
    if (!f) return false;
    StaticJsonDocument<STORAGE_JSON_SIZE> doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) {
        Serial.println("Erro ao carregar grow32.json");
        return false;
    }

    // --- Carrega relays ---
    JsonArray arrRelays = doc["relays"];
    for (size_t i = 0; i < arrRelays.size() && i < num_relays; i++) {
        JsonObject o = arrRelays[i];
        relays[i].name = String((const char*)o["name"]);
        relays[i].type = String((const char*)o["type"]);
        relays[i].wavemaker_mode = o["wavemaker_mode"] | -1;
    }

    // --- Carrega schedules ---
    JsonArray arrSchedules = doc["schedules"];
    for (int i = 0; i < arrSchedules.size() && i < NUM_RELAYS; i++) {
        JsonArray arrSched = arrSchedules[i];
        scheduleCounts[i] = 0;
        for (JsonObject so : arrSched) {
            if (scheduleCounts[i] < MAX_EVENTS) {
                ScheduleEvent& s = schedules[i][scheduleCounts[i]++];
                s.dayOfWeek = so["dayOfWeek"] | 0;
                s.h_on = so["h_on"] | 0;
                s.m_on = so["m_on"] | 0;
                s.s_on = so["s_on"] | 0;
                s.h_off = so["h_off"] | 0;
                s.m_off = so["m_off"] | 0;
                s.s_off = so["s_off"] | 0;
            }
        }
    }

    return true;
}
