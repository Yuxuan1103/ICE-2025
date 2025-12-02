const canvas = document.getElementById("simCanvas");
const ctx = canvas.getContext("2d");

canvas.width = 600;
canvas.height = 400;

// Pole properties
let angle = Math.PI / 24;    
let angularVel = 0;
let angularAcc = 0;
let length = 250;
let gravity = 0.005;        
let damping = 0.995;        
let angleLimit = Math.PI / 48; 
let poleWidth = 20;
// 3D angle placeholders
let pitch = 0;
let roll = 0;
let yaw = 0;

// Update physics
function update() {
  angularAcc = -gravity * Math.sin(angle);
  angularVel += angularAcc;
  angle += angularVel;

  // 3D angles
  pitch = angle * 180 / Math.PI;

  // fake roll: small sideways sway
  roll = Math.sin(angle * 2.2) * 10;

  // fake yaw: slow twist
  yaw = Math.cos(angle * 1.3) * 15;
}


// Draw rectangular pole
function drawPole(x1, y1, x2, y2, width) {
  const dx = x2 - x1;
  const dy = y2 - y1;
  const rot = Math.atan2(dy, dx);
  ctx.save();
  ctx.translate(x1, y1);
  ctx.rotate(rot);
  const wood = ctx.createLinearGradient(0, -width/2, 0, width/2);
  wood.addColorStop(0, "#6b4f2d");
  wood.addColorStop(0.5, "#8d6e3f");
  wood.addColorStop(1, "#5a3e20");
  ctx.fillStyle = wood;
  ctx.fillRect(0, -width/2, Math.hypot(dx, dy), width);
  ctx.restore();
}

function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  const pivotX = canvas.width / 2;
  const pivotY = 350;
  const endX = pivotX + length * Math.sin(angle);
  const endY = pivotY - length * Math.cos(angle);
  drawPole(pivotX, pivotY, endX, endY, poleWidth);
}
function angleDisplay(){
  ctx.fillStyle = Math.abs(angle) < angleLimit ? "#3afd04ff" : "#f00";
  ctx.font = "20px monospace";
  ctx.fillText("Angle: " + (angle * 180 / Math.PI).toFixed(2) + "°", 20, 30);
}
function rotationDisplay() {
  ctx.fillStyle = "#fff";
  ctx.font = "18px monospace";
  ctx.fillText(`Pitch: ${pitch.toFixed(2)}°`, 20, 60);
  ctx.fillText(`Roll:  ${roll.toFixed(2)}°`, 20, 85);
  ctx.fillText(`Yaw:   ${yaw.toFixed(2)}°`, 20, 110);
}

function loop() {
  update();
  draw();
  angleDisplay();
  rotationDisplay();
  requestAnimationFrame(loop);
}

loop();
