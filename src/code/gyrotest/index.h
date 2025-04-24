
#include "Arduino.h"
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>実験機</title>
  <meta http-equiv="content-type" charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
    }

    h1 {
      margin-top: 10px;
      margin-bottom: 10px;
      font-size: 1em;
      color: white;
      text-align: center;
    }


    h2 {
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
      text-align: center;
    }

    .topnav {
      overflow: hidden;
      background-color: #143642;
      text-align: center;
    }

    body {
      margin: 0;
    }

    .content {
      margin: 0 auto;
      padding-left: 0;
      padding-right: 0;
      display: flex;
      flex-wrap: wrap;
      flex-direction: row;
      justify-content: space-evenly;
    }

    .card {
      /* height: 500px; */
      width: 100%;
      text-align: center;
      background-color: #F8F7F9;
      ;
      box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5);
      max-width: 610px;
      display: flex;
      flex-direction: column;
    }

    .button {
      padding: 15px 50px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
    }

    /*.button:hover {background-color: #0f8b8d}*/
    .button:active {
      background-color: #0f8b8d;
      box-shadow: 2 2px #CDCDCD;
      transform: translateY(2px);
    }

    .state {
      font-size: 1.5rem;
      color: #8c8c8c;
      font-weight: bold;
    }

    input[type='checkbox'] {
      position: absolute;
      width: 0;
      height: 0;
    }

    .label {
      display: flex;
      justify-content: center;
    }

    .label1 {
      display: flex;
      justify-content: space-between;
    }

    .base {
      width: 56px;
      border-radius: 16px;
      height: 32px;
      background-color: #ddd;
      transition: 0.5s;
    }

    input:checked~.base {
      background-color: rgb(219, 234, 254);
    }

    input:checked~.topnav {
      background-color: crimson;
    }

    .circle {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 24px;
      height: 24px;
      border-radius: 12px;
      background-color: gray;
      transition: 0.5s;
    }

    input:checked~.circle {
      transform: translateX(100%);
      background-color: blue;
    }

    .box {
      width: 200px;
      height: 200px;
      border: 1px #000 solid;
      overflow-y: scroll;
    }

    .switch {
      position: relative;
    }

    .title {
      margin-left: 4px;
    }

    .serial {
      width: 100%;
      max-width: 600px;
      box-shadow: 2px 2px 12px 1px rgba(0, 0, 0, 0.5);
      height: 500px;
      display: flex;
      flex-direction: column;
    }

    .s-to {
      background-color: rgb(17, 201, 243);
      height: 24px;
      width: 100%;

    }

    .s-top {
      height: 24px;
      text-align: center;
      margin-top: 0;
      margin-bottom: 0;
      font-size: x-large;
    }

    .serial-s {
      height: 24px;
      width: 100px;
      text-align: center;
      background-color: white;
    }

    input:checked~.serial-s {
      background-color: #000;
      color: white;
    }

    .serial-st {
      position: none;
    }

    .s-monitor {
      height: 100%;
      text-align: left;
      background-color: rgb(255, 255, 255);
      overflow-y: scroll;
    }

    .s-send {
      height: 21px;
      width: 100%;
      background-color: #CDCDCD;
      display: flex;
      flex-direction: row;
      justify-content: space-between;
    }

    .s-sende {
      height: 15px;
      width: 100%;
      max-width: 550px;
      background-color: #CDCDCD;
      border-color: rgb(0, 0, 0);
    }

    .s-but {
      position: relative;
      height: 0;
      border: 0;
      margin-top: 0;
      margin-bottom: 0;
    }

    .btn-border {
      border: 1px solid #000;
      border-radius: 0;
      background: #fff;
    }

    .btn-border:hover {
      color: #fff;
      background: #000;
    }

    *,
    *:before,
    *:after {
      -webkit-box-sizing: inherit;
      box-sizing: inherit;
    }

    .btn,
    .btn,
    button.btn {
      padding: 0;
      cursor: pointer;
      -webkit-transition: all 0.3s;
      transition: all 0.3s;
      text-align: center;
      vertical-align: middle;
      text-decoration: none;
      letter-spacing: 0.1em;
      color: #212529;
      border-radius: 0.5rem;
    }

    .field {
      width: 60px;
    }

    /* Slider *
 * ====== */
    /* ## Slider main element */
    input[type=text] {
      width: 30px;
    }

    input[type=button] {
      visibility: hidden;
    }

    input[type=range] {
      display: block;
      margin: 0;
      padding: 0;
      font-size: inherit;
      width: 9.9em;
      height: 1em;
      border-radius: 0.25em;
      border: 0.2em solid #242424;
      background-color: #242424;
      background-size: 100% 100%;
      background-repeat: no-repeat;
      overflow: hidden;
      cursor: pointer;
      transition: box-shadow 0.2s linear;
      box-shadow: 0 0 0 0 transparent;
    }

    input[type=range]:focus {
      box-shadow: 0 0 0 0.1em #AAAAAA;
    }

    input[type=range]:hover {
      box-shadow: 0 0 0 0.15em #6FC5F0;
    }

    /* ## Orientation tweak, with [orient] attribute in degrees */
    /* > I know it's not standard, but that's the cleaner way I was thinking of. */
    input[type=range][orient] {
      /* > Would be cool heh :-) : */
      transform: rotate(attr(orient));
    }

    input[type=range][orient="90deg"] {
      transform: rotate(90deg);
    }

    input[type=range][orient="180deg"] {
      transform: rotate(180deg);
    }

    input[type=range][orient="270deg"] {
      transform: rotate(270deg);
    }

    /* ### all vertical ranges : */
    input[type=range][orient="90deg"],
    input[type=range][orient="270deg"] {
      margin: 4.5em -4.5em;
      display: inline-block;
    }

    input[type=range][orient="90deg"]::-webkit-slider-thumb,
    input[type=range][orient="270deg"]::-webkit-slider-thumb {
      cursor: ns-resize;
    }

    input[type=range][orient="90deg"]::-moz-range-thumb,
    input[type=range][orient="270deg"]::-moz-range-thumb {
      cursor: ns-resize;
    }

    input[type=range][orient="90deg"]::-ms-thumb,
    input[type=range][orient="270deg"]::-ms-thumb {
      cursor: ns-resize;
    }

    /* ## Slider components */
    /* ### Slider::-track */
    input[type=range]::-webkit-slider-runnable-track {
      border: none;
      background: none;
      height: 100%;
      width: 100%;
    }

    input[type=range]::-moz-range-track {
      border: none;
      background: none;
      height: 100%;
      width: 100%;
    }

    input[type=range]::-ms-track {
      border: none;
      background: none;
      height: 100%;
      width: 100%;
    }

    /* ### Slider::-thumb */
    input[type=range]::-webkit-slider-thumb {
      margin: 0.05em;
      padding: 0;
      height: 0.9em;
      width: 0.9em;
      border-radius: 0.1em;
      box-sizing: border-box;
      border: none;
      background-color: #6FC5F0;
      box-shadow: -10em 0 0 0 #313131, -9em 0 0 0 #313131, -8em 0 0 0 #2F343F, -7em 0 0 0 #283F6B, -6em 0 0 0 #214893, -5em 0 0 0 #1A52BC, -4em 0 0 0 #2769D3, -3em 0 0 0 #3E87DC, -2em 0 0 0 #55A5E6, -1em 0 0 0 #6FC5F0, 1em 0 0 0 #181818, 2em 0 0 0 #181818, 3em 0 0 0 #181818, 4em 0 0 0 #181818, 5em 0 0 0 #181818, 6em 0 0 0 #181818, 7em 0 0 0 #181818, 8em 0 0 0 #181818, 9em 0 0 0 #181818;
      cursor: ew-resize;
    }

    input[type=range]::-moz-range-thumb {
      margin: 0.05em;
      padding: 0;
      height: 0.9em;
      width: 0.9em;
      border-radius: 0.1em;
      box-sizing: border-box;
      border: none;
      background-color: #6FC5F0;
      box-shadow: -10em 0 0 0 #313131, -9em 0 0 0 #313131, -8em 0 0 0 #2F343F, -7em 0 0 0 #283F6B, -6em 0 0 0 #214893, -5em 0 0 0 #1A52BC, -4em 0 0 0 #2769D3, -3em 0 0 0 #3E87DC, -2em 0 0 0 #55A5E6, -1em 0 0 0 #6FC5F0, 1em 0 0 0 #181818, 2em 0 0 0 #181818, 3em 0 0 0 #181818, 4em 0 0 0 #181818, 5em 0 0 0 #181818, 6em 0 0 0 #181818, 7em 0 0 0 #181818, 8em 0 0 0 #181818, 9em 0 0 0 #181818;
      cursor: ew-resize;
    }

    input[type=range]::-ms-thumb {
      margin: 0.05em;
      padding: 0;
      height: 0.9em;
      width: 0.9em;
      border-radius: 0.1em;
      box-sizing: border-box;
      border: none;
      background-color: #6FC5F0;
      box-shadow: -10em 0 0 0 #313131, -9em 0 0 0 #313131, -8em 0 0 0 #2F343F, -7em 0 0 0 #283F6B, -6em 0 0 0 #214893, -5em 0 0 0 #1A52BC, -4em 0 0 0 #2769D3, -3em 0 0 0 #3E87DC, -2em 0 0 0 #55A5E6, -1em 0 0 0 #6FC5F0, 1em 0 0 0 #181818, 2em 0 0 0 #181818, 3em 0 0 0 #181818, 4em 0 0 0 #181818, 5em 0 0 0 #181818, 6em 0 0 0 #181818, 7em 0 0 0 #181818, 8em 0 0 0 #181818, 9em 0 0 0 #181818;
      cursor: ew-resize;
    }

    /* ## Browser tweaks */
    /* ### webkit */
    input[type=range],
    input[type=range]::-webkit-slider-runnable-track,
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
    }

    /* ### IE */
    input[type=range]::-ms-track {
      color: transparent;
    }

    input[type=range]::-ms-fill-lower,
    input[type=range]::-ms-fill-upper,
    input[type=range]::-ms-tooltip {
      display: none;
    }

    /* Label *
 * ====== */
    /* ## Label main element */
    label[type=range] {
      display: block;
      position: relative;
      margin: 0;
      padding: 0;
      font-size: inherit;
      line-height: 0.6em;
      width: 9.9em;
      height: 1em;
      border-radius: 0.25em;
      color: #242424;
      font-family: arial;
      border: 0.2em solid transparent;
      text-align: center;
      border-bottom-color: #242424;
      background-image: linear-gradient(#242424 calc(50% - .1em), #242424 calc(50% + .1em));
      background-position: bottom center;
      background-repeat: no-repeat;
      background-size: 0.2em 0.2em;
      color: #999;
    }

    label[type=range]:first-letter {
      font-size: 0.6em;
    }

    label[type=range]::before,
    label[type=range]::after {
      display: block;
      position: absolute;
      height: 1em;
      line-height: 1em;
      width: 1em;
    }

    label[type=range]::before {
      content: attr(before);
      left: -0.2em;
      text-align: left;
    }

    label[type=range]::after {
      content: attr(after);
      top: 0;
      right: -0.2em;
      text-align: right;
    }

    label[type=range][orient="90deg"],
    label[type=range][orient="270deg"] {
      border-bottom-color: transparent;
      display: inline-block;
      width: 1em;
      height: 10.1em;
      line-height: 10.1em;
      vertical-align: top;
    }

    label[type=range][orient="90deg"] {
      border-bottom-color: transparent;
      border-left-color: #242424;
      background-position: center left;
      text-align: center;
    }

    label[type=range][orient="90deg"]::before {
      top: -0.2em;
      bottom: auto;
      left: 0.1em;
    }

    label[type=range][orient="90deg"]::after {
      left: 0.1em;
      top: auto;
      bottom: -0.2em;
      right: 0;
      text-align: left;
    }

    label[type=range][orient="180deg"] {
      border-bottom-color: transparent;
      border-top-color: #242424;
      background-position: top center;
      line-height: 1.4em;
    }

    label[type=range][orient="180deg"]::before {
      bottom: 0;
      left: auto;
      right: -0.2em;
      text-align: right;
    }

    label[type=range][orient="180deg"]::after {
      bottom: 0;
      left: -0.2em;
      right: auto;
      text-align: left;
    }

    label[type=range][orient="270deg"] {
      border-bottom-color: transparent;
      border-right-color: #242424;
      background-position: center right;
      text-align: center;
    }

    label[type=range][orient="270deg"]::before {
      bottom: -0.2em;
      top: auto;
      right: 0.1em;
      left: auto;
      text-align: right;
    }

    label[type=range][orient="270deg"]::after {
      right: 0.1em;
      bottom: auto;
      top: -0.2em;
      left: auto;
      text-align: right;
    }

    .canvas {
      overflow: scroll;
      width: 100%;
      max-width: 350px;
    }

    .mode {
      background-color: #F8F7F9;
      ;
      flex-basis: auto;
      height: 20px;
      text-align: center;
    }

    input:checked~.mode {
      background-color: #AAAAAA;
    }

    .card-m {
      /* height: 500px; */
      width: 100px;
      max-width: 600px;
      text-align: center;
      display: flex;
      flex-direction: row;
      flex-wrap: wrap;
    }

    input[type='radio'] {
      position: absolute;
      width: 0;
      height: 0;
    }

    .base1 {
      width: 32px;
      /* border-radius: 16px; */
      height: 56px;
      background-color: #ddd;
      transition: 0.3s;
    }

    input:checked~.base1 {
      background-color: rgb(219, 234, 254);
    }

    .circle1 {
      position: absolute;
      top: 4px;
      left: 4px;
      width: 24px;
      height: 24px;
      /* border-radius: 12px; */
      background-color: gray;
      transition: 0.3s;
    }

    input:checked~.circle1 {
      transform: translateY(100%);
      background-color: blue;
    }

    h3 {
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
      text-align: center;
      margin: 10px;
    }

    #mode-buttons {
      margin-top: 20px;
    }

    .mode-radio {
      display: none;
    }

    .mode-label {
      display: inline-block;
      padding: 10px 20px;
      margin: 5px;
      border: 2px solid #004080;
      border-radius: 5px;
      cursor: pointer;
      transition: background 0.3s, color 0.3s;
    }

    .mode-radio:checked+.mode-label {
      background: #004080;
      color: white;
    }
  </style>
  </style>
  <script>
    window.addEventListener('load', function () {

      // フルスクリーン表示
      document.getElementById('button1').addEventListener('click', function () {
        document.body.requestFullscreen();
      });

      // フルスクリーン解除
      document.getElementById('button2').addEventListener('click', function () {
        document.exitFullscreen();
      });
    });
  </script>
