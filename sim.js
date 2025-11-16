const canvas = document.getElementById("simCanvas");
const ctx = canvas.getContext("2d");

canvas.width = 600;
canvas.height = 400;

// pole properties
let angle = Math.PI / 4;      // starting tilt
let angularVel = 0;
let angularAcc = 0;
let length = 150;             // length of the pole
let gravity = 0.005;          // fake gravity
let damping = 0.995;          // slows the swinging

function update() {
  // pendulum physics
  angularAcc = -gravity * Math.sin(angle);
  angularVel += angularAcc;
  angularVel *= damping;
  angle += angularVel;
}

function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // pivot point
  const pivotX = canvas.width / 2;
  const pivotY = 50;

  // pole endpoint
  const endX = pivotX + length * Math.sin(angle);
  const endY = pivotY + length * Math.cos(angle);

  // draw pole
  ctx.strokeStyle = "#00d5ff";
  ctx.lineWidth = 6;
  ctx.beginPath();
  ctx.moveTo(pivotX, pivotY);
  ctx.lineTo(endX, endY);
  ctx.stroke();

  // draw pivot
  ctx.fillStyle = "#ffffff";
  ctx.beginPath();
  ctx.arc(pivotX, pivotY, 8, 0, Math.PI * 2);
  ctx.fill();
}

function loop() {
  update();
  draw();
  requestAnimationFrame(loop);
}

loop();
