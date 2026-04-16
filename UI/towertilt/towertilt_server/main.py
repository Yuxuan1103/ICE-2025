# !/usr/bin/python3
# coding: utf-8

import asyncio

import websockets
import json
from flask import Flask, render_template, request, jsonify, send_from_directory
import threading
import os

from db_server import DbServer
from lora_server import LoraServer

class WebServer(object):
    """docstring for App"""

    def __init__(self):
        super(WebServer, self).__init__()
        self.app = Flask(__name__, static_folder='dist', static_url_path='', template_folder='dist')
        self.warp_rsp = lambda event_type, data: { "eventType": event_type, "data": data }

        self.monitoring_devices = set()  # 跟踪正在监控的设备
        self.active_connections = set()  # 添加连接池
        self.db = DbServer()
        self.lora_server = LoraServer(self.db, self.on_lora_receive_msg)
        self.setup_routes()

        self.system_status = {
            'loraConnected': False,
            'connectDeviceNum': 0,
            'monitorDeviceNum': 0
        }

    async def on_lora_receive_msg(self, packet):
        if packet.did in self.monitoring_devices:
            data = {
                'deviceId': packet.did,
                'data': [[packet.ts, packet.x, packet.y, packet.z]]
            }
            await self.broadcast_lora_data(data)

    async def broadcast_lora_data(self, lora_data):
        """广播LoRa数据到所有连接的客户端"""
        event_type = "MONITOR_INCLINATION_RECORD"
        rsp = self.warp_rsp(event_type, lora_data)
        await self.send_to_all(rsp)

    async def send_to_all(self, message):
        """发送消息到所有连接的客户端"""
        tasks = []
        disconnected = set()

        for websocket in self.active_connections:
            try:
                tasks.append(websocket.send(json.dumps(message)))
                print("[Response]", message)
            except Exception as e:
                print(f"[WS] Failed to send to client: {e}")
                disconnected.add(websocket)

        # 移除已断开的连接
        for ws in disconnected:
            self.active_connections.discard(ws)

        if tasks:
            await asyncio.gather(*tasks, return_exceptions=True)


    async def handler(self, websocket) -> None:
        self.active_connections.add(websocket)
        print(f"[WS] New connection established. Total connections: {len(self.active_connections)}")
        await self.sync_system_status()

        try:
            async for message in websocket:
                print("[ Request]", message)
                req = json.loads(message)
                event_type, req_data = (req['eventType'], req['data'])

                rsp_data = None

                if event_type == 'QUERY_DEVICE_INFO': # 查询设备信息
                    rsp_data = {
                        'deviceId': 3,
                        'name': 'Displacement_sensor416#',
                        'number': '356566078066735',
                        'protocol': 'Lora',
                        'sampling': 'None (the device actively uploads)',
                        'lastUpdate': '2024-10-14 14:39:03'
                    }
                elif event_type == 'BEGIN_DEVICE_MONITOR': # 添加设备到监控列表
                    self.monitoring_devices.add(req_data['deviceId'])
                    await self.sync_system_status()

                elif event_type == 'STOP_DEVICE_MONITOR': # 停止监控设备
                    self.monitoring_devices.discard(req_data['deviceId'])
                    await self.sync_system_status()

                elif event_type == 'LORA_CONNECT':
                    self.lora_server.init_lora('COM3', 115200)
                    if self.lora_server.lora_connected:
                        rsp_data = { 'code': 0, 'message': 'Open lora station success!'}
                    else:
                        rsp_data = { 'code': 1, 'message': 'Failed to open lora station!' }
                    await self.sync_system_status()

                elif event_type == 'QUERY_INCLINATION_RECORD':
                    did, ts_s, ts_e = req_data['did'], req_data['ts_s'], req_data['ts_e']
                    data = self.db.query_inclination_record(did, ts_s, ts_e) # 数据量大的时候可优化采样频率
                    rsp_data = {
                        'deviceId': did,
                        'data': list(map(lambda x: [x.ts, x.x, x.y, x.z], data))
                    }
                if rsp_data is None: rsp_data = 'OK'
                rsp = self.warp_rsp(event_type, rsp_data)
                await websocket.send(json.dumps(rsp))
                print("[Response]", rsp)
        finally:
            # 连接断开时从连接池中移除
            self.active_connections.discard(websocket)
            print(f"[WS] Connection closed. Total connections: {len(self.active_connections)}")

    async def sync_system_status(self):
        self.system_status['loraConnected'] = self.lora_server.lora_connected
        self.system_status['connectDeviceNum'] = len(self.active_connections)
        self.system_status['monitorDeviceNum'] = len(self.monitoring_devices)
        
        event_type = "SYNC_SYSTEM_STATUS"
        rsp = self.warp_rsp(event_type, self.system_status)
        await self.send_to_all(rsp)

    def setup_routes(self):
        @self.app.route('/')
        def serve_index():
            return send_from_directory(self.app.static_folder, 'index.html')

    def run_web(self, debug=False): self.app.run(host="localhost", port=9987, debug=debug, threaded=True, use_reloader=False)

    def run(self):
        async def start():
            async with websockets.serve(self.handler, "localhost", 9988):
                await asyncio.Future()

        print('[WS] Server start...')
        asyncio.run(start())

        print(".....")


if __name__ == '__main__':
    server = WebServer()

    web_server_thread = threading.Thread(target=server.run_web, args=(True,), daemon=True)
    web_server_thread.start()

    server.run()

