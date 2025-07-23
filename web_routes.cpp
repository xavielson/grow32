#include "web_routes.h"
#include <WebServer.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"
#include "web_html.h"

// Use NUM_RELAYS, MAX_EVENTS, relays, relayStates, relayManual, schedules, scheduleCounts como já definidos nos outros arquivos
WebServer server(80);

// ---- Handlers ----
void handleRoot()           { server.send(200, "text/html", getPage()); }
void handleRelayData()      {
  Serial.println("[RELAYDATA] Status solicitado");
  String js = "[";
  for (int i=0; i<NUM_RELAYS; i++) {
    if (i>0) js += ",";
    js += "{";
    js += "\"name\":\"" + htmlEscape(relays[i].name) + "\",";
    js += "\"type\":\"" + relays[i].type + "\",";
    js += "\"state\":" + String(relayStates[i] ? "true":"false") + ",";
    js += "\"num_sched\":" + String(scheduleCounts[i]) + ",";
    bool isManual = relayManual[i] || (scheduleCounts[i] == 0);
    js += "\"manual\":" + String(isManual ? "true":"false");
    js += "}";
  }
  js += "]";
  server.send(200, "application/json", js);
}
void handleToggleRelay()    {
  if (server.hasArg("rele")) {
    int idx = server.arg("rele").toInt();
    Serial.print("[TOGGLE] Pedido para trocar rele ");
    Serial.println(idx);
    relay_toggle(idx);
    Serial.print("[TOGGLE] Novo estado: ");
    Serial.println(relayStates[idx] ? "ON" : "OFF");
  } else {
    Serial.println("[TOGGLE] Pedido recebido sem argumento 'rele'");
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
  server.send(200,"application/json",js);
}
void handleAddSched()       {
  if (!server.hasArg("rele")) return server.send(400,"text/plain","Faltou rele");
  int idx = server.arg("rele").toInt();
  if (idx<0 || idx>=NUM_RELAYS) return server.send(400,"text/plain","IDX inválido");
  if (scheduleCounts[idx] >= MAX_EVENTS) return server.send(400,"text/plain","Limite de eventos");
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

// -- Setup rotas --
void setupWebRoutes() {
  server.on("/", handleRoot);
  server.on("/relaydata", handleRelayData);
  server.on("/toggle", handleToggleRelay);
  server.on("/setconfig", handleSetConfig);
  server.on("/clock", handleClock);
  server.on("/getsched", handleGetSched);
  server.on("/addsched", handleAddSched);
  server.on("/delsched", handleDelSched);
  server.on("/setauto", handleSetAuto); // Nova rota!
  server.begin();
}

// -- Loop --
void handleWebServer() {
  server.handleClient();
}
