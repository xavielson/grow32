#include "config.h"
#include "web_routes.h"
#include <WebServer.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "web_html.h"
#include "storage.h"
//#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

// Instância do servidor web
WebServer server(80);

// Nomes dos dias da semana
const char* diasSemana[8] = { "Todos", "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };

/**
 * Handler da página inicial HTML.
 * Endpoint: /
 */
void handleRoot() {
    server.send(200, "text/html; charset=utf-8", getPage(DEBUG_MODE));
}

/**
 * Handler: retorna dados JSON de todos os relés e seus agendamentos.
 * Endpoint: /relaydata
 * Método: GET
 */
void handleRelayData() {
    String js = "[";
    for (int i = 0; i < NUM_RELAYS; i++) {
        if (i > 0) js += ",";

        js += "{";
        js += "\"name\":\"" + htmlEscape(relays[i].name) + "\",";
        js += "\"type\":\"" + relays[i].type + "\",";
        js += "\"state\":" + String(relayStates[i] ? "true" : "false") + ",";
        js += "\"num_sched\":" + String(scheduleCounts[i]) + ",";
        js += "\"has_schedule\":" + String(relayHasSchedule[i] ? "true" : "false") + ",";
        js += "\"wavemaker_mode\":" + String(relays[i].wavemaker_mode) + ",";
        bool isWavemaker = relays[i].type == "Wavemaker";
        bool isManual = relayManual[i] || (!isWavemaker && scheduleCounts[i] == 0);
        js += "\"manual\":" + String(isManual ? "true" : "false");

        // Serialização dos horários/agendamentos
        js += ",\"horarios\":[";
        for (int j = 0; j < scheduleCounts[i]; j++) {
            if (j > 0) js += ",";
            ScheduleEvent& ev = schedules[i][j];
            int d = ev.dayOfWeek;
            String dia = diasSemana[d >= 0 && d <= 7 ? d : 0];

            char bufOn[9], bufOff[9];
            sprintf(bufOn, "%02d:%02d:%02d", ev.h_on, ev.m_on, ev.s_on);
            sprintf(bufOff, "%02d:%02d:%02d", ev.h_off, ev.m_off, ev.s_off);

            // Liga
            js += "{";
            js += "\"acao\":\"liga\",";
            js += "\"dia\":\"" + dia + "\",";
            js += "\"hora\":\"" + String(bufOn) + "\"";
            js += "},";

            // Desliga
            js += "{";
            js += "\"acao\":\"desliga\",";
            js += "\"dia\":\"" + dia + "\",";
            js += "\"hora\":\"" + String(bufOff) + "\"";
            js += "}";
        }
        js += "]";
        js += "}";
    }
    js += "]";
    server.send(200, "application/json", js);
}

/**
 * Handler: alterna (toggle) o estado de um relé.
 * Endpoint: /toggle?rele=<idx>
 * Método: GET
 */
void handleToggleRelay() {
    if (server.hasArg("rele")) {
        int idx = server.arg("rele").toInt();
        relay_toggle(idx);
    }
    server.send(200, "text/plain", "OK");
}

/**
 * Handler: altera nome/tipo do relé.
 * Endpoint: /setconfig?rele=<idx>&name=...&type=...
 * Método: GET
 */
void handleSetConfig() {
    if (server.hasArg("rele") && server.hasArg("name") && server.hasArg("type")) {
        int idx = server.arg("rele").toInt();
        String name = server.arg("name");
        String type = server.arg("type");
        if (idx >= 0 && idx < NUM_RELAYS) {
            relays[idx].name = name.substring(0, 24);
            if (type == "Led" || type == "Rega" || type == "Wavemaker" || type == "Runoff")
                relays[idx].type = type;
        }
    }
    storage_save_all(relays, NUM_RELAYS);
    server.send(200, "text/plain", "OK");
}

/**
 * Handler: retorna hora atual via NTP.
 * Endpoint: /clock
 * Método: GET
 */
void handleClock() {
    time_t now = ntp_now();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    const char* dias[] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
    char buf[40];
    snprintf(
        buf, sizeof(buf),
        "%s %02d:%02d:%02d",           // sem "Hora atual:", só dia + hora
        dias[timeinfo.tm_wday],
        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec
    );
    server.send(200, "text/plain", buf);
}

/**
 * Handler: retorna todos agendamentos de um relé.
 * Endpoint: /getsched?rele=<idx>
 * Método: GET
 */
void handleGetSched() {
    if (!server.hasArg("rele")) return server.send(400, "text/plain", "Faltou rele");
    int idx = server.arg("rele").toInt();
    if (idx < 0 || idx >= NUM_RELAYS) return server.send(400, "text/plain", "IDX inválido");
    String js = "[";
    for (int i = 0; i < scheduleCounts[idx]; i++) {
        if (i > 0) js += ",";
        js += "{";
        js += "\"dia\":" + String(schedules[idx][i].dayOfWeek);
        js += ",\"h_on\":" + String(schedules[idx][i].h_on);
        js += ",\"m_on\":" + String(schedules[idx][i].m_on);
        js += ",\"s_on\":" + String(schedules[idx][i].s_on);
        js += ",\"h_off\":" + String(schedules[idx][i].h_off);
        js += ",\"m_off\":" + String(schedules[idx][i].m_off);
        js += ",\"s_off\":" + String(schedules[idx][i].s_off);
        js += "}";
    }
    js += "]";
    Serial.printf("[GETSCHED] idx=%d, eventos=%d\n", idx, scheduleCounts[idx]);
    if (scheduleCounts[idx] > 0) {
        Serial.printf("[GETSCHED] Primeiro evento: %02d:%02d:%02d ~ %02d:%02d:%02d\n",
            schedules[idx][0].h_on, schedules[idx][0].m_on, schedules[idx][0].s_on,
            schedules[idx][0].h_off, schedules[idx][0].m_off, schedules[idx][0].s_off);
    }
    server.send(200, "application/json", js);
}

/**
 * Handler: adiciona agendamento para um relé.
 * Endpoint: /addsched
 * Método: GET
 * Params: rele, dia, h_on, m_on, s_on, h_off, m_off, s_off
 */
void handleAddSched() {
    if (!server.hasArg("rele")) return server.send(400, "text/plain", "Faltou rele");
    int idx = server.arg("rele").toInt();
    if (idx < 0 || idx >= NUM_RELAYS) return server.send(400, "text/plain", "IDX inválido");
    if (scheduleCounts[idx] >= MAX_EVENTS) return server.send(400, "text/plain", "Limite de eventos");

    // Se este é o primeiro agendamento, força modo automático
    if (scheduleCounts[idx] == 0) {
        relayManual[idx] = false;
    }

    ScheduleEvent ev;
    String type = relays[idx].type;
    ev.dayOfWeek = (type == "Led") ? 0 : server.arg("dia").toInt();
    ev.h_on = server.arg("h_on").toInt();
    ev.m_on = server.arg("m_on").toInt();
    ev.s_on = server.arg("s_on").toInt();
    ev.h_off = server.arg("h_off").toInt();
    ev.m_off = server.arg("m_off").toInt();
    ev.s_off = server.arg("s_off").toInt();
    schedules[idx][scheduleCounts[idx]++] = ev;

    relayHasSchedule[idx] = true;

    storage_save_all(relays, NUM_RELAYS);
    server.send(200, "text/plain", "OK");
}

/**
 * Handler: remove agendamento de um relé.
 * Endpoint: /delsched?rele=<idx>&idx=<eidx>
 * Método: GET
 */
void handleDelSched() {
    if (!server.hasArg("rele") || !server.hasArg("idx")) return server.send(400, "text/plain", "Faltou arg");
    int idx = server.arg("rele").toInt();
    int eidx = server.arg("idx").toInt();
    if (idx < 0 || idx >= NUM_RELAYS) return server.send(400, "text/plain", "IDX inválido");
    if (eidx < 0 || eidx >= scheduleCounts[idx]) return server.send(400, "text/plain", "Evento inválido");
    for (int i = eidx; i < scheduleCounts[idx] - 1; i++) {
        schedules[idx][i] = schedules[idx][i + 1];
    }
    scheduleCounts[idx]--;
    if (scheduleCounts[idx] == 0 && relays[idx].type != "Wavemaker") {
        relayHasSchedule[idx] = false;
    }
    storage_save_all(relays, NUM_RELAYS);
    server.send(200, "text/plain", "OK");
}

/**
 * Handler: volta relé para modo automático.
 * Endpoint: /setauto?rele=<idx>
 * Método: GET
 */
void handleSetAuto() {
    if (server.hasArg("rele")) {
        int idx = server.arg("rele").toInt();
        if (idx >= 0 && idx < NUM_RELAYS) {
            relayManual[idx] = false;
        }
    }
    server.send(200, "text/plain", "OK");
}

/**
 * Handler: debug - cria agendamentos de 1s para testes automáticos.
 * Endpoint: /debugsched
 * Método: GET (só se DEBUG_MODE)
 */
void handleDebugSched() {
    const char* nomes[] = {"Led Teste", "Bomba Teste", "Runoff Teste", "Led 2 Teste",
                          "Saída 5 Teste", "Saída 6 Teste", "Wavemaker 1 Teste", "Wavemaker 2 Teste"};
    const char* tipos[] = {"Led", "Rega", "Runoff", "Led",
                          "Rega", "Runoff", "Wavemaker", "Wavemaker"};

    for (int i = 0; i < NUM_RELAYS; i++) {
        relays[i].name = nomes[i];
        relays[i].type = tipos[i];
        relayHasSchedule[i] = true;
        relays[i].wavemaker_mode = -1; // Zera por padrão
    }

    // Limpa agendamentos antigos
    for (int i = 0; i < NUM_RELAYS; i++) {
        scheduleCounts[i] = 0;
    }

    time_t now = ntp_now();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    int base_h = timeinfo.tm_hour;
    int base_m = timeinfo.tm_min;
    int base_s = timeinfo.tm_sec;

    // Fase 1: alternando um relé por vez (3 ciclos)
    int ciclos = 3;
    int k = 0;
    for (; k < (NUM_RELAYS - 2) * ciclos; k++) { // -2 para não criar eventos para os wavemakers
        int h = base_h, m = base_m, s_on = base_s + k;
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;

        int s_off = s_on + 1;
        int m_off = m, h_off = h;
        if (s_off >= 60) { s_off = 0; m_off += 1; }
        if (m_off >= 60) { m_off = 0; h_off += 1; }
        if (h_off >= 24) h_off = 0;

        int rele = k % (NUM_RELAYS - 2);
        ScheduleEvent ev;
        if (relays[rele].type == "Rega") {
            ev.dayOfWeek = timeinfo.tm_wday + 1;
        } else {
            ev.dayOfWeek = 0;
        }
        ev.h_on = h;
        ev.m_on = m;
        ev.s_on = s_on;
        ev.h_off = h_off;
        ev.m_off = m_off;
        ev.s_off = s_off;

        if (scheduleCounts[rele] < MAX_EVENTS)
            schedules[rele][scheduleCounts[rele]++] = ev;
    }

    // Fase 2: todos ligados/desligados por 1s, repete 5x
    for (int fase = 0; fase < 5; fase++) {
        int h = base_h, m = base_m, s_on = base_s + k;
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;

        int s_off = s_on + 1;
        int m_off = m, h_off = h;
        if (s_off >= 60) { s_off = 0; m_off += 1; }
        if (m_off >= 60) { m_off = 0; h_off += 1; }
        if (h_off >= 24) h_off = 0;

        for (int rele = 0; rele < (NUM_RELAYS - 2); rele++) {
            ScheduleEvent ev;
            if (relays[rele].type == "Rega") {
                ev.dayOfWeek = timeinfo.tm_wday + 1;
            } else {
                ev.dayOfWeek = 0;
            }
            ev.h_on = h;
            ev.m_on = m;
            ev.s_on = s_on;
            ev.h_off = h_off;
            ev.m_off = m_off;
            ev.s_off = s_off;
            if (scheduleCounts[rele] < MAX_EVENTS)
                schedules[rele][scheduleCounts[rele]++] = ev;
        }
        k++;

        // Todos desligados por 1s (não cria agendamento novo)
        h = base_h; m = base_m; s_on = base_s + k;
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;
        k++;
    }

    // Para os dois últimos relés, configura como Wavemaker em modo 10s/10s
    for (int i = NUM_RELAYS - 2; i < NUM_RELAYS; i++) {
        relays[i].type = "Wavemaker";
        relays[i].wavemaker_mode = 6;
        scheduleCounts[i] = 0;
        relayHasSchedule[i] = true;
    }
    storage_save_all(relays, NUM_RELAYS);

    server.send(200, "text/plain", "Debug concluído: fase alternada + todos ligados/desligados + wavemaker.");
}

/**
 * Handler: define modo do wavemaker.
 * Endpoint: /setwavemakermode?rele=<idx>&mode=<modo>
 * Método: GET
 */
void handleSetWavemakerMode() {
    if (!server.hasArg("rele") || !server.hasArg("mode")) {
        server.send(400, "text/plain", "Parâmetros ausentes");
        return;
    }
    int idx = server.arg("rele").toInt();
    int modo = server.arg("mode").toInt();
    if (idx < 0 || idx >= NUM_RELAYS) {
        server.send(400, "text/plain", "IDX inválido");
        return;
    }
    relays[idx].wavemaker_mode = modo;
    scheduleCounts[idx] = 0; // Limpa eventos
    relayHasSchedule[idx] = true;
    storage_save_all(relays, NUM_RELAYS);
    server.send(200, "text/plain", "OK");
}

// /**
//  * Handler: exporta relay.json schedules.json em um arquivo
//  * Endpoint: "/export_all"
//  * Método: GET
//  */
// void handleExportAll() {
//     StaticJsonDocument<32768> doc; // Ajuste o tamanho conforme necessário

//     // Lê relays.json
//     File fRelays = SPIFFS.open("/relays.json", "r");
//     if (fRelays) {
//         DeserializationError err = deserializeJson(doc["relays"], fRelays);
//         fRelays.close();
//         if (err) {
//             server.send(500, "text/plain", "Erro ao ler relays.json");
//             return;
//         }
//     } else {
//         server.send(500, "text/plain", "relays.json não encontrado");
//         return;
//     }

//     // Lê schedules.json
//     File fSchedules = SPIFFS.open("/schedules.json", "r");
//     if (fSchedules) {
//         DeserializationError err = deserializeJson(doc["schedules"], fSchedules);
//         fSchedules.close();
//         if (err) {
//             server.send(500, "text/plain", "Erro ao ler schedules.json");
//             return;
//         }
//     } else {
//         server.send(500, "text/plain", "schedules.json não encontrado");
//         return;
//     }

//     String response;
//     serializeJson(doc, response);
//     server.sendHeader("Content-Disposition", "attachment; filename=\"backup_grow32.json\"");
//     server.send(200, "application/json", response);

// }



// void handleImportAll() {
//     if (!server.hasArg("plain")) {
//         server.send(400, "text/plain", "Dados JSON ausentes");
//         return;
//     }
//     String data = server.arg("plain");
//     if (data.length() < 10) {
//         server.send(400, "text/plain", "JSON muito curto");
//         return;
//     }
//     // Parse o JSON recebido
//     StaticJsonDocument<32768> doc; // ajuste se necessário!
//     DeserializationError err = deserializeJson(doc, data);
//     if (err) {
//         server.send(400, "text/plain", "JSON malformado");
//         return;
//     }
//     // Pega cada parte
//     if (!doc.containsKey("relays") || !doc.containsKey("schedules")) {
//         server.send(400, "text/plain", "Faltam campos 'relays' ou 'schedules'");
//         return;
//     }

//     // Salva relays
//     {
//         File f = SPIFFS.open("/relays.json", "w");
//         if (!f) {
//             server.send(500, "text/plain", "Erro ao abrir relays.json para escrita");
//             return;
//         }
//         StaticJsonDocument<32768> relayDoc;
//         relayDoc["relays"] = doc["relays"];
//         serializeJson(relayDoc, f);
//         f.close();
//     }
//     // Salva schedules
//     {
//         File f = SPIFFS.open("/schedules.json", "w");
//         if (!f) {
//             server.send(500, "text/plain", "Erro ao abrir schedules.json para escrita");
//             return;
//         }
//         StaticJsonDocument<32768> schedDoc;
//         schedDoc["schedules"] = doc["schedules"];
//         serializeJson(schedDoc, f);
//         f.close();
//     }

//     // Carrega para RAM
//     bool okRelays = storage_load_relays(relays, NUM_RELAYS);
//     bool okSchedules = storage_load_schedules();

//     if (!okRelays || !okSchedules) {
//         server.send(500, "text/plain", "Importou arquivos, mas erro ao carregar na RAM");
//         return;
//     }

//     updateRelayHasSchedule();
//     server.send(200, "text/plain", "Importação de relés e agendamentos concluída!");
// }

void handleExportAll() {
    File f = SPIFFS.open("/grow32.json", "r");
    if (!f) {
        server.send(500, "text/plain", "Erro ao abrir backup.");
        return;
    }
    server.sendHeader("Content-Disposition", "attachment; filename=backup_grow32.json");
    server.streamFile(f, "application/json");
    f.close();
}

void handleImportAll() {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Dados JSON ausentes");
        return;
    }
    String data = server.arg("plain");
    if (data.length() < 10) {
        server.send(400, "text/plain", "JSON muito curto");
        return;
    }

    // (Opcional: validar JSON antes de salvar)
    File f = SPIFFS.open("/grow32.json", "w");
    if (!f) {
        server.send(500, "text/plain", "Erro ao salvar backup.");
        return;
    }
    f.print(data);
    f.close();

    // Carrega imediatamente na RAM:
    storage_load_all(relays, NUM_RELAYS);

    updateRelayHasSchedule();
    server.send(200, "text/plain", "Backup restaurado com sucesso!");
}

/**
 * Função de setup das rotas HTTP.
 */
void setupWebRoutes() {
    server.on("/", handleRoot);
    server.on("/relaydata", handleRelayData);
    server.on("/toggle", handleToggleRelay);
    server.on("/setconfig", handleSetConfig);
    server.on("/clock", handleClock);
    server.on("/getsched", handleGetSched);
    server.on("/addsched", handleAddSched);
    server.on("/setwavemakermode", handleSetWavemakerMode);
    server.on("/export_all", HTTP_GET, handleExportAll);
    server.on("/import_all", HTTP_POST, handleImportAll); 
    server.on("/delsched", handleDelSched);

    // Rota de debug (1/0) — sempre registrada
    server.on("/debug", []() {
    #if DEBUG_MODE
        server.send(200, "text/plain", "1");
    #else
        server.send(200, "text/plain", "0");
    #endif
    });

    #if DEBUG_MODE
    server.on("/debugsched", handleDebugSched);
    #endif

    server.on("/setauto", handleSetAuto);

    server.begin();
}

/**
 * Mantém o servidor HTTP ativo no loop principal.
 */
void handleWebServer() {
    server.handleClient();
}
