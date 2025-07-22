#include "webinterface.h"
#include <WiFi.h>
#include <WebServer.h>
#include "relay.h"
#include "scheduler.h"
#include "ntpclock.h"

const char* ssid = "MLPX";
const char* password = "plx54321";
WebServer server(80);

// --- Funções auxiliares ---
String htmlEscape(const String& str) {
  String out = "";
  for (size_t i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if      (c == '&')  out += "&amp;";
    else if (c == '<')  out += "&lt;";
    else if (c == '>')  out += "&gt;";
    else if (c == '"')  out += "&quot;";
    else if (c == '\'') out += "&#39;";
    else                out += c;
  }
  return out;
}

String getPage() {
  String page = R"rawliteral(
    <html><head>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <style>
        body{font-family:sans-serif;max-width:440px;margin:auto;}
        button{width:80px;height:38px;font-size:15px;margin:7px;border-radius:8px;}
        .on{background:lime;}
        .off{background:#ccc;}
        .edit{background:#90caf9;}
        .label{font-weight:bold;}
        .card{border:1px solid #ccc;border-radius:13px;margin:8px;padding:10px 8px 10px 8px;}
        #modal-bg,#sched-bg{position:fixed;left:0;top:0;width:100vw;height:100vh;background:#0009;display:none;z-index:3;}
        #modal,#sched{position:fixed;left:50%;top:50%;transform:translate(-50%,-50%);
               background:white;padding:18px 18px 8px 18px;border-radius:18px;display:none;z-index:4;min-width:290px;}
        #modal input,#modal select,#sched input,#sched select{width:62px;font-size:16px;}
        #modal label{display:inline-block;width:45px;text-align:right;}
        #modal button,#sched button{width:auto;}
        .disabled{opacity:0.55;pointer-events:none;}
        #sched th{font-size:14px;}
        #sched td{font-size:15px;}
        .horariosBox {display:flex;flex-direction:column;gap:8px;}
        .horarioLinha {display:flex;flex-direction:row;align-items:center;gap:6px;}
        input[type=number]{padding:4px 2px;}
      </style>
    </head><body>
      <h2>Controle de Relés ESP32</h2>
      <div id="clock" style="font-size:16px;margin-bottom:10px"></div>
      <div id="relays"></div>
      <div id="modal-bg"></div>
      <div id="sched-bg"></div>
      <div id="modal">
        <form onsubmit="saveConfig(event)">
          <input type="hidden" id="relayIdx">
          <div><label>Nome:</label>
            <input id="name" maxlength="24"></div>
          <div style="margin-top:7px"><label>Tipo:</label>
            <select id="type" onchange="atualizaSchedFormTipo()">
              <option value="Led">Led</option>
              <option value="Rega">Rega</option>
              <option value="Wavemaker">Wavemaker</option>
              <option value="Runoff">Runoff</option>
            </select>
          </div>
          <div style="margin-top:14px;text-align:right;">
            <button type="button" onclick="closeModal()">Cancelar</button>
            <button type="submit">Salvar</button>
          </div>
        </form>
      </div>
      <div id="sched">
        <div style="font-size:17px;font-weight:bold;margin-bottom:8px">Agendamentos: <span id="sched-nome"></span></div>
        <table id="sched-table" border=0></table>
        <div style="margin-top:6px">
          <form onsubmit="addEvent(event)">
            <div id="sched-dia-div" style="margin-bottom:5px">
              <select id="sdia">
                <option value="0">Todos</option>
                <option value="1">Dom</option>
                <option value="2">Seg</option>
                <option value="3">Ter</option>
                <option value="4">Qua</option>
                <option value="5">Qui</option>
                <option value="6">Sex</option>
                <option value="7">Sab</option>
              </select>
            </div>
            <div class="horariosBox">
              <div class="horarioLinha">
                <span>Hora Ligar:</span>
                <input type="number" min="0" max="23" id="sh_on" value="0">:
                <input type="number" min="0" max="59" id="sm_on" value="0">:
                <input type="number" min="0" max="59" id="ss_on" value="0">
              </div>
              <div class="horarioLinha">
                <span>Hora Desligar:</span>
                <input type="number" min="0" max="23" id="sh_off" value="0">:
                <input type="number" min="0" max="59" id="sm_off" value="0">:
                <input type="number" min="0" max="59" id="ss_off" value="0">
              </div>
            </div>
            <div style="margin-top:10px">
              <button type="submit">Adicionar</button>
            </div>
          </form>
        </div>
        <div style="margin-top:10px;text-align:right">
          <button onclick="closeSched()">Fechar</button>
        </div>
      </div>
      <script>
        let relayData = [];
        let relayTypes = [];
        function fetchRelays() {
          fetch('/relaydata').then(r=>r.json()).then(js=>{
            relayData = js;
            relayTypes = js.map(r => r.type);
            let html = '';
            for (let i=0; i<js.length; i++) {
              html += "<div class='card'>";
              html += "<div class='label'>" + escapeHTML(js[i].name) + "</div>";
              html += "<div style='color:#444;font-size:13px'>" + js[i].type + "</div>";
              html += "<button class='"+(js[i].state?'on':'off')+(i>=3?' disabled':'')+"' onclick='toggleRelay("+i+")'>"+(js[i].state?'ON':'OFF')+"</button> ";
              html += "<button class='edit' onclick='editRelay("+i+")'>Editar</button>";
              html += "<button onclick='openSched("+i+")'>Agendar</button>";
              html += "</div>";
            }
            document.getElementById('relays').innerHTML = html;
          });
        }
        function fetchClock() {
          fetch('/clock').then(r=>r.text()).then(txt=>{
            document.getElementById('clock').innerText = txt;
          });
        }
        setInterval(fetchClock, 1000);
        function toggleRelay(idx) {
          if(idx<3)
            fetch('/toggle?rele='+idx).then(r=>fetchRelays());
        }
        function editRelay(idx) {
          document.getElementById('relayIdx').value = idx;
          document.getElementById('name').value = relayData[idx].name;
          document.getElementById('type').value = relayData[idx].type;
          document.getElementById('modal-bg').style.display = 'block';
          document.getElementById('modal').style.display = 'block';
        }
        function closeModal() {
          document.getElementById('modal-bg').style.display = 'none';
          document.getElementById('modal').style.display = 'none';
        }
        function saveConfig(ev) {
          ev.preventDefault();
          let idx = document.getElementById('relayIdx').value;
          let name = document.getElementById('name').value;
          let type = document.getElementById('type').value;
          fetch('/setconfig?rele='+idx+'&name='+encodeURIComponent(name)+'&type='+encodeURIComponent(type))
            .then(r=>{ closeModal(); fetchRelays(); });
        }

        // --- AGENDAMENTO ----
        let schedRelay = 0;
        function openSched(idx) {
          schedRelay = idx;
          document.getElementById('sched-nome').innerText = relayData[idx].name;
          atualizaSchedFormTipo();
          fetch('/getsched?rele='+idx).then(r=>r.json()).then(list=>{
            let html = "<tr><th>Dia</th><th>Liga</th><th>Desliga</th><th></th></tr>";
            for (let i=0; i<list.length; i++) {
              html += "<tr>";
              html += "<td>"+(list[i].dia==0?"Todos":["Dom","Seg","Ter","Qua","Qui","Sex","Sab"][list[i].dia-1])+"</td>";
              html += "<td>"+("0"+list[i].h_on).slice(-2)+":"+("0"+list[i].m_on).slice(-2)+":"+("0"+list[i].s_on).slice(-2)+"</td>";
              html += "<td>"+("0"+list[i].h_off).slice(-2)+":"+("0"+list[i].m_off).slice(-2)+":"+("0"+list[i].s_off).slice(-2)+"</td>";
              html += "<td><button onclick='delEvent("+i+")'>Remover</button></td>";
              html += "</tr>";
            }
            document.getElementById('sched-table').innerHTML = html;
            document.getElementById('sched-bg').style.display = 'block';
            document.getElementById('sched').style.display = 'block';
          });
        }
        function closeSched() {
          document.getElementById('sched-bg').style.display = 'none';
          document.getElementById('sched').style.display = 'none';
        }
        function addEvent(ev) {
          ev.preventDefault();
          let d = relayTypes[schedRelay]=="Led" ? 0 : document.getElementById('sdia').value;
          let hon = document.getElementById('sh_on').value;
          let mon = document.getElementById('sm_on').value;
          let son = document.getElementById('ss_on').value;
          let hoff = document.getElementById('sh_off').value;
          let moff = document.getElementById('sm_off').value;
          let soff = document.getElementById('ss_off').value;
          fetch('/addsched?rele='+schedRelay+'&dia='+d+'&h_on='+hon+'&m_on='+mon+'&s_on='+son+'&h_off='+hoff+'&m_off='+moff+'&s_off='+soff)
            .then(r=>openSched(schedRelay));
        }
        function delEvent(idx) {
          fetch('/delsched?rele='+schedRelay+'&idx='+idx)
            .then(r=>openSched(schedRelay));
        }
        function escapeHTML(str) {
          return str.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
        }
        // Esconde dia da semana se for LED
        function atualizaSchedFormTipo() {
          setTimeout(()=>{
            let idx = schedRelay;
            let tipo = relayTypes[idx] || "Led";
            let diaDiv = document.getElementById('sched-dia-div');
            if (tipo == "Led") diaDiv.style.display = "none";
            else diaDiv.style.display = "block";
          }, 1);
        }
        // Validação para garantir range (hora/min/seg)
        function forceInputLimits() {
          let fields = ['sh_on','sm_on','ss_on','sh_off','sm_off','ss_off'];
          fields.forEach(id => {
            let el = document.getElementById(id);
            el.oninput = function() {
              let min = parseInt(el.min), max = parseInt(el.max);
              let v = parseInt(el.value);
              if (isNaN(v)) el.value = min;
              else if (v < min) el.value = min;
              else if (v > max) el.value = max;
              else el.value = ('0'+v).slice(-2);
            }
          });
        }
        document.addEventListener('DOMContentLoaded', forceInputLimits);
        fetchRelays(); fetchClock();
      </script>
    </body></html>
  )rawliteral";
  return page;
}

// --- Handlers web ---
void handleRoot() {
  server.send(200, "text/html", getPage());
}

void handleRelayData() {
  String js = "[";
  for (int i=0; i<NUM_RELAYS; i++) {
    if (i>0) js += ",";
    js += "{";
    js += "\"name\":\"" + htmlEscape(relays[i].name) + "\",";
    js += "\"type\":\"" + relays[i].type + "\",";
    js += "\"state\":" + String(relayStates[i] ? "true":"false");
    js += "}";
  }
  js += "]";
  server.send(200, "application/json", js);
}

void handleToggleRelay() {
  if (server.hasArg("rele")) {
    int idx = server.arg("rele").toInt();
    relay_toggle(idx);
  }
  server.send(200, "text/plain", "OK");
}

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
  server.send(200, "text/plain", "OK");
}

void handleClock() {
  time_t now = ntp_now();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char buf[40];
  snprintf(buf, sizeof(buf), "Hora atual: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  server.send(200, "text/plain", buf);
}

void handleGetSched() {
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

void handleAddSched() {
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

void handleDelSched() {
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

// ------- Funções para setup/loop global -------
void webinterface_setup() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/relaydata", handleRelayData);
  server.on("/toggle", handleToggleRelay);
  server.on("/setconfig", handleSetConfig);
  server.on("/clock", handleClock);
  server.on("/getsched", handleGetSched);
  server.on("/addsched", handleAddSched);
  server.on("/delsched", handleDelSched);

  server.begin();
}

void webinterface_loop() {
  server.handleClient();
}
