
#include <time.h>
#include <stdio.h>

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || (defined(__cplusplus) && (__cplusplus >= 201103L))
	#include <assert.h>
#else
	#define static_assert(expr, msg)
#endif

#include "ansicolorconsole.h"

#include "HostCommunication.h"
#include "HostCommunicationCommand.h"
#include "HostCommunicationUtils.h"

// NOTE: 本文件仅供说明，移植后根据目标平台需求修改


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
		case HC_CMD_NOP:
		case HC_CMD_INVALID0:
		case HC_CMD_INVALID1:
		case HC_CMD_StartSample:
		case HC_CMD_StopSample:
		case HC_CMD_SwitchOperateStorageMode:
		{
			break;
		}
		case HC_CMD_Specify64kBlock:
		{
			static_assert(sizeof(HC_Cmd_Args_Specify64kBlock_t) <= sizeof(p_hc_status->cmdbuff), "'HC_Cmd_Args_Specify64kBlock_t' is too large");
			const HC_Cmd_Args_Specify64kBlock_t *const p_cmd = (HC_Cmd_Args_Specify64kBlock_t *)p_hc_status->cmdbuff;
			printf("\t" ANSI_COLOR_FG_GREEN "Command: %x " ANSI_COLOR_RESET "Sector No.: %#x\r\n", p_cmd->cmd, p_cmd->blockno);
			break;
		}
		case HC_CMD_Specify4kSector:
		{
			static_assert(sizeof(HC_Cmd_Args_Specify4kSector_t) <= sizeof(p_hc_status->cmdbuff), "'HC_Cmd_Args_Specify4kSector_t' is too large");
			const HC_Cmd_Args_Specify4kSector_t *const p_cmd = (HC_Cmd_Args_Specify4kSector_t *)p_hc_status->cmdbuff;
			printf("\t" ANSI_COLOR_FG_GREEN "Command: %x " ANSI_COLOR_RESET "Sector No.: %#x\r\n", p_cmd->cmd, p_cmd->sectorno);
			break;
		}
		case HC_CMD_WriteFlashAtAddr:
		{
			static_assert(sizeof(HC_CMD_WriteFlashAtAddr_Args_t) <= sizeof(p_hc_status->cmdbuff), "'HC_CMD_WriteFlashAtAddr_Args_t' is too large");
			const HC_CMD_WriteFlashAtAddr_Args_t *const p_cmd = (HC_CMD_WriteFlashAtAddr_Args_t *)p_hc_status->cmdbuff;
			datalength = p_cmd->length_m1 + 1;
			printf("\t" ANSI_COLOR_FG_GREEN "Command: %x " ANSI_COLOR_RESET "Addr: %#x, Length: %d\r\n", p_cmd->cmd, p_cmd->addr, datalength);
			break;
		}
		case HC_CMD_AskLastWriteResult:
		case HC_CMD_AskFlashStatus:
		case HC_CMD_AskFlashDeviceID:
		case HC_CMD_Output64kBlock:
		case HC_CMD_Erase64kBlock:
		case HC_CMD_Erase4kSector:
		case HC_CMD_EraseFullChip:
		{
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
		print_array_by_byte(p_hc_status->data_buff_head, datalength + 2);
		printf("]");
	}
	printf("\r\n");

	HC_CommandFinishHandle();	// NOTE: 命令执行完后才调用
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
	else if ((toc - tic) > HC_STATUS_RESET_BY_IDLE)
	{
		return true;
	}
	return false;
}
