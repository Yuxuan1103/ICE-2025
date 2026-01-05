// ========================
// Canvas setup
// ========================

const pitchCanvas = document.getElementById("pitchCanvas");
const rollCanvas = document.getElementById("rollCanvas");
const yawCanvas = document.getElementById("yawCanvas");

const pitchCtx = pitchCanvas.getContext("2d");
const rollCtx = rollCanvas.getContext("2d");
const yawCtx = yawCanvas.getContext("2d");

// ========================
// State
// ========================

let pitch = 0; // radians
let roll = 0;
let yaw = 0;

// Pitch physics (pendulum)
let pitchVelocity = 0;
const gravity = 0.8;
const damping = 0.995;

// Time
let lastTime = performance.now();

// ========================
// Drawing
// ========================

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
  ctx.strokeStyle = "#ffffff";
  ctx.stroke();

  ctx.restore();

  // base
  ctx.beginPath();
  ctx.arc(w / 2, h * 0.9, 6, 0, Math.PI * 2);
  ctx.fillStyle = "#ff5555";
  ctx.fill();
}

// ========================
// Simulation
// ========================

function updatePhysics(dt) {
  // Pitch pendulum
  const force = -gravity * Math.sin(pitch);
  pitchVelocity += force * dt;
  pitchVelocity *= damping;
  pitch += pitchVelocity * dt;

  // Placeholder roll & yaw
  const t = performance.now() * 0.001;
  roll = Math.sin(t * 0.7) * 0.4;
  yaw = Math.sin(t * 0.4 + 1) * 0.6;
}

// ========================
// UI update
// ========================

function radToDeg(r) {
  return (r * 180 / Math.PI).toFixed(1);
}

function updateUI() {
  drawPole(pitchCtx, pitch);
  drawPole(rollCtx, roll);
  drawPole(yawCtx, yaw);

  document.getElementById("pitchValue").textContent =
    `${radToDeg(pitch)}°`;
  document.getElementById("rollValue").textContent =
    `${radToDeg(roll)}°`;
  document.getElementById("yawValue").textContent =
    `${radToDeg(yaw)}°`;
}

// ========================
// Main loop
// ========================

function animate(now) {
  const dt = Math.min((now - lastTime) / 1000, 0.05);
  lastTime = now;

  updatePhysics(dt);
  updateUI();

  requestAnimationFrame(animate);
}

requestAnimationFrame(animate);

// ========================
// Map + Geolocation
// ========================

const map = L.map("map").setView([0, 0], 15);

L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
  attribution: "&copy; OpenStreetMap contributors"
}).addTo(map);

const marker = L.marker([0, 0]).addTo(map);

if ("geolocation" in navigator) {
  navigator.geolocation.watchPosition(
    (pos) => {
      const lat = pos.coords.latitude;
      const lon = pos.coords.longitude;
      marker.setLatLng([lat, lon]);
      map.setView([lat, lon]);
    },
    (err) => {
      console.error("Geolocation error:", err);
    },
    {
      enableHighAccuracy: true
    }
  );
}
