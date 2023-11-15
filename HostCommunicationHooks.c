
#include <stdio.h>

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
	printf(ANSI_COLOR_FG_GREEN "Command: %s(%d)", hc_getCommandString(cmdcode), cmdcode);

	int datalength;
	switch (cmdcode)
	{
	case HC_CMD_WriteFlashAtAddr:
		datalength = ((const HC_CMD_WriteFlashAtAddr_Args_t *)p_hc_status->cmdbuff)->length_m1 + 1 + 2;
		break;
	default:
		datalength = 0;
		break;
	}
	if (datalength)
	{
		printf("\tData: [");
		print_array_by_byte(p_hc_status->data_buff_head, datalength);
		printf("]");
	}
	printf("\r\n" ANSI_COLOR_RESET, hc_getCommandString(cmdcode), cmdcode);

	HC_CommandFinishHandle();
	return;
}
