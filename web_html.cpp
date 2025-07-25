#include "web_html.h"

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
      <meta name='viewport' content='width=device-width, initial-scale=1' charset="UTF-8">
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
        #modal label{display:inline-block;width:45px;text-align:right;}
        #modal button,#sched button{width:auto;}
        .disabled{opacity:0.55;pointer-events:none;}
        #sched th{font-size:14px;}
        #sched td{font-size:15px;}
        .horariosBox {display:flex;flex-direction:column;gap:8px;}
        input[type=number]{padding:4px 2px;}
        .horarioLinha {
          display: flex;
          flex-direction: row;
          align-items: center;
          gap: 6px;
        }
        .horarioLinha span {
          min-width: 110px; /* igual para ambos */
          display: inline-block;
          text-align: right;
          margin-right: 6px;
        }
        .horarioLinha input[type="number"] {
          width: 54px; /* todos iguais */
          font-size: 16px;
          text-align: center;
          padding: 4px 2px;
        }
        .horarioLinha input[type="number"]:not(:last-child) {
          margin-right: 2px;
        }
        .manual-btn {
          color: #f44336; 
          font-weight: bold;
          font-size: 12px;
        }
        .manual-blink {
          animation: blink 0.5s steps(1) infinite;
          color: #f44336;
        }
        @keyframes blink {
          0%, 100% { opacity: 1; }
          50% { opacity: 0.1; }
        }
        .sem-agendamento {
          color: #888;
          font-size: 12px;
          font-style: italic;
          font-weight: 400;
          margin-left: 4px;
          vertical-align: middle;
        }
        button:disabled {
          background: #eee !important;
          color: #aaa !important;
          border: 1px solid #ddd;
          cursor: not-allowed;
        }
        #wavemaker-sched-bg {
          position: fixed;
          left: 0; top: 0;
          width: 100vw; height: 100vh;
          background: #0009;
          display: none;
          z-index: 3;
        }
        #wavemaker-sched {
          position: fixed;
          left: 50%; top: 50%;
          transform: translate(-50%,-50%);
          background: white;
          padding: 18px 18px 8px 18px;
          border-radius: 18px;
          display: none;
          z-index: 4;
          min-width: 290px;
        }

        button.remove-x {
          width: auto !important;
          height: auto !important;
          font-size: 16px !important;
          font-weight: bold;
          padding: 0 6px !important;
          margin: 0 !important;
          line-height: 1 !important;
          vertical-align: middle;
          background: none !important;
          border: none !important;
          color: #f44336;
          cursor: pointer;
          display: inline-block;
        }
        #sched-table tr, #sched-table td, #sched-table th {
          padding: 2px 4px !important;
          height: auto !important;
        }        
      </style>
    </head><body>      
      <h2>Controle de Relés ESP32</h2>
      <div style="display: flex; align-items: center; justify-content: space-between; margin-bottom: 10px;">
        <div id="clock" style="font-size:16px;"></div>
        <div id="debug-row" style="display: none;">
          <button style="font-size:12px; width:auto; padding:5px 12px;" onclick="debugSched()">Debug</button>
        </div>
      </div>
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
                <input type="number" min="0" max="23" id="sh_on" value="00">:
                <input type="number" min="0" max="59" id="sm_on" value="00">:
                <input type="number" min="0" max="59" id="ss_on" value="00">
              </div>
              <div class="horarioLinha">
                <span>Hora Desligar:</span>
                <input type="number" min="0" max="23" id="sh_off" value="00">:
                <input type="number" min="0" max="59" id="sm_off" value="00">:
                <input type="number" min="0" max="59" id="ss_off" value="00">
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
      <div id="wavemaker-sched-bg"></div>
      <div id="wavemaker-sched">
        <div style="font-size:17px;font-weight:bold;margin-bottom:8px">
          Agendamento Wavemaker: <span id="wavemaker-nome"></span>
        </div>
        <form onsubmit="saveWavemakerSched(event)">
          <label for="wavemaker-intervalo">Intervalo:</label>
          <select id="wavemaker-intervalo"></select>
          <div style="margin-top:10px; text-align:right;">
            <button type="button" onclick="closeWavemakerSched()">Cancelar</button>
            <button type="submit">Salvar</button>
          </div>
        </form>
      </div>

      <script>
        let relayData = [];
        let relayTypes = [];
        let fetchRelaysInterval = null;
        function fetchRelays() {
          fetch('/relaydata').then(r=>r.json()).then(js=>{
            relayData = js;
            relayTypes = js.map(r => r.type);
            let html = '';
            for (let i=0; i<js.length; i++) {
              html += "<div class='card'>";
              html += "<div class='label'>" + escapeHTML(js[i].name);
              if (js[i].num_sched !== undefined && js[i].num_sched === 0) {
                html += " <span class='sem-agendamento'>(Sem agendamento)</span>";
              }
              html += "</div>";
              html += "<div style='color:#444;font-size:13px'>" + js[i].type + "</div>";

              // Checa se o tipo é permitido:
              let tipo = js[i].type || "";
              let tipoValido = ["Led", "Rega", "Wavemaker", "Runoff"].includes(tipo);

              // Botão ON/OFF
              html += "<button class='"+(js[i].state?'on':'off')+"' onclick='toggleRelay("+i+")'"+(tipoValido ? "" : " disabled")+">"+(js[i].state?'ON':'OFF')+"</button> ";

              // Botão Editar
              html += "<button class='edit' onclick='editRelay("+i+")'>Editar</button>";

              // Botão Agendar
              html += "<button onclick='openSched("+i+")'"+(tipoValido ? "" : " disabled")+">Agendar</button>";

              // BOTÃO MANUAL: mostra se manual==true e (tem agendamento OU é wavemaker)
              if (
                js[i].manual &&
                ["Led", "Rega", "Runoff", "Wavemaker"].includes(js[i].type) &&
                js[i].num_sched > 0
              ) {
                html += "<button class='manual-btn' onclick='setAuto("+i+")'><span class='manual-blink'>MANUAL</span></button>";
              }

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
        // Atualize o estado dos relés a cada 1s (não menos!)
        if(fetchRelaysInterval) clearInterval(fetchRelaysInterval);
        fetchRelaysInterval = setInterval(fetchRelays, 1000);
        function toggleRelay(idx) {
          // Atualiza o estado local imediatamente (optimistic)
          if (relayData && relayData[idx]) {
            relayData[idx].state = !relayData[idx].state;
          }
          fetch('/toggle?rele='+idx)
            .then(() => {
              fetchRelays();
            })
            .catch(() => {
              fetchRelays();
            });
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
          let tipo = relayTypes[idx];
          if (tipo == "Wavemaker") {
            openWavemakerSched(idx);
            return;
          }
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
              html += "<td><button class='remove-x' onclick='delEvent("+i+")' title='Remover'>&times;</button></td>";
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
        const wavemakerOptions = [
          {label: "Sempre ligado", on: 24*60*60, off: 0},
          {label: "15min ligado/15min desligado", on: 15*60, off: 15*60},
          {label: "30min ligado/30min desligado", on: 30*60, off: 30*60},
          {label: "1h ligado/1h desligado", on: 60*60, off: 60*60},
          {label: "6h ligado/6h desligado", on: 6*60*60, off: 6*60*60},
          {label: "12h ligado/12h desligado", on: 12*60*60, off: 12*60*60}
        ];


        // Se DEBUG_MODE, adiciona a opção 10s/10s:
        let DEBUG_MODE = false; 
        fetch('/debug').then(r=>r.text()).then(flag=>{DEBUG_MODE = (flag=="1");});

        function openWavemakerSched(idx) {
          schedRelay = idx;
          document.getElementById('wavemaker-nome').innerText = relayData[idx].name;
          let sel = document.getElementById('wavemaker-intervalo');
          sel.innerHTML = "";

          // Busca agendamento atual
          fetch('/getsched?rele=' + idx).then(r => r.json()).then(list => {
            // Descobre o intervalo salvo (considera só o primeiro evento do dia, que é o padrão do backend)
            let intervaloSalvo = null;
            if (list.length > 0) {
              let ev = list[0];
              let on = (ev.h_off * 3600 + ev.m_off * 60 + ev.s_off) - (ev.h_on * 3600 + ev.m_on * 60 + ev.s_on);
              let off = 0;
              if (list.length > 1) {
                let next_ev = list[1];
                off = (next_ev.h_on * 3600 + next_ev.m_on * 60 + next_ev.s_on) - (ev.h_off * 3600 + ev.m_off * 60 + ev.s_off);
                if (off < 0) off += 24*3600; // Se passar de meia noite
              }
              intervaloSalvo = {on, off};
            }

            // Preenche opções do combo e seleciona a correspondente, se existir
            let selectedIdx = 0;
            wavemakerOptions.forEach((opt, i) => {
              let option = document.createElement('option');
              option.value = i;
              option.text = opt.label;
              if (intervaloSalvo && opt.on === intervaloSalvo.on && opt.off === intervaloSalvo.off) {
                selectedIdx = i;
              }
              sel.add(option);
            });
            if (DEBUG_MODE) {
              let option = document.createElement('option');
              option.value = "debug";
              option.text = "10s ligado/10s desligado";
              if (intervaloSalvo && intervaloSalvo.on === 10 && intervaloSalvo.off === 10)
                option.selected = true;
              sel.add(option);
            }
            sel.selectedIndex = selectedIdx;

            document.getElementById('wavemaker-sched-bg').style.display = "block";
            document.getElementById('wavemaker-sched').style.display = "block";
          });
        }

        function closeWavemakerSched() {
          document.getElementById('wavemaker-sched-bg').style.display = "none";
          document.getElementById('wavemaker-sched').style.display = "none";
        }

        function saveWavemakerSched(ev) {
          ev.preventDefault();
          let sel = document.getElementById('wavemaker-intervalo');
          let optIdx = sel.value;
          let on, off;
          if(optIdx === "debug") {
            on = 10; off = 10;
          } else {
            on = wavemakerOptions[optIdx].on;
            off = wavemakerOptions[optIdx].off;
          }
          // Manda pro backend (intervalo em segundos):
          fetch(`/setwavemakersched?rele=${schedRelay}&on=${on}&off=${off}`)
            .then(()=>{ closeWavemakerSched(); fetchRelays(); });
        }

        function escapeHTML(str) {
          return str.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
        }
        function atualizaSchedFormTipo() {
          setTimeout(()=>{
            let idx = schedRelay;
            let tipo = relayTypes[idx] || "Led";
            let diaDiv = document.getElementById('sched-dia-div');
            if (tipo == "Led") diaDiv.style.display = "none";
            else diaDiv.style.display = "block";
          }, 1);
        }
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

        // NOVO: Função para voltar para automático
        function setAuto(idx) {
          fetch('/setauto?rele=' + idx)
            .then(r => fetchRelays());
        }
        function debugSched() {
          fetch('/debugsched')
            .then(()=> {
              fetchRelays();
            });
        }
        document.addEventListener('DOMContentLoaded', function() {
          fetch('/debug')
            .then(r => r.text())
            .then(flag => {
              if(flag === "1") document.getElementById('debug-row').style.display = '';
            });
        });
      </script>
    </body></html>
  )rawliteral";
  return page;
}
