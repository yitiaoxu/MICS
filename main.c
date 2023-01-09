#include <stdint.h>
#include <stdio.h>

#include <errno.h>
#include <Windows.h>

#include "ansicolorconsole.h"

#include "HostCommunication.h"
#include "HostCommunicationUtils.h"


static void print_array_by_byte(const void *arr, unsigned int len)
{
	for (const unsigned char *p = (const unsigned char *)arr; p < (const unsigned char *)arr + len; p++)
	{
		printf("%02x ", *p);
	}
}

void print_hc_status()
{
	char strbuffer[512];
	HostCommunicationStatus_t *const p_hc_status = (HostCommunicationStatus_t *)HC_Status();
	printf("%s\n", HostCommunicationStatus2str(strbuffer, p_hc_status));
	printf(ANSI_COLOR_FG_BRIGHT_BLACK);
	print_array_by_byte(p_hc_status->cmdbuff, HC_CMDBUFF_SIZE);
	printf(ANSI_COLOR_RESET "\n");
}

int main()
{
	bool exitflag = false;
	// HostCommunicationStatus_t *const p_hc_status = (HostCommunicationStatus_t *)HC_Status();
	uint16_t ch;
	// char strbuffer[512];

	// SetConsoleTextAttribute
	// https://learn.microsoft.com/zh-cn/windows/console/setconsoletextattribute
	// printf("\x1b[31m" "Color Char Test\n" "\x1b[0m");

	HC_Status_Init();

	do
	{
		// printf("p_hc_status = %#llx, HC_Status() = %#llx\r\n", (uintptr_t)p_hc_status, (uintptr_t)HC_Status());
		print_hc_status();
		scanf("%hu", &ch);
		// printf("Got: %d",ch);
		if (ch < UINT8_MAX)
		{
			HC_GotCharHandle(ch);
			HC_ResponseCheckHandle();
			HC_CheckAndExecuteHandle();
			HC_TimeOutCheckHandle();
		}
		else
		{
			exitflag = true;
		}
		printf("\n");
	} while (!exitflag);

	return 0;
}

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
	char strbuffer[512];
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
