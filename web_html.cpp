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

String getPage(bool debug) {
  
  String page = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <style>
    body {
      background: #f6f8fa;
      font-family: sans-serif;
      margin: 0;
    }
    #clock {
      width: 100%;
      max-width: 430px;
      margin: 20px auto 12px auto;
      text-align: center;
      font-size: 1.23em;
      font-family: monospace;
      font-weight: bold;
      letter-spacing: 2px;
      color: #333;
      background: none;
    }
    .cards {
      max-width: 430px;
      margin: 0 auto 0 auto;
      display: flex;
      flex-direction: column;
      gap: 8px;
    }
    .card {
      background: #fff;
      border-radius: 10px;
      padding: 8px 12px;
      box-shadow: 0 1px 5px #0001;
      display: flex;
      align-items: stretch;
      cursor: pointer;
      transition: box-shadow .18s;
      position: relative;
      min-height: 38px;
    }
    .card:hover {
      box-shadow: 0 2px 14px #2a8cf822;
    }
    .icon {
      width: 1.4em;
      font-size: 1.3em;
      display: flex;
      align-items: center;      /* Centraliza verticalmente */
      justify-content: center;
      flex-shrink: 0;
      /* margin-top: 2px;  Remova esta linha para alinhar melhor */
    }

    .labels {
      flex: 1 1 auto;
      display: flex;
      flex-direction: column;
      align-items: flex-start;
      justify-content: flex-start;
      margin-left: 10px;
      min-width: 0;
      gap: 0px;
      padding-top: 2px;
    }
    .labels > div:first-child {
      display: flex;
      flex-direction: column;
      align-items: flex-start;
      margin-bottom: 2px;
    }
    .label-nome {
      font-weight: 600;
      font-size: 0.8em;
      color: #333;
      line-height: 1.1;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      margin-bottom: 1px;
    }
    .label-tipo {
      font-size: 0.62em;
      color: #98a;
      margin-top: 0px;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      line-height: 1.1;
    }
    .status-linha {
      font-size: 0.65em;
      color: #678;
      margin-top: 0px;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
      min-height: 1em;
    }
    .switch-num-wrap {
      display: flex;
      align-items: center;
      gap: 7px;
      margin-left: 14px;
    }
    .channel-num {
      width: 1.2em;
      font-size: 1.15em;
      color: #333;
      display: flex;
      align-items: center;
      justify-content: center;
      font-variant-numeric: tabular-nums;
      font-weight: 600;
      letter-spacing: 0.3px;
      flex-shrink: 0;
    }
    .card.empty {
      border: 2px dashed #cdd;
      background: #f8fafc;
      color: #b9bec5;
      position: relative;
      min-height: 38px;
      padding: 8px 12px;
      display: flex;
      align-items: center;
      justify-content: flex-start;
      cursor: pointer;
    }
    .card.empty .plus-area {
      position: absolute;
      left: 0;
      top: 0;
      right: 0;
      bottom: 0;
      display: flex;
      align-items: center;
      justify-content: center;
      pointer-events: none;
      z-index: 1;
    }
    .card.empty .plus-btn {
      font-size: 1.2em;
      background: none;
      border: none;
      outline: none;
      color: #b9bec5;
      display: flex;
      align-items: center;
      justify-content: center;
      pointer-events: none;
      user-select: none;
      transition: none;
    }
    .card.empty:hover .plus-btn {
      color: #b9bec5;
      background: none;
    }
    .card.empty .switch-num-wrap {
      margin-left: auto;
      position: relative;
      z-index: 2;
    }
    .card.empty .channel-num {
      color: #bcc3cc;
      font-weight: bold;
    }
    .toggle-switch {
      position: relative;
      display: inline-block;
      width: 30px;
      height: 16px;
      vertical-align: middle;
    }
    .toggle-switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      border-radius: 22px;
      transition: .2s;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 12px;
      width: 12px;
      left: 2px;
      bottom: 2px;
      background: #fff;
      border-radius: 50%;
      transition: .2s;
    }
    input:checked + .slider {
      background: #16c869;
    }
    input:checked + .slider:before {
      transform: translateX(14px);
    }
    .toggle-switch input:not(:checked) + .slider {
      background: #b71c1c;
    }
    #modal-bg {
      position:fixed;
      left:0;
      top:0;
      width:100vw;
      height:100vh;
      background:#0009;
      display:none;
      z-index:10;
    }
    #modal-config,
    #modal-agendamentos {
      position:fixed;
      left:50%;
      top:50%;
      transform:translate(-50%,-50%);
      background:white;
      padding:20px 24px 18px 24px;
      border-radius:18px;
      display:none;
      z-index:20;
      min-width:320px;
      max-width:92vw;
    }
    #modal-config label, #modal-agendamentos label {
      font-size:15px;
      font-weight:600;
      margin-bottom:7px;
      display:block;
    }
    #modal-config input[type="text"] {
      width:100%;
      padding:8px;
      font-size:17px;
      border:1px solid #c5cdd3;
      border-radius:8px;
      margin-bottom:14px;
      box-sizing: border-box;
    }
    }
    #modal-agendamentos input[type="text"], #modal-agendamentos input[type="number"] {
      width:100%;
      padding:8px;
      font-size:17px;
      border:1px solid #c5cdd3;
      border-radius:8px;
      //margin-bottom:14px;
      box-sizing: border-box;
    }
    .type-list {
      list-style:none;
      margin:0;
      padding:0;
      display:flex;
      flex-direction:column;
      gap:7px;
      margin-bottom:18px;
    }
    .type-list li {
      display:flex;
      align-items:center;
      justify-content:space-between;
      border:1.5px solid #d2dae6;
      border-radius:11px;
      padding:11px 16px;
      font-size:17px;
      cursor:pointer;
      background:#f7fafd;
      transition:box-shadow .13s, border .13s, background .13s;
    }
    .type-list li.selected,
    .type-list li:active {
      border:2px solid #39f;
      background:#e8f3fd;
    }
    .type-list li:hover:not(.selected) {
      box-shadow:0 2px 12px #39f2;
      border:1.5px solid #39f;
      background:#e8f3fd;
    }
    .type-list .type-ico {
      font-size:1.23em;
      margin-left:16px;
    }
    .type-list .type-led { color:#e1be11; }
    .type-list .type-rega { color:#21b4c5; }
    .type-list .type-runoff { color:#8b6e54; }
    .type-list .type-wavemaker { color:#27a162; }
    .modal-actions {
      display:flex;
      justify-content:flex-end;
      gap:10px;
    }
    .modal-actions button {
      font-size:16px;
      padding:7px 22px;
      border-radius:8px;
      border:none;
    }
    .modal-actions .cancel-btn {
      background:#eee;
      color:#555;
    }
    .modal-actions .save-btn {
      background:#39f;
      color:#fff;
      font-weight:700;
    }
    .modal-actions .save-btn:disabled {
      background:#bbd2f3;
      color:#eee;
    }
    #modal-agendamentos {
      padding: 22px 20px 16px 20px;
      min-width: 330px;
      max-width: 99vw;
    }
    #modal-agendamentos h2 {
      margin: 0 0 13px 0;
      font-size: 1.12em;
      font-weight: 700;
      color: #222;
      letter-spacing: 0.5px;
    }
    #schedDay {
      min-width: 80px !important;
      width: 80px !important;
      height: 32px;
      font-weight: 600;
      font-size: 1em;      
      padding: 5px 3px;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
    }
    #schedDay, #schedDay option {
      text-align: left;
      color: #444;
    }
    .sched-list {
      list-style: none;
      padding: 0;
      margin: 0 0 13px 0;
      display: flex;
      flex-direction: column;
      gap: 8px;
    }
    .sched-list li {
      display: flex;
      align-items: center;
      background: #f4f8fc;
      border-radius: 8px;
      padding: 7px 11px 7px 13px;
      font-size: 0.97em;
      justify-content: space-between;
      box-shadow: 0 1px 6px #bbb2;
    }
    .sched-list .sched-info {
      display: flex;
      gap: 17px;
      align-items: center;
      flex: 1 1 auto;
    }
    .sched-list .sched-diasemana {
      font-weight: 700;
      color: #445;
      min-width: 44px;
    }
    .sched-list .sched-horas {
      font-family: monospace;
      letter-spacing: 1px;
      color: #295;
      min-width: 98px;
      font-size: 1em;
      font-weight: 600;
    }
    .sched-list button {
      background: #f55;
      color: #fff;
      border: none;
      padding: 4px 13px;
      border-radius: 6px;
      font-size: 0.95em;
      font-weight: 600;
      cursor: pointer;
      margin-left: 8px;
      transition: background .17s;
    }
    .sched-list button:hover {
      background: #c12;
    }
    .add-sched-form {
      display: flex;
      flex-direction: column;
      gap: 5px;
      align-items: flex-start;
      margin-top: 6px;
      margin-bottom: 9px;
    }
    .add-sched-form > div {
      display: flex;
      align-items: center;
      gap: 7px;
      margin-bottom: 2px;
    }
    .add-sched-form select,
    .add-sched-form input[type="number"] {
      width: 3.2em !important;
      max-width: 3.2em;
      min-width: 3.2em;
      font-size: 1em;
      text-align: center;
      padding: 3px 0;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
      box-sizing: border-box;
      height: 32px;
    }
    .add-sched-form select {
      min-width: 78px;
      font-weight: 600;
      font-size: 1em;
      padding: 5px 7px;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
    }
    .add-sched-form .add-btn,
    .agendamentos-close-btn {
      margin-top: 0;
      margin-left: 0;
    }
    .add-sched-form .add-btn {
      background: #39f;
      color: #fff;
      font-weight: 600;
      padding: 7px 17px;
      border-radius: 8px;
      border: none;
      cursor: pointer;
      font-size: 1em;
      transition: background .17s;
    }
    .add-sched-form .add-btn:disabled {
      background: #b9d3f9;
      color: #eee;
      cursor: not-allowed;
    }
    .agendamentos-close-btn {
      background: #eee;
      color: #333;
      border: none;
      border-radius: 8px;
      padding: 7px 22px;
      font-size: 1em;
      font-weight: 500;
      cursor: pointer;
      transition: background .15s;
    }
    .agendamentos-close-btn:hover {
      background: #ddd;
    }
    /* === PADRONIZAÇÃO DAS LABELS E DOS ":" NO MODAL AGENDAMENTOS === */
    #modal-agendamentos .add-sched-form label {
      font-size: 15px;
      font-weight: 600;
      color: #444;
      margin: 0;
      padding: 0;
      text-align: left;
      display: flex;
      align-items: center;
    }
    #modal-agendamentos .add-sched-form label[for="schedHOn"],
    #modal-agendamentos .add-sched-form label[for="schedHOff"] {
      width: 58px;  /* ajuste esse valor até alinhar perfeitamente */
      min-width: 0;  /* remove o min-width, só para garantir */
      max-width: 98px;
      display: flex;
      align-items: center;
      position: relative;
      //top: -7px;   /* ajuste para centralizar, aumente para subir mais */
    }
    #modal-agendamentos .add-sched-form span {
      font-size: 15px;
      font-weight: 600;
      color: #444;
      display: flex;
      align-items: center;
      height: 32px;
      line-height: 1;
      padding: 0 0px;
      position: relative;
      //top: -7px; /* OU ajuste entre 0~2px até bater o alinhamento visual */
    }
    #modal-agendamentos .add-sched-form select {
      font-size: 15px;
    }
    .clock-bar {
      position: relative;
      max-width: 430px;
      margin: 20px auto 12px auto;
      height: 40px; /* Ajuste se necessário, para acomodar o botão */
    }

    #clock {
      text-align: center;
      font-size: 1.23em;
      font-family: monospace;
      font-weight: bold;
      letter-spacing: 2px;
      color: #333;
      line-height: 40px; /* Igual à altura da .clock-bar, para centralizar verticalmente */
    }

    #debugBtn {
      position: absolute;
      right: 0;
      top: 50%;
      transform: translateY(-50%);
      margin-left: 0;
      padding: 3px 12px;
      background: #445;
      color: #eee;
      border: none;
      border-radius: 8px;
      font-size: 12px;
      font-weight: 600;
      box-shadow: 0 1px 3px #0002;
      cursor: pointer;
      transition: background .14s;
      display: none;
    }
    #debugBtn:hover {
      background: #9ca3ad;
      color: #eee;
    }
    #modal-wavemaker {
      position:fixed;
      left:50%;
      top:50%;
      transform:translate(-50%,-50%);
      background:white;
      padding:22px 20px 16px 20px;
      border-radius:18px;
      display:none;
      z-index:21;
      min-width:330px;
      max-width:98vw;
    }

  </style>
