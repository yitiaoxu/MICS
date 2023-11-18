#include "HostCommunicationUtils.h"

#include <stdio.h>
#include <string.h>

#include "asciiControlCode.h"
#include "ansicolorconsole.h"
#include "HostCommunication.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"

// #define STR(x) #x
#define ENUM_COLOR_STR(x) ANSI_COLOR_FG_BLUE #x ANSI_COLOR_RESET

const char *hc_getHandshakeStatusString(int status)
{
	switch (status)
	{
	case HC_HANDSHAKE_STATUS_IDLE:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_IDLE);
	case HC_HANDSHAKE_STATUS_GET_CMD:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_GET_CMD);
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_WAIT_ACK);
	case HC_HANDSHAKE_STATUS_GET_ARGS:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_GET_ARGS);
	case HC_HANDSHAKE_STATUS_WAIT_DATA:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_WAIT_DATA);
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_WAIT_EXEC);
	case HC_HANDSHAKE_STATUS_ON_ERROR:
		return ENUM_COLOR_STR(HC_HANDSHAKE_STATUS_ON_ERROR);
	default:
		return (ANSI_COLOR_FG_RED "<InvalidHandshakeStatusCode>" ANSI_COLOR_RESET);
	}
}

const char *hc_getFunctionStatusString(int status)
{
	switch (status)
	{
	case HC_FUNCTION_STATUS_STANDBY:
		return ENUM_COLOR_STR(HC_FUNCTION_STATUS_STANDBY);
	case HC_FUNCTION_STATUS_SAMPLING:
		return ENUM_COLOR_STR(HC_FUNCTION_STATUS_SAMPLING);
	case HC_FUNCTION_STATUS_OPERATE_STORAGE:
		return ENUM_COLOR_STR(HC_FUNCTION_STATUS_OPERATE_STORAGE);
	default:
		return (ANSI_COLOR_FG_RED "<InvalidHandshakeStatusCode>" ANSI_COLOR_RESET);
	}
}

const char *hc_getErrorCodeString(int errcode)
{
	switch (errcode)
	{
	case HC_ErrCode_NoError:
		return ENUM_COLOR_STR(HC_ErrCode_NoError);
	case HC_ErrCode_UnknownError:
		return ENUM_COLOR_STR(HC_ErrCode_UnknownError);
	case HC_ErrCode_UnexpectedStatus:
		return ENUM_COLOR_STR(HC_ErrCode_UnexpectedStatus);
	case HC_ErrCode_TimeOut:
		return ENUM_COLOR_STR(HC_ErrCode_TimeOut);
	case HC_ErrCode_ProtocolMismatch:
		return ENUM_COLOR_STR(HC_ErrCode_ProtocolMismatch);
	case HC_ErrCode_Busy:
		return ENUM_COLOR_STR(HC_ErrCode_Busy);
	case HC_ErrCode_UnknownCmd:
		return ENUM_COLOR_STR(HC_ErrCode_UnknownCmd);
	case HC_ErrCode_InvalidCmd:
		return ENUM_COLOR_STR(HC_ErrCode_InvalidCmd);
	case HC_ErrCode_ModeError:
		return ENUM_COLOR_STR(HC_ErrCode_ModeError);
	default:
		return (ANSI_COLOR_FG_RED "<InvalidErrCode>" ANSI_COLOR_RESET);
	}
}

