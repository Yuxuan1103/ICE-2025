// =======================
// STATE
// =======================

const poles = {};
const markers = {};
let selectedPole = null;

let pitch = 0, roll = 0, yaw = 0;

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

function radToDeg(r) {
  return (r * 180 / Math.PI).toFixed(1);
}

function updateUI() {
  drawPole(pitchCtx, pitch);
  drawPole(rollCtx, roll);
  drawPole(yawCtx, yaw);

  document.getElementById("pitchValue").textContent = radToDeg(pitch) + "°";
  document.getElementById("rollValue").textContent  = radToDeg(roll) + "°";
  document.getElementById("yawValue").textContent   = radToDeg(yaw) + "°";
}

// =======================
// LOOP
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

L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png").addTo(map);

// =======================
// POLE MANAGEMENT
// =======================

function addOrUpdatePole(data) {
  if (!poles[data.id]) {
    poles[data.id] = data;

    const marker = L.marker([data.lat || 40.7, data.lon || -74]).addTo(map);

    marker.on("click", () => selectPole(data.id));

    markers[data.id] = marker;
  } else {
    Object.assign(poles[data.id], data);
  }
}

function selectPole(id) {
  selectedPole = poles[id];

  document.getElementById("currentPole").textContent = id;

  updateAngles(selectedPole);
}

// =======================
// ANGLE UPDATE
// =======================

function degToRad(d) {
  return d * Math.PI / 180;
}

function updateAngles(data) {
  pitch = degToRad(data.pitch);
  roll  = degToRad(data.roll);
  yaw   = degToRad(data.yaw);
}

// =======================
// WEBSOCKET (REAL DATA)
// =======================

const socket = new WebSocket("ws://localhost:3000");

socket.onopen = () => {
  console.log("Connected to server");
};

socket.onmessage = (event) => {
  const data = JSON.parse(event.data);

  addOrUpdatePole(data);

  if (selectedPole && selectedPole.id === data.id) {
    updateAngles(data);
  }
};

socket.onerror = (err) => {
  console.error("WebSocket error:", err);
};

socket.onclose = () => {
  console.log("Disconnected from server");
};