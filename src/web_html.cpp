#include "web_html.h"

/**
 * Função utilitária para escapar caracteres perigosos do HTML.
 * Previne injeção ou quebras no template.
 */
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


/**
 * Retorna o HTML principal da interface web.
 * Inclui todo CSS, modais, frontend JS, etc.
 * O parâmetro debug ativa elementos extras para depuração.
 */
String getPage(bool debug) {
  
  String page = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <style>
    /* === GERAL: layout base, relógio, cards principais === */
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
      line-height: 40px;
    }
    .clock-bar {
      max-width: 430px;
      margin: 20px auto 12px auto;
      height: 40px;
      position: relative;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .settings-btn {
      position: absolute;
      right: 0;
      top: 50%;
      transform: translateY(-50%);
      background: none;
      border: none;
      color: #555;
      font-size: 1.0em;
      border-radius: 7px;
      transition: background 0.15s, color 0.15s;
      padding: 2px 16px 0 6px;
      height: 34px;
      display: flex;
      align-items: center;
      cursor: pointer;
    }
    .main-buttons {
      display: flex;
      justify-content: center;
      align-items: center;
      gap: 10px;
      margin: 30px 0 12px 0;
      position: static;
    }
    .mainBtn {
      padding: 7px 22px;
      background: #445;
      color: #eee;
      border: none;
      border-radius: 8px;
      font-size: 14px;
      font-weight: 600;
      box-shadow: 0 1px 3px #0002;
      cursor: pointer;
      transition: background .14s;
      display: inline-block;
    }
    .mainBtn:hover {
      background: #9ca3ad;
      color: #fff;
    }
    .mainBtn.danger {
      background: #e57373 !important;
      color: #fff;
    }
    .mainBtn.danger:hover {
      background: #f28b82 !important;
    }
    .side-filler {
      flex: 1 1 0;
      height: 1px;
      pointer-events: none;
    }

    /* === CARDS PRINCIPAIS DA LISTA === */
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
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
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

    /* === MODAL CONFIGURAÇÃO DE DISPOSITIVO === */
    #modal-bg {
      position: fixed;
      left: 0;
      top: 0;
      width: 100vw;
      height: 100vh;
      background: #0009;
      display: none;
      z-index: 10;
    }
    #modal-config {
      position: fixed;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: white;
      padding: 20px 24px 18px 24px;
      border-radius: 18px;
      display: none;
      z-index: 20;
      min-width: 100px;
      max-width: fit-content;
    }
    #modal-config label {
      font-size: 15px;
      font-weight: 600;
      margin-bottom: 7px;
      display: block;
    }
    #modal-config input[type="text"] {
      width: 100%;
      padding: 8px;
      font-size: 17px;
      border: 1px solid #c5cdd3;
      border-radius: 8px;
      margin-bottom: 14px;
      box-sizing: border-box;
    }
    .type-list {
      list-style: none;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      gap: 7px;
      margin-bottom: 18px;
    }
    .type-list li {
      display: flex;
      align-items: center;
      justify-content: space-between;
      border: 1.5px solid #d2dae6;
      border-radius: 11px;
      padding: 11px 16px;
      font-size: 17px;
      cursor: pointer;
      background: #f7fafd;
      transition: box-shadow .13s, border .13s, background .13s;
    }
    .type-list li.selected,
    .type-list li:active {
      border: 2px solid #39f;
      background: #e8f3fd;
    }
    .type-list li:hover:not(.selected) {
      box-shadow: 0 2px 12px #39f2;
      border: 1.5px solid #39f;
      background: #e8f3fd;
    }
    .type-list .type-ico {
      font-size: 1.23em;
      margin-left: 16px;
    }
    .type-list .type-led { color: #e1be11; }
    .type-list .type-rega { color: #21b4c5; }
    .type-list .type-runoff { color: #8b6e54; }
    .type-list .type-wavemaker { color: #27a162; }
    .modal-actions {
      display: flex;
      justify-content: flex-end;
      gap: 10px;
    }
    .modal-actions button {
      font-size: 16px;
      padding: 7px 22px;
      border-radius: 8px;
      border: none;
    }
    .modal-actions .cancel-btn {
      background: #eee;
      color: #555;
    }
    .modal-actions .save-btn {
      background: #39f;
      color: #fff;
      font-weight: 700;
    }
    .modal-actions .save-btn:disabled {
      background: #bbd2f3;
      color: #eee;
    }

    /* === MODAL AGENDAMENTOS (TELA DE HORÁRIOS) === */
    #modal-agendamentos {
      position: fixed;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: white;
      padding: 22px 20px 16px 20px;
      border-radius: 18px;
      display: none;
      z-index: 20;
      min-width: 330px;
      max-width: 99vw;
      /* Evitar sobreposição do botão */
      overflow: visible;
    }
    #modal-agendamentos h2 {
      margin: 0 0 13px 0;
      font-size: 1.12em;
      font-weight: 700;
      color: #222;
      letter-spacing: 0.5px;
    }
    #modal-agendamentos label {
      line-height: 25px;
      font-size: 13px;
      font-weight: 600;
      margin: 0;
      padding: 0;
      text-align: left;
      display: flex;
      align-items: center;
    }
    #modal-agendamentos .add-sched-form label[for="schedHOn"],
    #modal-agendamentos .add-sched-form label[for="schedHOff"] {
      font-size: 13px;
      width: 58px;
      min-width: 0;
      max-width: 98px;
      display: flex;
      align-items: center;
      position: relative;
      top: -4px;
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
      top: -4px;
    }
    #modal-agendamentos .add-sched-form select {
      font-size: 13px;
    }
    #modal-agendamentos input[type="text"],
    #modal-agendamentos input[type="number"] {
      width: 100%;
      padding: 3px;
      font-size: 15px;
      border: 1px solid #c5cdd3;
      border-radius: 5px;
      box-sizing: border-box;
    }
    .sched-modal-header {
      width: 100%;
      display: flex;
      justify-content: flex-end;
      align-items: center;
      margin-bottom: 0px;
    }
    #schedDay {
      min-width: 80px !important;
      width: 80px !important;
      height: 25px;
      font-weight: 600;
      font-size: 13px;
      padding: 0px 3px;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
      text-align: left;
      color: #444;
    }
    #schedDay option {
      text-align: left;
      color: #444;
    }
    .sched-list {
      align-items: flex-start;
      width: max-content;
      min-width: 0;
      list-style: none;
      padding-left: 2px;
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
      padding: 2px 5px 2px 6px;
      font-size: 0.97em;
      justify-content: flex-start;
      box-shadow: 0 1px 6px #bbb2;
    }
    .sched-list .sched-info {
      display: flex;
      gap: 17px;
      align-items: center;
    }
    .sched-list .sched-diasemana {
      font-size: 11px;
      font-weight: 500;
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
      padding: 1px 5px;
      border-radius: 6px;
      font-size: 0.95em;
      font-weight: 600;
      cursor: pointer;
      margin-left: 80px;
      transition: background .17s;
    }
    .sched-list button:hover {
      background: #c12;
    }
    .add-sched-form {
      display: flex;
      flex-direction: column;      
      align-items: flex-start;
      margin-top: 6px;
      margin-bottom: 9px;
      gap: 3px;
    }
    .add-sched-form > div {
      display: flex;
      align-items: center;
      gap: 7px;
      margin-bottom: 2px;
    }
    .add-sched-form select,
    .add-sched-form input[type="number"] {
      width: 2.7em !important;
      max-width: 3.2em;
      min-width: 2.7em;
      font-size: 1em;
      text-align: center;
      padding: 3px 0;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
      box-sizing: border-box;
      height: 25px;
    }
    .add-sched-form select {
      min-width: 78px;
      font-weight: 600;
      font-size: 1em;
      padding: 5px 7px;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
    }
    .schedBtns {
      transform: translateX(8px);
    }
    .addHrs {
      display: flex;
      flex-direction: column;
      gap: 8px;
      margin-left: auto;
      margin-right: auto;
      max-width: 340px;
      width: 100%;
      padding-top: 10px;
    }

    /* === MODAL APAGAR === */
    #modal-apagar {
      display: none;
      position: fixed;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: #fff;
      z-index: 100;
      padding: 28px 22px 16px 22px;
      border-radius: 18px;
      min-width: 250px;
      box-shadow: 0 8px 24px #0002;
    }
    #modal-bg-apagar {
      display: none;
      position: fixed;
      left: 0;
      top: 0;
      width: 100vw;
      height: 100vh;
      background: #0005;
      z-index: 99;
    }

    /* === MODAL WAVEMAKER === */
    #modal-wavemaker {
      position: fixed;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: white;
      padding: 22px 20px 16px 20px;
      border-radius: 18px;
      display: none;
      z-index: 21;
      min-width: 330px;
      max-width: 98vw;
    }

    /* === MODAL SETTINGS (OPÇÕES) === */
    #modal-settings {
      display: none;
      position: fixed;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: white;
      z-index: 40;
      min-width: 320px;
      max-width: 95vw;
      padding: 22px 20px 20px 20px;
      border-radius: 16px;
    }
    #modal-bg-settings {
      display: none;
      position: fixed;
      left: 0;
      top: 0;
      width: 100vw;
      height: 100vh;
      background: #0008;
      z-index: 39;
    }

    /* === ELEMENTOS ESPECIAIS === */
    .manual-label {
      display: inline-block;
      background: #ffe1e8;
      color: #d31b42;
      font-weight: 700;
      font-size: 0.55em;
      border-radius: 4px;
      padding: 1px 7px 1px 7px;
      cursor: pointer;
      border: 0.8px solid #f6a3b2;
      letter-spacing: 0.7px;
      margin-right: 2.5px;
      margin-left: 0;
      transition: background 0.18s;
      box-shadow: 0 0.4px 0.9px #f6a3b266;
    }
    .manual-label:hover {
      background: #ffd2dc;
    }
    /* === MODAL FLUSH === */
    #flushBtn {
      background: #445;
      color: #fff;
      border: none;
      border-radius: 7px;
      font-size: 11px;
      font-weight: 600;
      padding: 3px 10px;
      box-shadow: 0 2px 10px #26b3c41a;
      cursor: pointer;
      z-index: 22;
      transition: background .14s;
      margin-right: 7px;
      margin-top: 0;
      /* Remova position absolute e top/right! */
      position: static;
    }
    #flushBtn:hover {
      background: #9ca3ad;
    }

    #modal-flush input[type="number"]::-webkit-inner-spin-button,
    #modal-flush input[type="number"]::-webkit-outer-spin-button {
      -webkit-appearance: none;
      margin: 0;
    }
    #modal-flush input[type="number"] {
      border: 1px solid #ccd6e1;
      border-radius: 5px;
      font-size: 15px;
      padding: 3px 2px;
      box-sizing: border-box;
    }
    #modal-flush select {
      font-size: 14px;
      font-weight: 600;
      border: 1px solid #ccd6e1;
      border-radius: 7px;
      padding: 2px 7px;
    }
    #modal-flush label[for="flushExecAgora"],
    #modal-flush label[for="flushExecAgendar"] {
      font-size: 14px;
    }


  </style>
