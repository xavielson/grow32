#include "config.h"
#include "web_routes.h"
#include <WebServer.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "web_html.h"

// Use NUM_RELAYS, MAX_EVENTS, relays, relayStates, relayManual, schedules, scheduleCounts como já definidos nos outros arquivos
WebServer server(80);

// ---- Handlers ----
void handleRoot()           { server.send(200, "text/html; charset=utf-8", getPage(DEBUG_MODE)); }

const char* diasSemana[8] = { "Todos", "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };

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
    bool isManual = relayManual[i] || (scheduleCounts[i] == 0);
    js += "\"manual\":" + String(isManual ? "true" : "false");

    // ---- AGENDAMENTOS ----
    js += ",\"horarios\":[";
    for (int j = 0; j < scheduleCounts[i]; j++) {
      if (j > 0) js += ",";
      ScheduleEvent& ev = schedules[i][j];
      int d = ev.dayOfWeek; // 0=Todos, 1=Dom, etc
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
    // ----------------------
    js += "}";
  }
  js += "]";
  server.send(200, "application/json", js);
}

void handleToggleRelay()    {
  if (server.hasArg("rele")) {
    int idx = server.arg("rele").toInt();
    //Serial.print("[TOGGLE] Pedido para trocar rele ");
    //Serial.println(idx);
    relay_toggle(idx);
    //Serial.print("[TOGGLE] Novo estado: ");
    //Serial.println(relayStates[idx] ? "ON" : "OFF");
  } else {
    //Serial.println("[TOGGLE] Pedido recebido sem argumento 'rele'");
  }
  server.send(200, "text/plain", "OK");
}
void handleSetConfig()      {
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
  server.send(200, "text/plain", "OK");
}
void handleClock()          {
  time_t now = ntp_now();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char buf[40];
  snprintf(buf, sizeof(buf), "Hora atual: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  server.send(200, "text/plain", buf);
}
void handleGetSched()       {
  if (!server.hasArg("rele")) return server.send(400,"text/plain","Faltou rele");
  int idx = server.arg("rele").toInt();
  if (idx<0 || idx>=NUM_RELAYS) return server.send(400,"text/plain","IDX inválido");
  String js="[";
  for (int i=0; i<scheduleCounts[idx]; i++) {
    if(i>0) js+=",";
    js+="{";
    js+="\"dia\":"+String(schedules[idx][i].dayOfWeek);
    js+=",\"h_on\":"+String(schedules[idx][i].h_on);
    js+=",\"m_on\":"+String(schedules[idx][i].m_on);
    js+=",\"s_on\":"+String(schedules[idx][i].s_on);
    js+=",\"h_off\":"+String(schedules[idx][i].h_off);
    js+=",\"m_off\":"+String(schedules[idx][i].m_off);
    js+=",\"s_off\":"+String(schedules[idx][i].s_off);
    js+="}";
  }
  js+="]";
  Serial.printf("[GETSCHED] idx=%d, eventos=%d\n", idx, scheduleCounts[idx]);
  if (scheduleCounts[idx] > 0) {
    Serial.printf("[GETSCHED] Primeiro evento: %02d:%02d:%02d ~ %02d:%02d:%02d\n",
      schedules[idx][0].h_on, schedules[idx][0].m_on, schedules[idx][0].s_on,
      schedules[idx][0].h_off, schedules[idx][0].m_off, schedules[idx][0].s_off);
  }
  server.send(200,"application/json",js);
}
void handleAddSched() {
  if (!server.hasArg("rele")) return server.send(400,"text/plain","Faltou rele");
  int idx = server.arg("rele").toInt();
  if (idx<0 || idx>=NUM_RELAYS) return server.send(400,"text/plain","IDX inválido");
  if (scheduleCounts[idx] >= MAX_EVENTS) return server.send(400,"text/plain","Limite de eventos");

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

  server.send(200,"text/plain","OK");
}

void handleDelSched()       {
  if (!server.hasArg("rele") || !server.hasArg("idx")) return server.send(400,"text/plain","Faltou arg");
  int idx = server.arg("rele").toInt();
  int eidx = server.arg("idx").toInt();
  if (idx<0 || idx>=NUM_RELAYS) return server.send(400,"text/plain","IDX inválido");
  if (eidx<0 || eidx>=scheduleCounts[idx]) return server.send(400,"text/plain","Evento inválido");
  for(int i=eidx; i<scheduleCounts[idx]-1; i++) {
    schedules[idx][i] = schedules[idx][i+1];
  }
  scheduleCounts[idx]--;
  if (scheduleCounts[idx] == 0 && relays[idx].type != "Wavemaker") {
      relayHasSchedule[idx] = false;
  }
  server.send(200,"text/plain","OK");
}

// NOVO: rota para voltar ao modo automático
void handleSetAuto() {
    if (server.hasArg("rele")) {
        int idx = server.arg("rele").toInt();
        if (idx >= 0 && idx < NUM_RELAYS) {
            relayManual[idx] = false;
        }
    }
    server.send(200, "text/plain", "OK");
}

// DEBUG: cria agendamentos de 1 segundo para debug manual (só no modo debug)
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

    // Limpa agendamentos antigos de TODOS os relés
    for (int i = 0; i < NUM_RELAYS; i++) {
        scheduleCounts[i] = 0;
    }

    time_t now = ntp_now();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    int base_h = timeinfo.tm_hour;
    int base_m = timeinfo.tm_min;
    int base_s = timeinfo.tm_sec;

    // ==== Fase 1: alternando um relé por vez (3 ciclos) ====
    int ciclos = 3;
    int k = 0;
    for (; k < (NUM_RELAYS-2) * ciclos; k++) { // Atenção: -2 para não criar eventos para os wavemakers
        int h = base_h, m = base_m, s_on = base_s + k;

        // Rollover dos segundos/minutos/horas
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;

        int s_off = s_on + 1;
        int m_off = m, h_off = h;
        if (s_off >= 60) { s_off = 0; m_off += 1; }
        if (m_off >= 60) { m_off = 0; h_off += 1; }
        if (h_off >= 24) h_off = 0;

        int rele = k % (NUM_RELAYS-2); // Só para os que não são wavemaker
        ScheduleEvent ev;
        ev.dayOfWeek = 0;
        ev.h_on = h;
        ev.m_on = m;
        ev.s_on = s_on;
        ev.h_off = h_off;
        ev.m_off = m_off;
        ev.s_off = s_off;

        if (scheduleCounts[rele] < MAX_EVENTS)
            schedules[rele][scheduleCounts[rele]++] = ev;
    }

    // ==== Fase 2: todos ligados 1s, todos desligados 1s, repete 5x ====
    for (int fase = 0; fase < 5; fase++) {
        // Todos ligados por 1s (menos wavemakers)
        int h = base_h, m = base_m, s_on = base_s + k;
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;

        int s_off = s_on + 1;
        int m_off = m, h_off = h;
        if (s_off >= 60) { s_off = 0; m_off += 1; }
        if (m_off >= 60) { m_off = 0; h_off += 1; }
        if (h_off >= 24) h_off = 0;

        for (int rele = 0; rele < (NUM_RELAYS-2); rele++) { // Só os normais
            ScheduleEvent ev;
            ev.dayOfWeek = 0;
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

        // Todos desligados por 1s (sem agendamento para o próximo segundo)
        h = base_h; m = base_m; s_on = base_s + k;
        if (s_on >= 60) { m += s_on / 60; s_on = s_on % 60; }
        if (m >= 60)    { h += m / 60;    m = m % 60; }
        if (h >= 24)    h = h % 24;
        k++;
    }

    // ==== Para os dois últimos relés, configura como Wavemaker em modo 10s/10s ====
    for (int i = NUM_RELAYS-2; i < NUM_RELAYS; i++) {
        relays[i].type = "Wavemaker";
        relays[i].wavemaker_mode = 6; // 10s Ligado/10s Desligado
        scheduleCounts[i] = 0;        // Não cria agendamentos
        relayHasSchedule[i] = true;   // Tem "agendamento" (modo especial)
    }

    server.send(200, "text/plain", "Debug concluído: fase alternada + todos ligados/desligados + wavemaker.");
}


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
    scheduleCounts[idx] = 0; // Limpa todos os eventos para este relay!
    relayHasSchedule[idx] = true;  // <--- PASSO 4: marca que agora tem agendamento (modo ativo)
    server.send(200, "text/plain", "OK");
}


// -- Setup rotas --
void setupWebRoutes() {
  server.on("/", handleRoot);
  server.on("/relaydata", handleRelayData);
  server.on("/toggle", handleToggleRelay);
  server.on("/setconfig", handleSetConfig);
  server.on("/clock", handleClock);
  server.on("/getsched", handleGetSched);
  server.on("/addsched", handleAddSched);  
  server.on("/setwavemakermode", handleSetWavemakerMode);
  server.on("/delsched", handleDelSched);
  

  // Rota de debug SEMPRE registrada para sincronizar com o frontend
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

// -- Loop --
void handleWebServer() {
  server.handleClient();
}
