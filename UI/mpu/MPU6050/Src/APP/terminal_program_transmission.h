#ifndef __TERMINAL_PROGRAM_TRANSMISSION_H
#define __TERMINAL_PROGRAM_TRANSMISSION_H

#include "common.h"


extern uint8_t  StartCode;  //起始码
extern uint8_t  EndCode;  //结束码
extern uint8_t  TerminalID ;  //终端ID
extern uint8_t  GatewayID;  //网关ID
extern uint8_t  ControlCode ;//控制码

uint8_t TerminalProgramDataSendControl (uint32_t timeout, uint16_t package_serial_number_max, uint32_t program_start_address);
void Result_Return_4G (uint8_t functioncode, uint8_t targetcode,  uint8_t FunctionReturnCode,  uint8_t Responseder);

#endif
