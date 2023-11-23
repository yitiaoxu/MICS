#ifndef __HOSTCOMMUNICATIONCOMMAND_H
#define __HOSTCOMMUNICATIONCOMMAND_H

#include "assist_cmsis.h"

#include "HostCommunicationCommandCode.h"

typedef __PACKED_STRUCT _HC_CMD_Specify64kBlock_Args_t
{
	uint8_t cmd;
	uint16_t length_m1;
} HC_Cmd_Args_Specify64kBlock_t;

typedef __PACKED_STRUCT _HC_CMD_WriteFlashAtAddr_Args_t
{
	uint8_t cmd;
	uint32_t addr;
	uint16_t length_m1;
} HC_CMD_WriteFlashAtAddr_Args_t;

#endif // __HOSTCOMMUNICATIONCOMMAND_H
