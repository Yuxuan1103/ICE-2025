// =======================
// STATE
// =======================

const poles = {};
const markers = {};
let selectedPole = null;

let pitch = 0, roll = 0, yaw = 0;
let wsConnected = false;
let loraConnected = false;
let monitoringDeviceId = null;

// =======================
// CANVAS SETUP
// =======================

const pitchCtx = document.getElementById("pitchCanvas").getContext("2d");
const rollCtx  = document.getElementById("rollCanvas").getContext("2d");
const yawCtx   = document.getElementById("yawCanvas").getContext("2d");

// =======================
// DRAWING
// =======================

function drawPole(ctx, angle) {
  const w = ctx.canvas.width;
  const h = ctx.canvas.height;

  ctx.clearRect(0, 0, w, h);

  ctx.save();
  ctx.translate(w / 2, h * 0.9);
  ctx.rotate(angle);

  ctx.beginPath();
  ctx.moveTo(0, 0);
  ctx.lineTo(0, -h * 0.7);
  ctx.lineWidth = 6;
  ctx.strokeStyle = "#4cc9f0";
  ctx.stroke();

  ctx.restore();
}

// =======================
// UI
// =======================

function degToDisplay(d) {
  return d.toFixed(1);
}

function updateUI() {
  drawPole(pitchCtx, degToRad(pitch));
  drawPole(rollCtx,  degToRad(roll));
  drawPole(yawCtx,   degToRad(yaw));

  document.getElementById("pitchValue").textContent = degToDisplay(pitch) + "\u00B0";
  document.getElementById("rollValue").textContent  = degToDisplay(roll)  + "\u00B0";
  document.getElementById("yawValue").textContent   = degToDisplay(yaw)   + "\u00B0";
}

// =======================
// ANIMATION LOOP
// =======================

function animate() {
  updateUI();
  requestAnimationFrame(animate);
}
animate();

// =======================
// MAP
// =======================

const map = L.map("map").setView([40.7128, -74.0060], 13);

L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
  maxZoom: 19,
  attribution: "&copy; OpenStreetMap"
}).addTo(map);

// =======================
// POLE MANAGEMENT
// =======================

function addOrUpdatePole(data) {
  if (!poles[data.id]) {
    poles[data.id] = data;

    const marker = L.marker([data.lat || 40.7128, data.lon || -74.0060]).addTo(map);
    marker.bindPopup("Device: " + data.id);
    marker.on("click", function () { selectPole(data.id); });

    markers[data.id] = marker;
  } else {
    Object.assign(poles[data.id], data);
  }
}

function selectPole(id) {
  selectedPole = poles[id];
  document.getElementById("currentPole").textContent = id;
  applyAngles(selectedPole);
}

// =======================
// ANGLE HELPERS
// =======================

function degToRad(d) {
  return d * Math.PI / 180;
}

function applyAngles(data) {
  pitch = data.pitch;
  roll  = data.roll;
  yaw   = data.yaw;
}

// =======================
// WEBSOCKET HELPERS
// =======================

function sendEvent(eventType, data) {
  if (socket && socket.readyState === WebSocket.OPEN) {
    socket.send(JSON.stringify({ eventType: eventType, data: data }));
  }
}

function updateStatusUI() {
  var statusEl = document.getElementById("systemStatus");
  var loraEl   = document.getElementById("loraStatus");

  if (wsConnected) {
    statusEl.textContent = "Connected";
    statusEl.className   = "status-badge connected";
  } else {
    statusEl.textContent = "Disconnected";
    statusEl.className   = "status-badge disconnected";
  }

  loraEl.textContent = loraConnected ? "LoRa: Connected" : "LoRa: Not connected";
}

// =======================
// CONTROL BUTTONS
// =======================

document.getElementById("btnLoraConnect").addEventListener("click", function () {
  sendEvent("LORA_CONNECT", {});
});