</head>

<body>
  <label for="em">
    <div class="switch">
      <input type="checkbox" nama="#" value="1" id="em" checked>
      <!-- <div class="circle"></div>
        <div class="base"></div> -->
      <div class="topnav">
        <h1>非常停止</h1>
      </div>
    </div>
  </label>
  <div class="content">
    <div class="label">
      <input type="radio" name="mode" id="mode1" class="mode-radio" checked>
      <label for="mode1" class="mode-label" onclick="changecard(0)">モード1</label>
      <input type="radio" name="mode" id="mode2" class="mode-radio">
      <label for="mode2" class="mode-label" onclick="changecard(1)">モード2</label>
    </div>
    <div class="card" id="ctrl">
      <fieldset>
        <legend>コントローラー</legend>
        <button id="button1">フルスクリーン表示</button>
        <button id="button2">解除</button>
        <label class="label1">
          <div>
            <fieldset>
              <legend>rps</legend>
              <span id="rps1">0</span>
              <a>&nbsp;r/s</a>
            </fieldset>
            <input type="number" id="kmove1" style="width: 50px;">
          </div>
          <div>
            <h3>toggle</h3>
            <div style="display: flex; justify-content: center; align-items: center;">
              <label for="toggle1">
                <div class="switch">
                  <input type="checkbox" nama="#" value="1" id="toggle1">
                  <div class="circle"></div>
                  <div class="base"></div>
                </div>
                <input type="number" id="kmove3">
              </label>
            </div>
            <button class="switch" id="update">update</button>
          </div>
          <div>
            <fieldset>
              <legend>rps2</legend>
              <span id="rps2">0</span>
              <a>&nbsp;r/s</a>
            </fieldset>
            <input type="number" id="kmove2" style="width: 50px;">
          </div>
        </label>
      </fieldset>
      <p></p>
    </div>
    <div id="graph" class="card">
      <div class="canvas">
        <canvas id="canvas" width="100%" height="200"></canvas>
      </div>
    </div>
    <script>
      var drw_x = [100, 200, 300, 400, 500, 600]
      var drw_y = [100, 50, 70, 80, 100, 0]
      var graph_array = [0, 0]
      window.onload = function () {
        // init_graph()
        // change()
        // document.getElementById('sss').addEventListener('click', () => {change()})
        // document.getElementById('mmm').addEventListener('click', () => {change()})
        // drawglaph(1)
      }
      let r = 0
      let g = 0
      let b = 0
      var serials = false;
      var text = null;
      var data = { type: null, value_1: null, value_2: null, value_3: null }
      var rdata = { type: null, value_1: null, value_2: null, value_3: null }
      var gateway = `ws://192.168.42.1:81`;
      var websocket;
      console.log(isSmartPhone())
      function isSmartPhone() {
        if (window.innerWidth <= 500) {
          return true;
        } else {
          return false;
        }
      }
      function m_i_1() {
        console.log(document.getElementById("mortor1_v").value - 0)
        document.getElementById("mortor_1").value = document.getElementById("mortor1_v").value
        mortor(1, document.getElementById("mortor_1").value * 0.9 + 90)
        return false
      }
      function m_i_2() {
        console.log(document.getElementById("mortor2_v").value - 0)
        document.getElementById("mortor_2").value = document.getElementById("mortor2_v").value
        //   console.log(document.getElementById("mortor_2").value*0.9+90)
        mortor(2, document.getElementById("mortor_2").value * 0.9 + 90)
        return false
      }
      function toggle(value1, value2) {
        data.type = "toggle"
        data.value_1 = value1
        data.value_2 = value2
        send()
      }
      function set(value1, value2) {
        data.type = "set"
        data.value_2 = value1
        data.value_1 = Math.round(value2)
        send()
      }
      function changeMode(mode) {
        console.log(mode)
        console.log("Mode changed to:", mode);
        data.type = "mode"
        data.value_1 = mode
        send()
      }
      window.addEventListener('load', onLoad);
      function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen = onOpen;
        websocket.onclose = onClose;
        websocket.onmessage = onMessage; // <-- add this line
      }
      function onOpen(event) {
        console.log('Connection opened');
      }
      function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 200);
      }
      function onMessage(event) {
        var data = event.data + ""
        rdata = JSON.parse(data)
        console.log(rdata)
        if (rdata.type == "servo") {
          document.getElementById(`${rdata.type}_${rdata.value_1}`).value = rdata.value_2
          document.getElementById(`${rdata.type}${rdata.value_1}_v`).value = rdata.value_2
        } else if (rdata.type == "mortor") {
          document.getElementById(`${rdata.type}_${rdata.value_1}`).value = (rdata.value_2) / 2.55
          document.getElementById(`${rdata.type}${rdata.value_1}_v`).value = (rdata.value_2) / 2.55
        } else if (rdata.type == "m_st" || rdata.type == "s_st") {
          document.getElementById(`${(rdata.type == "s_st") ? "servo" : "mortor"}${rdata.value_1}`).checked = ((rdata.value_2 == 0) ? 0 : 1)
        } else if (rdata.type == "servos" || rdata.type == "mortors" || rdata.type == "soles") {
          document.getElementById(`${rdata.type}`).checked = ((rdata.value_1 == 0) ? 0 : 1)
        } else if (rdata.type == "sole") {
          document.getElementById(`${rdata.type}${rdata.value_1}`).checked = ((rdata.value_2 == 0) ? 0 : 1)
        } else if (rdata.type == "em") {
          document.getElementById("em").checked = rdata.value_2
        } else if (rdata.type == "rps") {
          document.getElementById(`rps${rdata.value_1 + 1}`).innerHTML = rdata.value_2
        } else if (rdata.type == "graph") {
          graph_array[0] = rdata.value_1
          graph_array[1] = rdata.value_2
          graph_array[2] = rdata.value_3
          graph(graph_array[0], graph_array[1], graph_array[2])
        }
      }
      function onLoad(event) {
        initWebSocket();
        initButton();
        document.getElementById("graph").style.display = "none"
        document.getElementById("ctrl").style.display = ""
      }
      function graph(value1, value2, value3) {

        var canvas = document.getElementById('canvas');
        var ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.beginPath();
        ctx.moveTo(0, 100);
        ctx.lineTo(600, 100);
        ctx.moveTo(0, 0);
        ctx.lineTo(0, 200);
        ctx.stroke();
        ctx.beginPath();
        ctx.strokeStyle = "red";
        ctx.lineWidth = 2;
        ctx.moveTo(0, 100 - value1);
        ctx.lineTo(100, 100 - value2);
        ctx.lineTo(200, 100 - value3);
        ctx.stroke();


      }
      function changecard(value) {
        if (value == 0) {
          document.getElementById("graph").style.display = "none"
          document.getElementById("ctrl").style.display = ""
        } else {
          document.getElementById("graph").style.display = ""
          document.getElementById("ctrl").style.display = "none"
        }
      }
      function mortor_1() {
        document.getElementById("mortor1_v").value = document.getElementById("robomas1").value
        set(0, Math.round(document.getElementById("robomas1").value * 2.55) * document.getElementById("kmove1").value)
        set(1, -1 * Math.round(document.getElementById("robomas1").value * 2.55) * document.getElementById("kmove1").value)
      }
      function mortor_2() {
        document.getElementById("mortor2_v").value = document.getElementById("robomas2").value
        set(1, Math.round(document.getElementById("robomas2").value * 2.55) * document.getElementById("kmove2").value)
        set(0, Math.round(document.getElementById("robomas2").value * 2.55) * document.getElementById("kmove2").value)
      }
      function toggle_1() {
        toggle(0, document.getElementById("toggle1").checked)
      }
      function toggle_2() {
        toggle(1, document.getElementById("toggle2").checked)
      }
      function em() {
        data.type = "em"
        data.value_1 = document.getElementById("em").checked
        send()
      }
      function sned_config() {
        data.type = "config"
        data.value_1 = document.getElementById("kmove1").value
        data.value_2 = document.getElementById("kmove2").value
        send()
      }
      function initButton() {
        document.getElementById('em').addEventListener('input', () => { em() })
        document.getElementById('toggle1').addEventListener('input', () => { toggle_1() })
        // document.getElementById('toggle2').addEventListener('input', () => { toggle_2() })
        // document.getElementById('robomas1').addEventListener('input', () => { mortor_1() })
        // document.getElementById('robomas2').addEventListener('input', () => { mortor_2() })
        document.getElementById("update").addEventListener('click', () => { sned_config() })
        document.getElementById("kmove1").value = 100
        document.getElementById("kmove2").value = 100
        document.getElementById("kmove3").value = 120
      }
      function send() {
        var wdata = {
          "type": data.type,
          "value_1": data.value_1,
          "value_2": data.value_2,
          "value_3": data.value_3
        }
        console.log(JSON.stringify(wdata));
        websocket.send(JSON.stringify(wdata));
        data.type = null
        data.value_1 = null
        data.value_2 = null
        data.value_3 = null
      }
    </script>
</body>

</html>
)rawliteral";