</head>
<body>

  <!-- ======================= GERAL ======================= -->
  <div class="clock-bar">
    <div id="clock"></div>
    <button id="settingsBtn" title="Opções" class="settings-btn">⚙️</button>
  </div>
  <div class="cards" id="cards"></div>
  <div class="main-buttons">
    <button id="debugBtn" class="mainBtn" style="display:none;">Debug</button>
  </div>

  <!-- ================== MODAL CONFIGURAÇÃO ================== -->
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

  <!-- ================== MODAL AGENDAMENTOS ================== -->
  <div id="modal-agendamentos">
    <div class="sched-modal-header">
      <button id="flushBtn" style="display:none;">Flush</button>
    </div>
    <ul class="sched-list" id="schedList"></ul>
    <form class="add-sched-form" id="addSchedForm" autocomplete="off">
      <div style="display:flex; align-items:center; text-align:center; gap:4px; margin-top:10px; padding-left:69px;">
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
      <div class="addHrs">
        <div style="display:flex;">
          <label for="schedHOn" style="min-width:58px;">Liga:</label>
          <input type="number" class="hr-input" id="schedHOn" min="0" max="23" placeholder="H" maxlength="2" required style="width:2.2em;text-align:center;">
          <span>:</span>
          <input type="number" class="hr-input" id="schedMOn" min="0" max="59" placeholder="M" maxlength="2" required style="width:2.2em;text-align:center;">
          <span>:</span>
          <input type="number" class="hr-input" id="schedSOn" min="0" max="59" placeholder="S" maxlength="2" required style="width:2.2em;text-align:center;">
        </div>
        <div style="display:flex;">
          <label for="schedHOff" style="min-width:58px;">Desliga:</label>
          <input type="number" class="hr-input" id="schedHOff" min="0" max="23" placeholder="H" maxlength="2" required style="width:2.2em;text-align:center;">
          <span>:</span>
          <input type="number" class="hr-input" id="schedMOff" min="0" max="59" placeholder="M" maxlength="2" required style="width:2.2em;text-align:center;">
          <span>:</span>
          <input type="number" class="hr-input" id="schedSOff" min="0" max="59" placeholder="S" maxlength="2" required style="width:2.2em;text-align:center;">
        </div>
      </div>
      <div class="schedBtns" style="display:flex; gap:12px; margin-top:10px;">
        <button type="submit" style="background:#39f;color:#fff;" class="mainBtn">Adicionar</button>
        <button type="button" class="mainBtn" onclick="closeAgendamentoModal()">Fechar</button>
        <button type="button" class="mainBtn danger" id="apagarBtn">Apagar</button>
      </div>
    </form>
  </div>

  <!-- ================== MODAL APAGAR ================== -->
  <div id="modal-apagar" style="display:none;position:fixed;left:50%;top:50%;transform:translate(-50%,-50%);
  background:#fff;z-index:100;padding:28px 22px 16px 22px;border-radius:18px;min-width:250px;box-shadow:0 8px 24px #0002;">
    <div style="font-weight:600;font-size:1.09em;color:#c00;margin-bottom:16px;text-align:center;">
      O que você deseja apagar?<br>
      <span style="font-weight:400;color:#555;font-size:0.95em;">(Esta ação é irreversível!)</span>
    </div>
    <div style="display:flex;flex-direction:column;gap:13px;">
      <button class="mainBtn danger" onclick="apagarAgendamentos()">Apagar Agendamentos</button>
      <button class="mainBtn danger" onclick="apagarDispositivo()">Apagar Dispositivo</button>
      <button class="mainBtn" onclick="closeApagarModal()">Cancelar</button>
    </div>
  </div>
  <div id="modal-bg-apagar" style="display:none;position:fixed;left:0;top:0;width:100vw;height:100vh;background:#0005;z-index:99;"></div>

  <!-- ================== MODAL WAVEMAKER ================== -->
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
    <div class="schedBtns" style="display:flex; justify-content:center; gap:12px; margin-top:2px; padding-top:20px; padding-right:11px;">      
      <button type="button" class="mainBtn" onclick="closeWavemakerModal()">Fechar</button>
      <button type="button" class="mainBtn danger" onclick="openConfirmApagarModal()" id="apagarBtn">Apagar</button>
    </div>
  </div>
  <div id="modal-bg-wavemaker" style="display:none;position:fixed;left:0;top:0;width:100vw;height:100vh;background:#0009;z-index:10;"></div>

  <!-- Modal de confirmação para apagar dispositivo wavemaker -->
  <div id="modal-confirm-apagar" style="display:none; position: fixed; left: 50%; top: 50%; 
    transform: translate(-50%, -50%);
    background: #fff;
    padding: 24px 28px;
    border-radius: 16px;
    box-shadow: 0 8px 24px #0009;
    z-index: 50;
    min-width: 280px;
    text-align: center;
  ">
    <p style="font-weight: 600; font-size: 1.1em; margin-bottom: 16px;">
      Tem certeza que deseja apagar este dispositivo?<br>
      <small style="font-weight: 400; font-size: 0.9em; color: #555;">
        Esta ação é irreversível!
      </small>
    </p>
    <div style="display: flex; justify-content: center; gap: 12px;">
      <button type="button" class="mainBtn" onclick="closeConfirmApagarModal()">Cancelar</button>
      <button type="button" class="mainBtn danger" onclick="apagarDispositivoWavemaker()">Confirmar</button>
    </div>
  </div>
  <div id="modal-bg-confirm-apagar" style="display:none; position: fixed; left: 0; top: 0; 
    width: 100vw; height: 100vh; background: #0009; z-index: 49;">
  </div>
  <!-- ================== MODAL FLUSH (REGA) ================== -->
  <div id="modal-flush" style="display:none; position:fixed; left:50%; top:50%; transform:translate(-50%,-50%); background:#fff; z-index:120; min-width:320px; max-width:96vw; padding:28px 22px 18px 22px; border-radius:18px; box-shadow:0 8px 24px #0003;">
    <form id="flushForm" autocomplete="off">
      <div style="display:flex; flex-wrap:wrap; align-items:center; gap:7px; margin-bottom:13px; font-size: 11px;">
        <span>Flush de</span>
        <input type="number" id="flushQtd" min="1" max="99" maxlength="2" required style="width:2.4em; text-align:center;">
        <span>regas de</span>
        <input type="number" id="flushDur" min="1" max="99" maxlength="2" required style="width:2.4em; text-align:center;">
        <span>minutos, intervalos de</span>
        <input type="number" id="flushInt" min="1" max="99" maxlength="2" required style="width:2.4em; text-align:center;">
        <span>minutos</span>
      </div>
      <div style="margin-bottom:8px; display:flex; flex-direction:column; gap:4px;">
        <label style="display:flex; align-items:center; gap:7px; font-size:14px;">
          <input type="radio" name="flushExec" value="agora" id="flushExecAgora" checked>
          <span>Agora</span>
        </label>
        <label style="display:flex; align-items:center; gap:7px; font-size:14px;">
          <input type="radio" name="flushExec" value="agendar" id="flushExecAgendar">
          <span>Dia:</span>
          <select id="flushDia" style="min-width:72px;">
            <option value="1">Dom</option>
            <option value="2">Seg</option>
            <option value="3">Ter</option>
            <option value="4">Qua</option>
            <option value="5">Qui</option>
            <option value="6">Sex</option>
            <option value="7">Sáb</option>
          </select>
          <span>às</span>
          <input type="number" id="flushHora" min="0" max="23" maxlength="2" required style="width:2.2em; text-align:center;">
          <input type="number" id="flushMin" min="0" max="59" maxlength="2" required style="width:2.2em; text-align:center;">
          <input type="number" id="flushSec" min="0" max="59" maxlength="2" required style="width:2.2em; text-align:center;">
        </label>
      </div>
      <div style="display:flex; gap:12px; margin-top:18px; justify-content:center;">
        <button type="submit" class="mainBtn" style="background:#21b4c5;">Confirmar</button>
        <button type="button" class="mainBtn" onclick="closeFlushModal()">Fechar</button>
      </div>
    </form>
  </div>

  <div id="modal-bg-flush" style="display:none; position:fixed; left:0; top:0; width:100vw; height:100vh; background:#0007; z-index:119;"></div>

  <!-- ================== MODAL SETTINGS/OPÇÕES ================== -->
  <div id="modal-settings" style="display:none; position:fixed; left:50%; top:50%; transform:translate(-50%,-50%); background:white; z-index:40; min-width:320px; max-width:95vw; padding:22px 20px 20px 20px; border-radius:16px;">
    <h2 style="margin-top:0;font-size:1.18em;">Opções</h2>
    <button id="exportBtn" style="margin-bottom:8px;width:100%;" class="mainBtn">Salvar backup</button>
    <input type="file" id="importFile" accept=".json,application/json" style="display:none;">
    <button id="importBtn" style="margin-bottom:8px;width:100%;" class="mainBtn">Restaurar backup</button>
    <button id="resetBtn" style="margin-bottom:8px;width:100%;" class="mainBtn">Apagar tudo</button>
    <div style="display:flex;justify-content:flex-end;gap:10px;">
      <button class="mainBtn" onclick="closeSettingsModal()">Fechar</button>
    </div>
  </div>
  <div id="modal-bg-settings" style="display:none;position:fixed;left:0;top:0;width:100vw;height:100vh;background:#0008;z-index:39;"></div>

  <!-- ================== JS ORGANIZADO EM SEÇÕES ================== -->


  <script>
    // ====================== GERAL E UTILITÁRIOS ======================
    const DEBUG_MODE = )rawliteral";
    page += (debug ? "true" : "false");
    page += R"rawliteral(;

    let relayData = Array.from({length: 8}, (_,i)=>({ name: "", type: "", state: false, num_sched: 0, manual: false }));
    let ntpTime = null;

    // Ícone de cada tipo de dispositivo
    function getIcon(type) {
      if (type === "LED" || type === "Led") return "💡";
      if (type === "Rega") return "💧";
      if (type === "Ventilador") return "🌀";
      if (type === "Wavemaker") return "💨";
      if (type === "Runoff") return "🧺";
      return "🔌";
    }

    // Atualiza status da linha do card (próximo evento, etc)
    function getStatusLinha(device) {
      if (typeof ntpTime === "undefined" || !ntpTime) return "Sincronizando horário...";
      if (device.type === "Wavemaker") {
        const modos = [
          "Sempre ligado","15min Ligado/15min Desligado","30min Ligado/30min Desligado",
          "1h Ligado/1h Desligado","6h Ligado/6h Desligado","12h Ligado/12h Desligado","10s Ligado/10s Desligado"
        ];
        let idx = typeof device.wavemaker_mode === "number" ? device.wavemaker_mode : parseInt(device.wavemaker_mode);
        return modos[idx] || "Modo indefinido";
      }
      if (!device.has_schedule || !device.horarios || device.horarios.length === 0) return "Sem agendamentos";

      const diasSemana = ["Dom","Seg","Ter","Qua","Qui","Sex","Sáb"];
      const diaAtual = ntpTime.weekday;
      const segundosAtuais = ntpTime.hour * 3600 + ntpTime.minute * 60 + ntpTime.second;
      let proximoEvento = null, menorDiferenca = 8 * 86400;
      function horaParaSegundos(horaStr) {
        const partes = (horaStr || "00:00:00").split(":").map(Number);
        return (partes[0]||0)*3600 + (partes[1]||0)*60 + (partes[2]||0);
      }
      device.horarios.forEach(ev => {
        let diasParaTestar = [];
        if (ev.dia === "Todos") diasParaTestar = [0,1,2,3,4,5,6];
        else if (typeof ev.dia === "number") diasParaTestar = [ev.dia];
        else if (!isNaN(Number(ev.dia))) diasParaTestar = [Number(ev.dia)];
        else {
          let idx = diasSemana.indexOf(ev.dia);
          if (idx >= 0) diasParaTestar = [idx];
        }
        const temHOnHOff = ev.h_on && ev.h_off;
        const hOnSeg = horaParaSegundos(ev.h_on);
        const hOffSeg = horaParaSegundos(ev.h_off);
        const unicaHora = horaParaSegundos(ev.hora);
        diasParaTestar.forEach(idxDiaEv => {
          let diasAteEvento = idxDiaEv - diaAtual;
          if (diasAteEvento < 0) diasAteEvento += 7;
          const baseSegundos = diasAteEvento * 86400;
          if (temHOnHOff) {
            if (diasAteEvento === 0 && segundosAtuais < hOnSeg) {
              const dif = baseSegundos + (hOnSeg - segundosAtuais);
              if (dif < menorDiferenca) {
                menorDiferenca = dif;
                proximoEvento = { acao: "liga", hora: ev.h_on, dia: idxDiaEv };
              }
            } else if (diasAteEvento === 0 && segundosAtuais >= hOnSeg && segundosAtuais < hOffSeg) {
              const dif = baseSegundos + (hOffSeg - segundosAtuais);
              if (dif < menorDiferenca) {
                menorDiferenca = dif;
                proximoEvento = { acao: "desliga", hora: ev.h_off, dia: idxDiaEv };
              }
            } else if (diasAteEvento !== 0) {
              const dif = baseSegundos + (hOnSeg - segundosAtuais);
              if (dif < menorDiferenca) {
                menorDiferenca = dif;
                proximoEvento = { acao: "liga", hora: ev.h_on, dia: idxDiaEv };
              }
            }
          } else {
            const evSeg = unicaHora;
            let eventoNoPassado = diasAteEvento === 0 && evSeg <= segundosAtuais;
            let ajuste = eventoNoPassado ? 7 * 86400 : 0;
            const difSeg = baseSegundos + (evSeg - segundosAtuais) + ajuste;
            if (difSeg < menorDiferenca) {
              menorDiferenca = difSeg;
              proximoEvento = {
                acao: ev.acao || "liga",
                hora: ev.hora || "00:00:00",
                dia: idxDiaEv
              };
            }
          }
        });
      });
      if (proximoEvento) {
        let mostrarDia = diasSemana[proximoEvento.dia];
        let acao = proximoEvento.acao === "liga" ? "Liga" : "Desliga";
        return `${acao} ${mostrarDia} ${proximoEvento.hora}`;
      }
      return "Nenhum evento futuro encontrado.";
    }

    // Atualização de horário do relógio
    function fetchClock(cb) {
      fetch("/clock").then(r => r.text()).then(txt => {
        let t = txt.replace(/^Hora atual:\s*/, '');
        let dia, hora;
        if (/^[A-Z]{3} \d\d:\d\d:\d\d$/.test(t)) {
          [dia, hora] = t.split(" ");
        } else if (/^\d\d:\d\d:\d\d$/.test(t)) {
          hora = t;
          const dias = ["DOM","SEG","TER","QUA","QUI","SEX","SAB"];
          dia = dias[(new Date()).getDay()];
        } else {
          document.getElementById("clock").textContent = t;
          ntpTime = null;
          if (cb) cb();
          return;
        }
        const diasIdx = { "DOM":0, "SEG":1, "TER":2, "QUA":3, "QUI":4, "SEX":5, "SAB":6 };
        let weekday = diasIdx[dia.toUpperCase()] ?? 0;
        let [h, m, s] = hora.split(":").map(Number);
        ntpTime = { weekday, hour: h, minute: m, second: s };
        document.getElementById("clock").textContent = t;
        if (cb) cb();
        updateStatusLinhas(); // também atualiza os cards sempre que o clock muda
      });
    }


    // =================== TELA PRINCIPAL (CARDS) ===================
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
                </div>
                <span class="label-tipo">${d.type}</span>
                <div class="status-linha">${getStatusLinha(d)}</div>
              </div>
              <div class="switch-num-wrap">
                ${d.manual ? `<span class="manual-label" onclick="desativarManual(${i}, event)">MANUAL</span>` : ""}
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

    // Setup dos listeners dos cards
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
          const tipo = relayData[idx]?.type || "";
          if (tipo === "Wavemaker") openWavemakerModal(idx);
          else openAgendamentoModal(idx);
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

    // Atualiza só o status-linha e o botão ON/OFF de cada card (usado no polling a cada 1s)
    function updateStatusLinhas() {
      const cardDivs = document.querySelectorAll('.card:not(.empty)');
      cardDivs.forEach((card, i) => {
        const idx = parseInt(card.getAttribute('data-idx'));
        const device = relayData[idx];
        if (!device) return;
        const statusDiv = card.querySelector('.status-linha');
        if (statusDiv) statusDiv.textContent = getStatusLinha(device);
        const checkbox = card.querySelector('input[type="checkbox"]');
        if (checkbox) checkbox.checked = !!device.state;
      });
    }

    // ================== MODAL CONFIGURAÇÃO DE DISPOSITIVO ==================
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

    // ================== MODAL AGENDAMENTOS (SCHEDULE) ==================
    let agendamentoIdx = null;
    function openAgendamentoModal(idx) {
      agendamentoIdx = idx;
      document.getElementById('modal-bg').style.display = 'block';
      document.getElementById('modal-agendamentos').style.display = 'block';
      document.getElementById('addSchedForm').reset();
      fetchAgendamentos(idx);
      // Para LED: esconde o campo de dia da semana
      const tipo = relayData[idx]?.type || "";
      // Botao Flush
      const flushBtn = document.getElementById('flushBtn');
      const header = flushBtn.parentElement;
      if (tipo === "Rega") {
        flushBtn.style.display = "";
        header.style.marginBottom = "10px";  // Espaço só quando tem botão
        header.style.minHeight = "16px";    // Ou o valor exato do botão
      } else {
        flushBtn.style.display = "none";
        header.style.marginBottom = "0";
        header.style.minHeight = "0";
      }

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
    function pad2(n) { return n.toString().padStart(2, "0"); }
    function delAgendamento(evIdx) {
      const idx = agendamentoIdx;
      fetch(`/delsched?rele=${idx}&idx=${evIdx}`, {method:"POST"})
        .then(()=>{ fetchAgendamentos(idx); });
    }
    document.querySelectorAll('.hr-input').forEach(function(input) {
      input.addEventListener('input', function() {
        this.value = this.value.replace(/[^\d]/g, '');
        if (this.value.length > 2) {
          this.value = this.value.slice(0, 2);
        }
      });
    });
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
          fetchRelays();
          document.getElementById('addSchedForm').reset();
        });
    };

    // ================= MODAL FLUSH ===================
    document.getElementById('flushBtn').onclick = function() {
      document.getElementById('modal-bg-flush').style.display = 'block';
      document.getElementById('modal-flush').style.display = 'block';
    };
    function closeFlushModal() {
      document.getElementById('modal-bg-flush').style.display = 'none';
      document.getElementById('modal-flush').style.display = 'none';
    }
    document.getElementById('modal-bg-flush').onclick = closeFlushModal;

    document.querySelectorAll('#modal-flush input[type="number"]').forEach(function(input) {
      input.addEventListener('input', function() {
        this.value = this.value.replace(/[^\d]/g, '');
        if (this.value.length > 2) {
          this.value = this.value.slice(0, 2);
        }
      });
    });

    // Desabilita campos de dia/hora quando está "Agora" selecionado:
    document.getElementById('flushExecAgora').onchange = function() {
      document.getElementById('flushDia').disabled = true;
      document.getElementById('flushHora').disabled = true;
      document.getElementById('flushMin').disabled = true;
      document.getElementById('flushSec').disabled = true;
    };
    document.getElementById('flushExecAgendar').onchange = function() {
      document.getElementById('flushDia').disabled = false;
      document.getElementById('flushHora').disabled = false;
      document.getElementById('flushMin').disabled = false;
      document.getElementById('flushSec').disabled = false;
    };
    // Inicia com campos desabilitados (modo "Agora" default)
    document.getElementById('flushDia').disabled = true;
    document.getElementById('flushHora').disabled = true;
    document.getElementById('flushMin').disabled = true;
    document.getElementById('flushSec').disabled = true;

    function agendarFlush(idx) {
      // Coletar os valores do modal:
      const qtd = parseInt(document.getElementById('flushQtd').value, 10);
      const dur = parseInt(document.getElementById('flushDur').value, 10);  // minutos
      const intervalo = parseInt(document.getElementById('flushInt').value, 10);  // minutos

      let startTime;
      let dia;
      if (document.getElementById('flushExecAgora').checked) {
        const now = new Date();
        startTime = now;
        dia = now.getDay() + 1; // Domingo = 1 ... Sábado = 7 (ajuste para seu backend)
      } else {
        dia = parseInt(document.getElementById('flushDia').value, 10);
        let h = parseInt(document.getElementById('flushHora').value, 10);
        let m = parseInt(document.getElementById('flushMin').value, 10);
        let s = parseInt(document.getElementById('flushSec').value, 10);
        startTime = new Date();
        startTime.setHours(h, m, s, 0);
        // Ajustar dia para o selecionado (se precisar avançar até o dia correto)
      }

      let eventos = [];
      let currTime = new Date(startTime);
      for (let i = 0; i < qtd; i++) {
        let liga = new Date(currTime);
        let desliga = new Date(liga);
        desliga.setMinutes(desliga.getMinutes() + dur);

        eventos.push({
          dia: dia,
          h_on: liga.getHours(),
          m_on: liga.getMinutes(),
          s_on: liga.getSeconds(),
          h_off: desliga.getHours(),
          m_off: desliga.getMinutes(),
          s_off: desliga.getSeconds(),
          isFlush: 1
        });

        // Próxima rega começa após a anterior acabar + intervalo
        currTime = new Date(desliga);
        currTime.setMinutes(currTime.getMinutes() + intervalo);
      }

      // Fazer fetch para cada evento
      let promessas = [];
      eventos.forEach(ev => {
        let params = `rele=${idx}&dia=${ev.dia}&h_on=${ev.h_on}&m_on=${ev.m_on}&s_on=${ev.s_on}` +
                    `&h_off=${ev.h_off}&m_off=${ev.m_off}&s_off=${ev.s_off}&isFlush=1`;
        promessas.push(fetch(`/addsched?${params}`));
      });

      // Fechar modal e atualizar interface depois de tudo
      Promise.all(promessas).then(() => {
        closeFlushModal();
        fetchAgendamentos(idx);
        fetchRelays();
      });
    }
    document.getElementById('flushForm').onsubmit = function(ev) {
      ev.preventDefault();
      agendarFlush(agendamentoIdx); // Use o índice do relé/dispositivo sendo agendado
    };



    // ================== MODAL APAGAR ==================
    document.getElementById("apagarBtn").onclick = function() {
      document.getElementById("modal-apagar").style.display = "block";
      document.getElementById("modal-bg-apagar").style.display = "block";
    };
    function closeApagarModal() {
      document.getElementById("modal-apagar").style.display = "none";
      document.getElementById("modal-bg-apagar").style.display = "none";
    }
    function apagarAgendamentos() {
      if (typeof agendamentoIdx !== "number") return;
      fetch("/reset_schedules?rele=" + agendamentoIdx, {method:"POST"})
        .then(()=>{ closeApagarModal(); fetchAgendamentos(agendamentoIdx); fetchRelays(); });
    }
    function apagarDispositivo() {
      if (typeof agendamentoIdx !== "number") return;
      fetch("/reset_device?rele=" + agendamentoIdx, {method:"POST"})
        .then(()=>{ closeApagarModal(); closeAgendamentoModal(); fetchRelays(); });
    }

    // ================== MODAL WAVEMAKER ==================
    let wavemakerIdx = null;
    function openWavemakerModal(idx) {
      wavemakerIdx = idx;
      document.getElementById('modal-bg-wavemaker').style.display = 'block';
      document.getElementById('modal-wavemaker').style.display = 'block';
      if (DEBUG_MODE)
        document.querySelector('#wavemakerOptionList li[data-opt="debug"]').style.display = '';
      else
        document.querySelector('#wavemakerOptionList li[data-opt="debug"]').style.display = 'none';
    }
    function closeWavemakerModal() {
      document.getElementById('modal-bg-wavemaker').style.display = 'none';
      document.getElementById('modal-wavemaker').style.display = 'none';
      wavemakerIdx = null;
    }
    document.querySelectorAll('#wavemakerOptionList li').forEach(li => {
      li.onclick = function() {
        let opt = li.getAttribute('data-opt');
        let mode = (opt === "debug") ? 6 : parseInt(opt);
        selectWavemakerOption(wavemakerIdx, mode);
      }
    });
    document.getElementById('modal-bg-wavemaker').onclick = closeWavemakerModal;
    function selectWavemakerOption(idx, mode) {
      fetch(`/setwavemakermode?rele=${idx}&mode=${mode}`)
        .then(() => {
          closeWavemakerModal();
          fetchRelays();
        });
    }

    // ======= CONFIRMAÇAO DELETE WAVEMAKER ======= //
    function openConfirmApagarModal() {
      document.getElementById('modal-confirm-apagar').style.display = 'block';
      document.getElementById('modal-bg-confirm-apagar').style.display = 'block';
    }
    function closeConfirmApagarModal() {
      document.getElementById('modal-confirm-apagar').style.display = 'none';
      document.getElementById('modal-bg-confirm-apagar').style.display = 'none';
    }
    function apagarDispositivoWavemaker() {      
      if (typeof wavemakerIdx !== "number") return;
      fetch("/reset_device?rele=" + wavemakerIdx, {method: "POST"})
        .then(() => {
          closeWavemakerModal();
          closeConfirmApagarModal(); 
          fetchRelays();         
        });
    }
    document.getElementById('modal-bg-confirm-apagar').onclick = closeConfirmApagarModal;


    // ================== MODAL OPÇÕES / BACKUP/RESTORE ==================
    document.getElementById('settingsBtn').onclick = function() {
      document.getElementById('modal-settings').style.display = 'block';
      document.getElementById('modal-bg-settings').style.display = 'block';
    };
    function closeSettingsModal() {
      document.getElementById('modal-settings').style.display = 'none';
      document.getElementById('modal-bg-settings').style.display = 'none';
    }
    document.getElementById('modal-bg-settings').onclick = closeSettingsModal;
    document.getElementById('importBtn').onclick = function() {
      document.getElementById('importFile').click();
    };
    document.getElementById('importFile').onchange = function(evt) {
      const file = evt.target.files[0];
      if (!file) return;
      const reader = new FileReader();
      reader.onload = function(e) {
        fetch('/import_all', {
          method: 'POST',
          headers: {'Content-Type':'application/json'},
          body: e.target.result
        })
        .then(r => r.text())
        .then(txt => {
          alert(txt);
          fetchRelays();
        })
        .catch(err => alert("Erro ao importar backup: " + err));
      };
      reader.readAsText(file);
    };
    document.getElementById('exportBtn').onclick = function() {
      fetch('/export_all')
        .then(resp => {
          if (!resp.ok) throw new Error("Falha ao exportar backup!");
          return resp.blob();
        })
        .then(blob => {
          const url = window.URL.createObjectURL(blob);
          const a = document.createElement('a');
          a.href = url;
          a.download = "grow32.json";
          document.body.appendChild(a);
          a.click();
          setTimeout(() => {
            window.URL.revokeObjectURL(url);
            a.remove();
          }, 200);
        })
        .catch(err => alert("Erro ao exportar: " + err));
    };
    document.getElementById('resetBtn').onclick = function() {
      if (confirm('Tem certeza que deseja zerar tudo? Isso apagará todas as configurações e agendamentos.')) {
        fetch('/reset_all', {method: "POST"})
          .then(r => r.text())
          .then(txt => {
            alert(txt);
            fetchRelays();
          })
          .catch(err => alert("Erro ao zerar: " + err));
      }
    };

    // ================== FUNÇÕES GERAIS ==================
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
    function desativarManual(idx, ev) {
      ev.stopPropagation();
      fetch(`/setauto?rele=${idx}`)
        .then(() => setTimeout(fetchRelays, 200));
    }

    // =============== INICIALIZAÇÃO ===============
    if (typeof DEBUG_MODE !== 'undefined' && DEBUG_MODE) {
      document.getElementById("debugBtn").style.display = "block";
      document.getElementById("debugBtn").onclick = function() {
        fetch("/debugsched").then(r => {
          if (r.ok) fetchRelays();
        });
      };
    } else {
      document.getElementById("debugBtn").style.display = "none";
    }
    // Primeira vez: primeiro o clock, depois os relés!
    fetchClock(function() {
      fetchRelays();
    });

    setInterval(fetchClock, 1000);

    setInterval(() => {
      fetch("/relaydata")
        .then(r => r.json())
        .then(js => {
          relayData = js;
          updateStatusLinhas();
        });
    }, 1000);


  </script>
</body>
</html>
  )rawliteral";
  return page;
}
