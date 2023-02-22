#ifndef __HOSTCOMMUNICATIONCOMMAND_H
#define __HOSTCOMMUNICATIONCOMMAND_H

#include "sys.h"

#include "HostCommunicationCommandCode.h"

typedef __PACKED_STRUCT _HC_CMD_Specify64kBlock_Args_t
{
	uint8_t cmd;
	uint16_t arg1;
} HC_Cmd_Args_Specify64kBlock_t;


#endif // __HOSTCOMMUNICATIONCOMMAND_H
