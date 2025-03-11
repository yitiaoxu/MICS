#ifndef __HOSTCOMMUNICATIONCOMMAND_H
#define __HOSTCOMMUNICATIONCOMMAND_H

#ifdef __ARM_ARCH
    #include "main.h"
#else
    #include "assist_cmsis.h"
#endif

#include "HostCommunicationCommandCode.h"

typedef __PACKED_STRUCT _HC_CMD_Specify64kBlock_Args_t
{
	uint8_t cmd;			// 命令码
	uint16_t blockno;		// 块号
} HC_Cmd_Args_Specify64kBlock_t;

typedef __PACKED_STRUCT _HC_CMD_Specify4kSector_Args_t
{
	uint8_t cmd;			// 命令码
	uint16_t sectorno;		// 扇区号
} HC_Cmd_Args_Specify4kSector_t;

typedef __PACKED_STRUCT _HC_CMD_WriteFlashAtAddr_Args_t
{
	uint8_t cmd;			// 命令码
	uint32_t addr;			// 地址
	uint16_t length_m1;		// 长度减一
} HC_CMD_WriteFlashAtAddr_Args_t;

typedef __PACKED_STRUCT _HC_CMD_SetRealTimeClock_Args_t
{
	uint8_t cmd;			// 命令码
	uint32_t timestamp;		// UNIX时间戳
} HC_CMD_SetRealTimeClock_Args_t;

#endif // __HOSTCOMMUNICATIONCOMMAND_H