</head>
<body>
  <div class="clock-bar">
    <div id="clock"></div>
    <button id="debugBtn" style="display:none;">Debug</button>
  </div>
  <div class="cards" id="cards"></div>
  <div id="modal-bg"></div>
  <div id="modal-config">
    <form id="configForm" autocomplete="off">
      <label for="nameInput">Nome do dispositivo</label>
      <input type="text" id="nameInput" maxlength="24" autocomplete="off" required>
      <label>Tipo</label>
      <ul class="type-list" id="typeList">
        <li data-type="Led"><span>Led</span> <span class="type-ico type-led">💡</span></li>
        <li data-type="Rega"><span>Rega</span> <span class="type-ico type-rega">💧</span></li>
        <li data-type="Runoff"><span>Runoff</span> <span class="type-ico type-runoff">🧺</span></li>
        <li data-type="Wavemaker"><span>Wavemaker</span> <span class="type-ico type-wavemaker">💨</span></li>
      </ul>
      <div class="modal-actions">
        <button type="button" class="cancel-btn" onclick="closeConfigModal()">Cancelar</button>
        <button type="submit" class="save-btn" id="saveBtn" disabled>Salvar</button>
      </div>
      <input type="hidden" id="relayIdx">
    </form>
  </div>
  <!-- Modal Agendamentos -->
  <div id="modal-agendamentos">
    <ul class="sched-list" id="schedList"></ul>
    <form class="add-sched-form" id="addSchedForm" autocomplete="off">
      <div style="display:flex; align-items:center; gap:4px; margin-bottom:4px; margin-top:8px;">
        <label for="schedDay" style="min-width:30px;">Dia:</label>
        <select id="schedDay">
          <option value="0">Todos</option>
          <option value="1">Dom</option>
          <option value="2">Seg</option>
          <option value="3">Ter</option>
          <option value="4">Qua</option>
          <option value="5">Qui</option>
          <option value="6">Sex</option>
          <option value="7">Sáb</option>
        </select>
      </div>
      <div style="display:flex; align-items:center; gap:4px;">
        <label for="schedHOn" style="min-width:58px;">Liga:</label>
        <input type="number" id="schedHOn" min="0" max="23" placeholder="H" maxlength="2" required style="width:2.2em;text-align:center;">
        <span>:</span>
        <input type="number" id="schedMOn" min="0" max="59" placeholder="M" maxlength="2" required style="width:2.2em;text-align:center;">
        <span>:</span>
        <input type="number" id="schedSOn" min="0" max="59" placeholder="S" maxlength="2" required style="width:2.2em;text-align:center;">
      </div>
      <div style="display:flex; align-items:center; gap:4px; margin-bottom:10px;">
        <label for="schedHOff" style="min-width:58px;">Desliga:</label>
        <input type="number" id="schedHOff" min="0" max="23" placeholder="H" maxlength="2" required style="width:2.2em;text-align:center;">
        <span>:</span>
        <input type="number" id="schedMOff" min="0" max="59" placeholder="M" maxlength="2" required style="width:2.2em;text-align:center;">
        <span>:</span>
        <input type="number" id="schedSOff" min="0" max="59" placeholder="S" maxlength="2" required style="width:2.2em;text-align:center;">
      </div>
      <div style="display:flex; gap:12px; margin-top:2px;">
        <button type="submit" class="add-btn">Adicionar</button>
        <button type="button" class="agendamentos-close-btn" onclick="closeAgendamentoModal()">Fechar</button>
      </div>
    </form>
  </div>
  <!-- Modal Agendamento Wavemaker -->
  <div id="modal-wavemaker" style="display:none;">
    <h2 style="margin-top:0; margin-bottom:16px; font-size:1.13em;">Agendamento Wavemaker</h2>
    <ul class="type-list" id="wavemakerOptionList" style="margin-bottom:0;">
      <li data-opt="0"><span>Sempre ligado</span></li>
      <li data-opt="1"><span>15min ligado/15min desligado</span></li>
      <li data-opt="2"><span>30min ligado/30min desligado</span></li>
      <li data-opt="3"><span>1h ligado/1h desligado</span></li>
      <li data-opt="4"><span>6h ligado/6h desligado</span></li>
      <li data-opt="5"><span>12h ligado/12h desligado</span></li>
      <li data-opt="debug" style="display:none;"><span>10s ligado/10s desligado</span></li>
    </ul>
  </div>
  <div id="modal-bg-wavemaker" style="display:none;position:fixed;left:0;top:0;width:100vw;height:100vh;background:#0009;z-index:10;"></div>

  <script>
    const DEBUG_MODE = )rawliteral";