document.getElementById("btnStartMonitor").addEventListener("click", function () {
  var deviceId = parseInt(document.getElementById("deviceIdInput").value, 10);
  if (isNaN(deviceId) || deviceId < 1) return;

  sendEvent("BEGIN_DEVICE_MONITOR", { deviceId: deviceId });
  monitoringDeviceId = deviceId;

  document.getElementById("btnStartMonitor").disabled = true;
  document.getElementById("btnStopMonitor").disabled  = false;
});

document.getElementById("btnStopMonitor").addEventListener("click", function () {
  if (monitoringDeviceId !== null) {
    sendEvent("STOP_DEVICE_MONITOR", { deviceId: monitoringDeviceId });
    monitoringDeviceId = null;

    document.getElementById("btnStartMonitor").disabled = false;
    document.getElementById("btnStopMonitor").disabled  = true;
  }
});

// =======================
// WEBSOCKET (CONNECT TO BACKEND)
// =======================

var wsHost = window.location.hostname || "localhost";
var socket = null;
var wsReconnectTimer = null;

function connectWebSocket() {
  if (socket && (socket.readyState === WebSocket.OPEN || socket.readyState === WebSocket.CONNECTING)) {
    return;
  }

  socket = new WebSocket("ws://" + wsHost + ":9988");

  socket.onopen = function () {
    console.log("[WS] Connected to backend");
    wsConnected = true;
    updateStatusUI();
    if (wsReconnectTimer) { clearTimeout(wsReconnectTimer); wsReconnectTimer = null; }
  };

  socket.onmessage = function (event) {
    var msg;
    try { msg = JSON.parse(event.data); } catch (e) { return; }

    var eventType = msg.eventType;
    var data      = msg.data;

    switch (eventType) {

      // ---- 实时倾斜数据推送 ----
      case "MONITOR_INCLINATION_RECORD": {
        var deviceId = data.deviceId;
        var records  = data.data;           // [[ts, x, y, z], ...]

        if (records && records.length > 0) {
          var latest = records[records.length - 1];
          // latest = [timestamp, x(顺线角), y(横线角), z(综合角)]
          var poleData = {
            id:    "pole_" + deviceId,
            pitch: latest[1],   // 顺线角 → Pitch
            roll:  latest[2],   // 横线角 → Roll
            yaw:   latest[3],   // 综合角 → Yaw
            lat:   40.7128,
            lon:  -74.0060
          };

          addOrUpdatePole(poleData);

          // 自动选中第一个杆塔
          if (!selectedPole) {
            selectPole(poleData.id);
          }

          if (selectedPole && selectedPole.id === poleData.id) {
            applyAngles(poleData);
          }
        }
        break;
      }

      // ---- 系统状态同步 ----
      case "SYNC_SYSTEM_STATUS": {
        loraConnected = !!data.loraConnected;
        updateStatusUI();
        break;
      }

      // ---- LoRa 连接应答 ----
      case "LORA_CONNECT": {
        if (data.code === 0) {
          loraConnected = true;
          console.log("[LoRa] " + data.message);
        } else {
          loraConnected = false;
          console.error("[LoRa] " + data.message);
        }
        updateStatusUI();
        break;
      }

      // ---- 设备信息查询应答 ----
      case "QUERY_DEVICE_INFO": {
        console.log("[Device Info]", data);
        break;
      }

      default:
        console.log("[WS] Unknown event:", eventType, data);
    }
  };

  socket.onerror = function (err) {
    console.error("[WS] Error:", err);
  };

  socket.onclose = function () {
    console.log("[WS] Disconnected, will retry in 3s...");
    wsConnected   = false;
    loraConnected = false;
    updateStatusUI();
    // 自动重连
    if (!wsReconnectTimer) {
      wsReconnectTimer = setTimeout(function () {
        wsReconnectTimer = null;
        connectWebSocket();
      }, 3000);
    }
  };
}

connectWebSocket();