const char *hc_getCommandString(int cmd)
{
	switch (cmd)
	{
	case HC_CMD_NOP:
		return ENUM_COLOR_STR(HC_CMD_NOP);
	case HC_CMD_INVALID0:
		return ENUM_COLOR_STR(HC_CMD_INVALID0);
	case HC_CMD_INVALID1:
		return ENUM_COLOR_STR(HC_CMD_INVALID1);

	case HC_CMD_FuncStatusBackToStandBy:
		return ENUM_COLOR_STR(HC_CMD_FuncStatusBackToStandBy);

	case HC_CMD_StartSample:
		return ENUM_COLOR_STR(HC_CMD_StartSample);
	case HC_CMD_StopSample:
		return ENUM_COLOR_STR(HC_CMD_StopSample);

	case HC_CMD_SwitchOperateStorageMode:
		return ENUM_COLOR_STR(HC_CMD_SwitchOperateStorageMode);
	case HC_CMD_Specify64kBlock:
		return ENUM_COLOR_STR(HC_CMD_Specify64kBlock);
	case HC_CMD_Output64kBlock:
		return ENUM_COLOR_STR(HC_CMD_Output64kBlock);
	case HC_CMD_Erase64kBlock:
		return ENUM_COLOR_STR(HC_CMD_Erase64kBlock);
	case HC_CMD_Specify4kSector:
		return ENUM_COLOR_STR(HC_CMD_Specify4kSector);
	case HC_CMD_Output4kSector:
		return ENUM_COLOR_STR(HC_CMD_Output4kSector);
	case HC_CMD_Erase4kSector:
		return ENUM_COLOR_STR(HC_CMD_Erase4kSector);
	case HC_CMD_AskLastWriteResult:
		return ENUM_COLOR_STR(HC_CMD_AskLastWriteResult);
	case HC_CMD_AskFlashStatus:
		return ENUM_COLOR_STR(HC_CMD_AskFlashStatus);
	case HC_CMD_WriteFlashAtAddr:
		return ENUM_COLOR_STR(HC_CMD_WriteFlashAtAddr);
	case HC_CMD_EraseFullChip:
		return ENUM_COLOR_STR(HC_CMD_EraseFullChip);

	default:
		return (ANSI_COLOR_FG_RED "<InvalidCommandCode>" ANSI_COLOR_RESET);
	}
}

char *HostCommunicationStatus2str(char *const buffer, const HostCommunicationStatus_t *const p_hc_status)
{
	sprintf(
		buffer,
		ANSI_COLOR_FG_MAGENTA "HostCommunicationStatus" ANSI_COLOR_RESET "(handshake_status = %s(%d), function_status = %s(%d), "
		"args_bytes_remain = %d, data_bytes_remain = %d, can_count = %d, cmdbuff_idx = %d, databuff_idx = %d, "
		"handshake_errcode = %s(%#x), datarecv_errcode = %s(%#x), "
		"tic = %u), toc = %u",
		hc_getHandshakeStatusString(p_hc_status->handshake_status), p_hc_status->handshake_status,
		hc_getFunctionStatusString(p_hc_status->function_status), p_hc_status->function_status,
		p_hc_status->args_bytes_remain, p_hc_status->data_bytes_remain, p_hc_status->can_count,
		p_hc_status->cmdbuff_idx, p_hc_status->data_buff_point - p_hc_status->data_buff_head,
		hc_getErrorCodeString(p_hc_status->handshake_errcode), p_hc_status->handshake_errcode,
		hc_getErrorCodeString(p_hc_status->datarecv_errcode), p_hc_status->datarecv_errcode,
		p_hc_status->wait_tic, HC_GetTimeHook()
	);
	return buffer;
}


void print_array_by_byte(const void *arr, unsigned int len)
{
	for (const unsigned char *p = (const unsigned char *)arr; p < (const unsigned char *)arr + len; p++)
	{
		printf("%02x ", *p);
	}
}

void print_hc_status()
{
	char strbuffer[512];
	const HostCommunicationStatus_t *const p_hc_status = HC_Status();
	printf("%s\r\n", HostCommunicationStatus2str(strbuffer, p_hc_status));
	printf(ANSI_COLOR_FG_BRIGHT_BLACK);
	print_array_by_byte((void *)(p_hc_status->cmdbuff), HC_CMDBUFF_SIZE);
	printf(ANSI_COLOR_RESET "\r\n");
}
