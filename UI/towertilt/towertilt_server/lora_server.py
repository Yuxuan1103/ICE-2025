# !/usr/bin/python3
# coding: utf-8

import struct
import serial
import asyncio
import time
import random

from typing import Optional

from logger import info
from db_server import InclinationRecord

def bytes_to_float(byte_data: bytes) -> float:
    try:
        return struct.unpack('>f', byte_data)[0]  # 小端序
    except:
        return 0.0

class LoraServer(object):
    """docstring for App"""
    def __init__(self, db, on_lora_receive_msg):
        super(LoraServer, self).__init__()
        self.db = db
        self.on_lora_receive_msg = on_lora_receive_msg
        self.lora_connected = False
        self.lora_serial = None
        self._lora_task = None

    def init_lora(self, port='COM3', baud_rate=9600):
        if self.lora_connected: return
        """初始化LoRa串口连接"""
        try:
            self.lora_serial = serial.Serial(port, baud_rate, timeout=1)
            info(f"[LoRa] Connected to {port}")
            self._lora_task = asyncio.create_task(self.lora_data_receiver())
            info(f"[LoRa] Receiver open")
            self.lora_connected = True
        except Exception as e:
            info(f"[LoRa] Failed to connect: {e}")
            self.lora_connected = False
            # self._lora_task = asyncio.create_task(self.lora_data_receiver_test())

    async def lora_data_receiver_test(self):
        while True:
            rr = random.random()
            packet = InclinationRecord(3, int(time.time()), round(1.2+rr, 3), round(0.241+rr, 3), round(-0.125+rr, 3))
            self.db.insert_inclination_record(packet)
            await self.on_lora_receive_msg(packet)
            await asyncio.sleep(1)

    async def lora_data_receiver(self):
        """LoRa数据接收任务"""
        if not self.lora_connected or not self.lora_serial:
            info("LoRa未连接或串口未初始化")
            return

        info("启动LoRa数据接收任务")
        reconnect_attempts = 0
        max_reconnect_attempts = 5

        while self.lora_connected:
            try:
                # 检查串口是否仍然可用
                if not self.lora_serial.is_open:
                    info("LoRa串口已关闭")
                    break

                if self.lora_serial.in_waiting > 0:
                    # 读取LoRa数据
                    # 读取可用数据
                    available_bytes = self.lora_serial.in_waiting
                    raw_data = self.lora_serial.read(available_bytes)

                    # print('available_bytes', available_bytes)
                    # print('raw_data', bytes(raw_data).hex(' '), '\n\n')

                    if raw_data:
                        # 解析LoRa数据
                        parsed_packets = self.parse_lora_data_packets(raw_data)
                        print(parsed_packets)

                        for packet in parsed_packets:
                            if packet:
                                self.db.insert_inclination_record(packet) # 可优化为缓存加异步存储
                                await self.on_lora_receive_msg(packet)

                await asyncio.sleep(0.01)
                reconnect_attempts = 0

            except serial.SerialException as e:
                info(f"LoRa串口通信错误: {e}")

                reconnect_attempts += 1
                if reconnect_attempts >= max_reconnect_attempts:
                    info("LoRa重连次数超限，停止接收")
                    break
                await asyncio.sleep(1)  # 重连前等待

            except Exception as e:
                info(f"LoRa数据接收未知错误: {e}")
                await asyncio.sleep(0.1)
        info("LoRa数据接收任务结束")
        self.lora_connected = False

    def parse_lora_data_packets(self, raw_data: bytes) -> list:
        """解析LoRa原始数据"""
        packets = []
        data_buffer = bytearray(raw_data)

        # 据格式为: 0xAA LoraOrder_TiltData LoraTypeOrder GatID WayID 0x11 x[4] y[4] z[4] temp[4] v A_flag crc16H crc16l 0x55
        # x [1].low [1].high [0].low [0].high

        # 查找完整的数据包
        start_index = 0
        while start_index < len(data_buffer):
            # 查找帧头 0xAA
            if data_buffer[start_index] != 0xAA:
                start_index += 1
                continue

            if len(data_buffer) - start_index < 27:
                return []

            # 检查帧尾 0x55
            if data_buffer[start_index + 26] != 0x55:  # 帧尾位置
                start_index += 1
                continue

            try:
                # 解析数据包
                packet_data = data_buffer[start_index:start_index + 27]
                parsed_packet = self.parse_single_packet(packet_data)

                if parsed_packet:
                    packets.append(parsed_packet)

                start_index += 27  # 移动到下一个包

            except Exception as e:
                info(f"解析数据包失败: {e}")
                start_index += 1

        return packets

    def parse_single_packet(self, packet_data: bytes) -> Optional[InclinationRecord]:
        """解析单个数据包"""
        try:
            # 解包二进制数据
            # 格式: 0xAA(1) + 命令(1) + 类型(1) + GatID(1) + WayID(1) + 0x11(1) +
            #       x(4) + y(4) + z(4) + temp(4) + voltage(1) + A_flag(1) + crc16h + crc16l + 0x55(1)

            fields = struct.unpack_from('>6B 4f 2B H B', packet_data, 0)
            (header, command, msg_type, gat_id, way_id, marker, x, y, z, temperature, voltage, a_flag, crc16_received, footer) = fields

            return InclinationRecord(way_id, int(time.time()), round(x, 3), round(y, 3), round(z, 3))

        except Exception as e:
            info(f"解析单个数据包错误: {e}")
            return None