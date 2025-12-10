document.addEventListener("DOMContentLoaded", () => {

  function setupCanvas(id) {
    const c = document.getElementById(id);
    const w = 300, h = 300;

    // REAL pixel resolution
    c.width = w;
    c.height = h;

    return c.getContext("2d");
  }

  const ctxPitch = setupCanvas("cPitch");
  const ctxRoll  = setupCanvas("cRoll");
  const ctxYaw   = setupCanvas("cYaw");

  const pitchEl = document.getElementById("pitchVal");
  const rollEl  = document.getElementById("rollVal");
  const yawEl   = document.getElementById("yawVal");

  let angle = Math.PI / 20;
  let angularVel = 0;
  let angularAcc = 0;

  const gravity = 0.008;
  const damping = 0.995;
  const length = 120;
  const poleWidth = 14;

  function physics() {
    angularAcc = -gravity * Math.sin(angle);
    angularVel += angularAcc;
    angularVel *= damping;
    angle += angularVel;

    return {
      pitch: angle * 180 / Math.PI,
      roll: Math.sin(angle * 2.0) * 12,
      yaw: Math.cos(angle * 6) * 35

    };
  }

  function drawPole(ctx, deg) {
    const w = ctx.canvas.width;
    const h = ctx.canvas.height;

    ctx.clearRect(0, 0, w, h);

    const pivotX = w / 2;
    const pivotY = h - 20;
    const rad = deg * Math.PI / 180;

    ctx.save();

    // Move to the pivot
    ctx.translate(pivotX, pivotY);

    // Canvas 0 rad points to the right, so rotate 90° counterclockwise
    ctx.rotate(rad - Math.PI / 2);

    // Draw pole extending "up" relative to the pivot
    ctx.fillStyle = "#9a7442";
    ctx.fillRect(0, -poleWidth / 2, length, poleWidth);

    ctx.restore();

    // Pivot dot
    ctx.fillStyle = "#eee";
    ctx.beginPath();
    ctx.arc(pivotX, pivotY, 6, 0, Math.PI * 2);
    ctx.fill();
  }



  function loop() {
    const { pitch, roll, yaw } = physics();

    pitchEl.textContent = pitch.toFixed(2) + "°";
    rollEl.textContent  = roll.toFixed(2) + "°";
    yawEl.textContent   = yaw.toFixed(2) + "°";

    drawPole(ctxPitch, pitch);
    drawPole(ctxRoll, roll);
    drawPole(ctxYaw, yaw);

    requestAnimationFrame(loop);
  }

  loop();
});
