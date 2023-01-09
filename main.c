#include <stdint.h>
#include <stdio.h>

// #include <errno.h>
// #include <Windows.h>

#include "ansicolorconsole.h"

#include "HostCommunication.h"
#include "HostCommunicationUtils.h"


int main()
{
	bool exitflag = false;
	HostCommunicationStatus_t *const p_hc_status = (HostCommunicationStatus_t *)HC_Status();
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
			bool change;
			change = HC_GotCharHandle(ch);
			// printf("change = %d\n", change);
		}
		else if (ch == 810)
		{
			if ((p_hc_status->handshake_status == HC_HANDSHAKE_STATUS_WAIT_ACK) ||
				(p_hc_status->handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC) ||
				(p_hc_status->handshake_status == HC_HANDSHAKE_STATUS_ON_ERROR))
				print_hc_status();
			HC_ResponseCheckHandle();
		}
		else if (ch == 811)
		{
			HC_CheckAndExecuteHandle();
			HC_TimeOutCheckHandle();
		}
		else if (ch == 1919)
		{
			exitflag = true;
		}
		else
		{
			printf(ANSI_COLOR_FG_YELLOW "Not a valid char: " ANSI_COLOR_RESET "%d\n", ch);
		}
		printf("\n");
	} while (!exitflag);

	return 0;
}
