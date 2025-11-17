const canvas = document.getElementById("simCanvas");
const ctx = canvas.getContext("2d");

canvas.width = 600;
canvas.height = 400;

// pole properties
let angle = Math.PI / 24;    
let angularVel = 0;
let angularAcc = 0;
let length = 250;
let gravity = 0.005;        
let damping = 0.995;        
let angleLimit = Math.PI / 48; 
let poleWidth = 20;

// update physics
function update() {
  angularAcc = -gravity * Math.sin(angle);
  angularVel += angularAcc;
  // angularVel *= damping;
  angle += angularVel;
}

// draw rectangular pole
function drawPole(x1, y1, x2, y2, width) {
  const dx = x2 - x1;
  const dy = y2 - y1;
  const rot = Math.atan2(dy, dx);

  ctx.save();
  ctx.translate(x1, y1);
  ctx.rotate(rot);

  // gradient metal pole
  const wood = ctx.createLinearGradient(0, -width/2, 0, width/2);
  wood.addColorStop(0, "#6b4f2d");
  wood.addColorStop(0.5, "#8d6e3f");
  wood.addColorStop(1, "#5a3e20");

  ctx.fillStyle = wood;
  ctx.beginPath();
  ctx.fillRect(0, -width/2, Math.hypot(dx, dy), width, width/2);
  ctx.fill();

  ctx.restore();
}


function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  const pivotX = canvas.width / 2;
  const pivotY = 350;

  const endX = pivotX + length * Math.sin(angle);
  const endY = pivotY - length * Math.cos(angle);

  drawPole(pivotX, pivotY, endX, endY, poleWidth);

  // angle display
  if (Math.abs(angle) < angleLimit) {
    ctx.fillStyle = "#3afd04ff";
  } else {
    ctx.fillStyle = "#f00";
  }
  ctx.font = "20px monospace";
  ctx.fillText("Angle: " + (angle * 180 / Math.PI).toFixed(2) + "°", 20, 30);
}
function loop() {
  update();
  draw();
  requestAnimationFrame(loop);
}

loop();