page += (debug ? "true" : "false");
page += R"rawliteral(;

    let relayData = Array.from({length: 8}, (_,i)=>({ name: "", type: "", state: false, num_sched: 0, manual: false }));
    // Mostra o botão de debug se DEBUG_MODE for true
    if (typeof DEBUG_MODE !== 'undefined' && DEBUG_MODE) {
      document.getElementById("debugBtn").style.display = "block";
      document.getElementById("debugBtn").onclick = function() {
        fetch("/debugsched").then(r => {
          if (r.ok) fetchRelays();
        });
      };
    } else {
      // Se preferir, pode esconder explicitamente se não estiver no modo debug:
      document.getElementById("debugBtn").style.display = "none";
    }

    function getStatusLinha(device) {
      console.log("Dispositivo:", device.name, JSON.stringify(device.horarios));
      // 1. Se for wavemaker, retorna o modo apropriado imediatamente
      if (device.type === "Wavemaker") {
        const modos = [
          "Sempre ligado",
          "15min Ligado/15min Desligado",
          "30min Ligado/30min Desligado",
          "1h Ligado/1h Desligado",
          "6h Ligado/6h Desligado",
          "12h Ligado/12h Desligado",
          "10s Ligado/10s Desligado"
        ];
        let idx = typeof device.wavemaker_mode === "number" ? device.wavemaker_mode : parseInt(device.wavemaker_mode);
        return modos[idx] || "Modo indefinido";
      }

      // 2. Sem agendamento? Retorna mensagem padrão
      if (!device.has_schedule || !device.horarios || device.horarios.length === 0)
        return "Sem agendamentos";

      // ... restante do código para demais tipos ...
      const agora = new Date();
      const diaAtual = agora.getDay(); // 0=Dom, 1=Seg...
      const segundosAtuais = agora.getHours() * 3600 + agora.getMinutes() * 60 + agora.getSeconds();
      const diasSemana = ["Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"];

      let proximoEvento = null;
      let menorDiferenca = 8 * 24 * 3600; // mais que uma semana

      device.horarios.forEach(ev => {
        let diasParaTestar = [];
        if (ev.dia === "Todos" || ev.dia === 0 || ev.dia === "0") {
          diasParaTestar = [0,1,2,3,4,5,6];
        } else if (typeof ev.dia === "number") {
          diasParaTestar = [ev.dia];
        } else {
          let idx = diasSemana.indexOf(ev.dia);
          if (idx >= 0) diasParaTestar = [idx];
        }

        let evHora = (ev.hora || ev.h_on || "00:00:00").split(':').map(Number);
        let evSegundos = evHora[0]*3600 + evHora[1]*60 + evHora[2];

        diasParaTestar.forEach(idxDiaEv => {
          let diasAteEvento = idxDiaEv - diaAtual;
          if (diasAteEvento < 0) diasAteEvento += 7;

          let difSeg = diasAteEvento * 86400 + (evSegundos - segundosAtuais);
          if (difSeg < 0) difSeg += 7 * 86400; // Garante só futuro

          if (difSeg < menorDiferenca) {
            menorDiferenca = difSeg;
            proximoEvento = {
              acao: ev.acao || "liga",
              hora: (ev.hora || ev.h_on || "00:00:00"),
              dia: idxDiaEv
            };
          }
        });
      });

      if (!proximoEvento) return "Sem agendamentos";

      let mostrarDia = "hoje";
      if (proximoEvento.dia !== diaAtual) {
        mostrarDia = diasSemana[proximoEvento.dia];
      }

      let acao = proximoEvento.acao === "liga" ? "Liga" : "Desliga";
      return `${acao} ${mostrarDia} ${proximoEvento.hora}`;
    }

    function getIcon(type) {
      if (type === "LED" || type === "Led") return "💡";
      if (type === "Rega") return "💧";
      if (type === "Ventilador") return "🌀";
      if (type === "Wavemaker") return "💨";
      if (type === "Runoff") return "🧺";
      return "🔌";
    }

    function renderCards(relayData) {
      let html = "";
      for (let i = 0; i < 8; i++) {
        const d = relayData[i];
        if (d && d.name && d.name.trim() !== "") {
          html += `
            <div class="card" data-idx="${i}">
              <span class="icon">${getIcon(d.type)}</span>
              <div class="labels">
                <div>
                  <span class="label-nome">${d.name}</span>
                  <span class="label-tipo">${d.type}</span>
                </div>
                <div class="status-linha">${getStatusLinha(d)}</div>
              </div>
              <div class="switch-num-wrap">
                <label class="toggle-switch" onclick="event.stopPropagation()">
                  <input type="checkbox" data-idx="${i}" ${d.state ? "checked" : ""}>
                  <span class="slider"></span>
                </label>
                <span class="channel-num">${i+1}</span>
              </div>
            </div>
          `;
        } else {
          html += `
          <div class="card empty" data-idx="${i}">
            <div class="plus-area">
              <span class="plus-btn">＋</span>
            </div>
            <div class="switch-num-wrap">
              <span class="channel-num">${i+1}</span>
            </div>
          </div>
          `;
        }
      }
      document.getElementById("cards").innerHTML = html;
      setupListeners();
    }

    function setupListeners() {
      const cardsDiv = document.getElementById("cards");
      cardsDiv.onclick = function(event) {
        let card = event.target.closest('.card');
        if (!card) return;
        if (event.target.matches('input[type="checkbox"]')) return;
        const idx = parseInt(card.getAttribute('data-idx'));
        if (card.classList.contains('empty')) {
          openConfigModal(idx);
        } else {
          // Checa o tipo do device aqui!
          const tipo = relayData[idx]?.type || "";
          if (tipo === "Wavemaker") {
            openWavemakerModal(idx);
          } else {
            openAgendamentoModal(idx);
          }
        }
      };
      cardsDiv.querySelectorAll('input[type="checkbox"]').forEach(cb => {
        cb.onclick = function(event) {
          event.stopPropagation();
          const idx = parseInt(cb.getAttribute('data-idx'));
          const checked = cb.checked;
          toggleRelay(idx, checked);
        };
      });
    }
    function updateStatusLinhas() {
      const cardDivs = document.querySelectorAll('.card:not(.empty)');
      cardDivs.forEach((card, i) => {
        const idx = parseInt(card.getAttribute('data-idx'));
        const device = relayData[idx];
        if (!device) return;
        const statusDiv = card.querySelector('.status-linha');
        if (statusDiv) {
          // Adicione este log:
          console.log("Recalculando status", device.name, getStatusLinha(device));
          statusDiv.textContent = getStatusLinha(device);
        }
        // Atualiza botão on/off
        const checkbox = card.querySelector('input[type="checkbox"]');
        if (checkbox) checkbox.checked = !!device.state;
      });
    }

    // ---- Modal moderno ----
    let selectedType = null;
    function openConfigModal(idx) {
      document.getElementById('modal-bg').style.display = 'block';
      document.getElementById('modal-config').style.display = 'block';
      document.getElementById('nameInput').value = "";
      document.getElementById('relayIdx').value = idx;
      selectedType = null;
      document.querySelectorAll('.type-list li').forEach(li => li.classList.remove('selected'));
      updateSaveBtn();
    }

    function closeConfigModal() {
      document.getElementById('modal-bg').style.display = 'none';
      document.getElementById('modal-config').style.display = 'none';
    }

    document.querySelectorAll('.type-list li').forEach(li => {
      li.onclick = function() {
        document.querySelectorAll('.type-list li').forEach(li2 => li2.classList.remove('selected'));
        li.classList.add('selected');
        selectedType = li.getAttribute('data-type');
        updateSaveBtn();
      };
    });

    document.getElementById('nameInput').oninput = updateSaveBtn;
    function updateSaveBtn() {
      const nameOk = document.getElementById('nameInput').value.trim().length > 0;
      document.getElementById('saveBtn').disabled = !(nameOk && selectedType);
    }

    document.getElementById('configForm').onsubmit = function(ev) {
      ev.preventDefault();
      const idx = document.getElementById('relayIdx').value;
      const name = document.getElementById('nameInput').value.trim();
      const type = selectedType;
      if (!name || !type) return;
      fetch('/setconfig?rele='+idx+'&name='+encodeURIComponent(name)+'&type='+encodeURIComponent(type))
        .then(r=>{ closeConfigModal(); setTimeout(fetchRelays, 300); });
    };

    function fetchRelays() {
      fetch("/relaydata").then(r=>r.json()).then(js=>{
        relayData = js;
        renderCards(relayData);
      });
    }

    function toggleRelay(idx, checked) {
      fetch("/toggle?rele=" + idx)
        .then(() => { setTimeout(fetchRelays, 200); });
    }

    function fetchClock() {
      fetch("/clock").then(r => r.text()).then(txt => {
        let t = txt.replace(/^Hora atual:\s*/, '');
        if (/^\d\d:\d\d:\d\d$/.test(t)) {
          const dias = ["DOM","SEG","TER","QUA","QUI","SEX","SAB"];
          const now = new Date();
          const dia = dias[now.getDay()];
          t = dia + " " + t;
        }
        document.getElementById("clock").textContent = t;
      });
    }
    
    // Inicializa cards na tela
    fetch("/relaydata").then(r=>r.json()).then(js=>{
      relayData = js;
      renderCards(relayData);
    });

    setInterval(fetchClock, 1000);
    fetchClock();

    // Atualiza status-linha e botão on/off a cada segundo COM os dados mais recentes
    setInterval(() => {
      fetch("/relaydata")
        .then(r => r.json())
        .then(js => {
          relayData = js;
          updateStatusLinhas();
        });
    }, 1000);

    // ---- Modal Agendamento NOVO ----
    let agendamentoIdx = null;
    function openAgendamentoModal(idx) {
      agendamentoIdx = idx;
      document.getElementById('modal-bg').style.display = 'block';
      document.getElementById('modal-agendamentos').style.display = 'block';
      document.getElementById('addSchedForm').reset();
      fetchAgendamentos(idx);

      // Diferenciar para LED: esconder campo Dia
      const tipo = relayData[idx]?.type || "";
      const campoDia = document.querySelector('#addSchedForm [for="schedDay"]').parentElement;
      if (tipo === "Led" || tipo === "LED") {
        campoDia.style.display = "none";
        document.getElementById('schedDay').value = "0";
      } else {
        campoDia.style.display = "";
      }
    }
    function closeAgendamentoModal() {
      document.getElementById('modal-bg').style.display = 'none';
      document.getElementById('modal-agendamentos').style.display = 'none';
      agendamentoIdx = null;
    }
    function fetchAgendamentos(idx) {
      fetch('/getsched?rele=' + idx)
        .then(r=>r.json())
        .then(js => {
          let html = '';
          const dias = ["Todos","Dom","Seg","Ter","Qua","Qui","Sex","Sáb"];
          js.forEach((ev, i) => {
            html += `<li>
                <span class="sched-info">
                  <span class="sched-diasemana">${dias[ev.dia]}</span>
                  <span class="sched-horas">${pad2(ev.h_on)}:${pad2(ev.m_on)}:${pad2(ev.s_on)} ~ ${pad2(ev.h_off)}:${pad2(ev.m_off)}:${pad2(ev.s_off)}</span>
                </span>
                <button onclick="delAgendamento(${i})" title="Excluir">&#128465;</button>
              </li>`;
          });
          document.getElementById('schedList').innerHTML = html || '<li style="color:#999;font-style:italic;">Nenhum agendamento cadastrado.</li>';
        });
    }
    function pad2(n) {
      return n.toString().padStart(2, "0");
    }
    function delAgendamento(evIdx) {
      const idx = agendamentoIdx;
      fetch(`/delsched?rele=${idx}&idx=${evIdx}`, {method:"POST"})
        .then(()=>{ fetchAgendamentos(idx); });
    }
    document.getElementById('addSchedForm').onsubmit = function(ev) {
      ev.preventDefault();
      const idx = agendamentoIdx;
      const dia = document.getElementById('schedDay').value;
      const h_on = document.getElementById('schedHOn').value;
      const m_on = document.getElementById('schedMOn').value;
      const s_on = document.getElementById('schedSOn').value;
      const h_off = document.getElementById('schedHOff').value;
      const m_off = document.getElementById('schedMOff').value;
      const s_off = document.getElementById('schedSOff').value;
      fetch(`/addsched?rele=${idx}&dia=${dia}&h_on=${h_on}&m_on=${m_on}&s_on=${s_on}&h_off=${h_off}&m_off=${m_off}&s_off=${s_off}`)
        .then(()=>{
          fetchAgendamentos(idx);
          // ATUALIZA O CARD DE TODOS OS RELÉS, INCLUINDO ON/OFF E STATUS-LINHA
          fetchRelays();
          document.getElementById('addSchedForm').reset();
        });
    }
    // Abre o modal (chame essa função ao clicar em "Agendar" para Wavemaker)
    let wavemakerIdx = null;

    function openWavemakerModal(idx) {
      wavemakerIdx = idx; // Salva o índice
      document.getElementById('modal-bg-wavemaker').style.display = 'block';
      document.getElementById('modal-wavemaker').style.display = 'block';

      // Se for modo debug, mostra a opção extra
      if (DEBUG_MODE)
        document.querySelector('#wavemakerOptionList li[data-opt="debug"]').style.display = '';
      else
        document.querySelector('#wavemakerOptionList li[data-opt="debug"]').style.display = 'none';
    }


    // Fecha o modal
    function closeWavemakerModal() {
      document.getElementById('modal-bg-wavemaker').style.display = 'none';
      document.getElementById('modal-wavemaker').style.display = 'none';
      wavemakerIdx = null;
    }

    // Lógica de clique nas opções
    document.querySelectorAll('#wavemakerOptionList li').forEach(li => {
      li.onclick = function() {
        let opt = li.getAttribute('data-opt');
        // 'debug' para a opção extra; converte para 6 ou outro índice se quiser
        let mode = (opt === "debug") ? 6 : parseInt(opt);
        selectWavemakerOption(wavemakerIdx, mode);
      }
    });
    // Clica no fundo para fechar também (opcional)
    document.getElementById('modal-bg-wavemaker').onclick = closeWavemakerModal;
    function selectWavemakerOption(idx, mode) {
      fetch(`/setwavemakermode?rele=${idx}&mode=${mode}`)
        .then(() => {
          closeWavemakerModal();
          fetchRelays(); // Atualiza a tela depois!
        });
    }
    fetchRelays();
  </script>
</body>
</html>
  )rawliteral";
  return page;
}
