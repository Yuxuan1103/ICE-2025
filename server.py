import asyncio
import websockets
import serial
import json

# ===== SERIAL SETUP =====
ser = serial.Serial("COM3", 115200, timeout=1)  # change COM port

# ===== CLIENTS =====
clients = set()

async def handler(websocket):
    clients.add(websocket)
    try:
        await websocket.wait_closed()
    finally:
        clients.remove(websocket)

# ===== SERIAL READ LOOP =====
async def serial_loop():
    while True:
        try:
            line = ser.readline().decode("utf-8").strip()

            if not line:
                await asyncio.sleep(0.01)
                continue

            # EXPECTED FORMAT FROM SENSOR:
            # {"id":"pole_001","pitch":10,"roll":2,"yaw":5}
            data = json.loads(line)

            # send to all connected clients
            if clients:
                await asyncio.gather(
                    *[client.send(json.dumps(data)) for client in clients]
                )

        except Exception as e:
            print("Error:", e)

        await asyncio.sleep(0.01)

# ===== MAIN =====
async def main():
    server = await websockets.serve(handler, "localhost", 3000)
    print("WebSocket running on ws://localhost:3000")

    await asyncio.gather(server.wait_closed(), serial_loop())

asyncio.run(main())