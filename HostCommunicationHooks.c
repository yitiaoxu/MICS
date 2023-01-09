
#include <stdio.h>

#include "ansicolorconsole.h"

#include "HostCommunication.h"
#include "HostCommunicationUtils.h"

extern void print_hc_status(void);

void HC_SendACKHook(uint16_t errcode)
{
	printf(ANSI_COLOR_FG_GREEN "ACK" ANSI_COLOR_RESET "\terrcode=%d\n", errcode);
	return;
}

void HC_SendNAKHook(uint16_t errcode)
{
	printf(ANSI_COLOR_FG_RED "NAK" ANSI_COLOR_RESET " %#02x\terrcode=%d\n", (uint8_t)errcode, errcode);
	return;
}

void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status)
{
	print_hc_status();
	printf(ANSI_COLOR_FG_RED "***Error***: %s\n" ANSI_COLOR_RESET, hc_getErrorCodeString(p_hc_status->errcode));
}

void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status)
{
	print_hc_status();
	int cmdcode = p_hc_status->cmdbuff[0];
	printf(ANSI_COLOR_FG_GREEN "Command: %s(%d)\n" ANSI_COLOR_RESET, hc_getCommandString(cmdcode), cmdcode);
	HC_CommandFinishHandle();
	return;
}
