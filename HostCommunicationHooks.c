
#include <stdio.h>
#include <time.h>

#include "ansicolorconsole.h"

#include "HostCommunication.h"
#include "HostCommunicationCommand.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationUtils.h"

void HC_SendACKHook(uint16_t handshake_errcode)
{
	printf(ANSI_COLOR_FG_GREEN "ACK" ANSI_COLOR_RESET "\terrcode=%d\r\n", handshake_errcode);
	return;
}

void HC_SendNAKHook(uint16_t handshake_errcode)
{
	printf(ANSI_COLOR_FG_RED "NAK" ANSI_COLOR_RESET " %#02x\terrcode=%d\r\n", (uint8_t)handshake_errcode, handshake_errcode);
	return;
}

void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status)
{
	print_hc_status();
	printf(ANSI_COLOR_FG_RED "***Error***: %s\r\n" ANSI_COLOR_RESET, hc_getErrorCodeString(p_hc_status->handshake_errcode));
}

void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status)
{
	print_hc_status();
	int cmdcode = p_hc_status->cmdbuff[0];
	printf(ANSI_COLOR_FG_GREEN "Command: " ANSI_COLOR_RESET "%s(%d)", hc_getCommandString(cmdcode), cmdcode);

	int datalength = 0;
	switch (cmdcode)
	{
	// case HC_CMD_Specify64kBlock:
	// 	break;
	case HC_CMD_WriteFlashAtAddr:
	{
		const HC_CMD_WriteFlashAtAddr_Args_t *const p_cmd = (const HC_CMD_WriteFlashAtAddr_Args_t *)(p_hc_status->cmdbuff);
		datalength = p_cmd->length_m1 + 1 + 2;
		printf("\t" ANSI_COLOR_FG_GREEN "Args: " ANSI_COLOR_RESET "[%#x, %d]", p_cmd->addr, p_cmd->length_m1);
		break;
	}
	default:
	{
		break;
	}
	}
	if (datalength)
	{
		printf("\t" ANSI_COLOR_FG_GREEN "Data: " ANSI_COLOR_RESET "[");
		print_array_by_byte(p_hc_status->data_buff_head, datalength);
		printf("]");
	}
	printf("\r\n");

	HC_CommandFinishHandle();
	return;
}

uint32_t HC_GetTimeHook()
{
	return clock();
}

bool HC_TimeoutHook(uint32_t tic, uint32_t toc)
{
	if (tic > toc)
	{
		return true;
	}
	else if ((toc - tic) > CLOCKS_PER_SEC * 10)
	{
		return true;
	}
	return false;
}
