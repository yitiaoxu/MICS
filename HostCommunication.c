#include "HostCommunication.h"
#include "sys.h"

// #include <stdio.h>

// #include "ansicolorconsole.h"

#include "asciiControlCode.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"


// #define FUNCTION_ENTRY_INFO(_info)	// 忽略所有FUNCTION_ENTRY_INFO
#ifndef FUNCTION_ENTRY_INFO
	#if (defined(__stdio_h) || defined(_INC_STDIO))
		#ifdef __ANSICOLORCONSOLE_H
			#define FUNCTION_ENTRY_INFO(_info)	printf(ANSI_COLOR_FG_BRIGHT_GRAY _info ANSI_COLOR_RESET)
		#else
			#define FUNCTION_ENTRY_INFO(_info)	printf(_info)
		#endif // __ANSICOLORCONSOLE_H
	#else // __stdio_h || _INC_STDIO
		#define FUNCTION_ENTRY_INFO(_info)
	#endif // __stdio_h || _INC_STDIO
#endif // FUNCTION_ENTRY_INFO

static HostCommunicationStatus_t hc_status;

/// @brief 同时改变握手状态和错误代码
/// @param next_handshake_status 送入'hc_status.handshake_status'的值
/// @param next_errorcode 送入'hc_status.errcode'的值
__STATIC_FORCEINLINE void _hc_handshake_error(uint16_t next_handshake_status, uint16_t next_errorcode)
{
	// TODO: 是否要考虑开关中断？
	hc_status.errcode          = next_errorcode;
	hc_status.handshake_status = next_handshake_status;
	return;
}

void HC_Status_Reset()
{
	hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
	hc_status.function_status = HC_FUNCTION_STATUS_STANDBY;
	hc_status.args_bytes_remain = 0;
	hc_status.data_bytes_remain = 0;
	hc_status.can_count = 0;
	hc_status.cmdbuff_idx = 0;
	hc_status.errcode = HC_ErrCode_NoError;
	hc_status.data_buff_point = hc_status.data_buff_head;
	return;
}

void HC_Status_Init(uint8_t *databuff)
{
	hc_status.data_buff_head = databuff;
	HC_Status_Reset();
	return;
}

__STATIC_FORCEINLINE void hc_HandShakeStatus_Reset()
{
	_hc_handshake_error(HC_HANDSHAKE_STATUS_IDLE, HC_ErrCode_NoError);
	return;
}

const HostCommunicationStatus_t *HC_Status()
{
	return &hc_status;
}

// bool HC_CommandWaitToExecute()
// {
// 	return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC);
// 	// return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_ACK || hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC;
// }

// 检测连续的CAN序列
bool hc_cancel_test(uint8_t ch)
{
	if (ch == ASCII_CAN)
		hc_status.can_count++;
	else
		hc_status.can_count = 0;

	#ifdef __HC_DEBUG
	if (hc_status.can_count)
		printf("第%d个CAN\r\n", hc_status.can_count);
	#endif

	return (bool)(hc_status.can_count >= HC_STATUS_RESET_BY_CAN_NUM);
}


bool HC_GotCharHandle(uint8_t ch)
{
	// TODO: 在'HC_HANDSHAKE_STATUS_WAIT_EXEC'和'HC_HANDSHAKE_STATUS_ON_ERROR'状态时不调用此函数，似乎可以避免命令等待执行和握手错误时发送ENQ行为的不同

	bool change = true;

	// 连续8次输入CAN则复位
	if (hc_cancel_test(ch))
	{
		HC_Status_Reset();
		#ifdef __HC_DEBUG
		printf("因CAN达到8次而状态复位\r\n");
		#endif // __HC_DEBUG
		return change;
	}

	switch (hc_status.handshake_status) // 根据当前状态处理握手
	{
	case HC_HANDSHAKE_STATUS_IDLE:
	{
		if (ch == ASCII_ENQ) // 仅识别ENQ
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_GET_CMD;

			// 这两个参数运行中唯一一处重置，不要轻易修改
			hc_status.cmdbuff_idx = 0;
			hc_status.bytes_remain = 0;
		}
		else // 剩下的字符都忽略
		{
			change = false;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_CMD:
	{
		if (ch == HC_CMD_FuncStatusBackToStandBy) // 特殊命令单独处理
		{
			switch (hc_status.function_status)
			{
			case HC_FUNCTION_STATUS_SAMPLING:
				ch = HC_CMD_StopSample;
				break;

			case HC_FUNCTION_STATUS_OPERATE_STORAGE:
				ch = HC_CMD_SwitchOperateStorageMode;
				break;

			case HC_FUNCTION_STATUS_STANDBY:
			default:
				ch = HC_CMD_NOP;
				break;
			}
		}

		hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // 命令存入缓存

		switch (ch) // 此处是各命令的处理
		{
		case HC_CMD_NOP:
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			break;
		case HC_CMD_INVALID0:
		case HC_CMD_INVALID1:
			_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_InvalidCmd);
			break;

		case HC_CMD_StartSample:
		case HC_CMD_StopSample:
			if ((hc_status.function_status == HC_FUNCTION_STATUS_STANDBY) ||
				(hc_status.function_status == HC_FUNCTION_STATUS_SAMPLING))
			{
				hc_status.function_status = (ch == HC_CMD_StartSample) ? HC_FUNCTION_STATUS_SAMPLING : HC_FUNCTION_STATUS_STANDBY;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
			}
			break;

		case HC_CMD_SwitchOperateStorageMode:
			switch (hc_status.function_status)
			{
			case HC_FUNCTION_STATUS_STANDBY:
				hc_status.function_status = HC_FUNCTION_STATUS_OPERATE_STORAGE;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
				break;
			case HC_FUNCTION_STATUS_OPERATE_STORAGE:
				hc_status.function_status = HC_FUNCTION_STATUS_STANDBY;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
				break;
			default:
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
				break;
			}
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGE模式下的二字节参数命令
		case HC_CMD_Specify64kBlock:
			if (hc_status.function_status == HC_FUNCTION_STATUS_OPERATE_STORAGE)
			{
				hc_status.bytes_remain = 2;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_GET_ARGS;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
			}
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGE模式下的零字节参数命令
		case HC_CMD_Output64kBlock:
		case HC_CMD_Erase64kBlock:
		case HC_CMD_EraseFullChip:
			if (hc_status.function_status == HC_FUNCTION_STATUS_OPERATE_STORAGE)
			{
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
			}
			break;

		default:
			_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_UnknownCmd);
			break;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_ARGS:
	{
		// 若参数不满则继续填充
		if (hc_status.args_bytes_remain > 0)
		{
			hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // 参数存入缓存
			--hc_status.args_bytes_remain;
		}

		// 这里两个if不能合在一起，因为参数输入完成（args_bytes_remain==0）后要立刻
		// 转移状态，不能留到下一次读字符，否则会丢失这个字符。

		// 若参数填满则进入下一个状态
		if (hc_status.args_bytes_remain == 0)
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
	{
		if (ch == ASCII_ENQ) // 仅识别ENQ
		{
			hc_status.errcode = HC_ErrCode_Busy;
		}
		else // 剩下的字符都忽略
		{
			change = false;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_ON_ERROR:
	{
		// 握手状态机在错误状态下送入新字符会直接被忽略
		// 状态也不会有变化
		change = false;
		break;
	}
	default:
	{
		_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_UnexpectedStatus);
	}
	}

	return change;
}

void HC_CommandFinishHandle()
{
	hc_HandShakeStatus_Reset();
}


// // NOTE: 协议中没有这个
// // 命令执行中出现错误后设置状态以便通知上位机
// void HC_ErrorOccuredHandle(uint16_t errcode)
// {
// 	_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, errorcode);
// 	return;
// }

void HC_ResponseCheckHandle()
{
	FUNCTION_ENTRY_INFO("HC_ResponseCheckHandle()\r\n");

	switch (hc_status.handshake_status)
	{
	// 握手完成，回复上位机ACK
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
		HC_SendACKHook(hc_status.errcode);
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		break;

	// 上一次握手完成还未执行，新的命令就发送进来，此时应回复忙错误
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		if (hc_status.errcode == HC_ErrCode_NoError)
		{
			// 这条语句会在命令未执行时运行本函数时执行，一般来说，应当在
			// 调用HC_ResponseCheckHandle()后调用HC_CheckAndExecuteHandle()
			break;
		}
		HC_SendNAKHook(hc_status.errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_status.errcode = HC_ErrCode_NoError;
		break;

	// 握手中出现错误，回复上位机NAK
	// 'HC_HANDSHAKE_STATUS_ON_ERROR'状态在'HC_ResponseCheckHandle()'中即变回'HC_HANDSHAKE_STATUS_IDLE'，
	// 因此不需要像'HC_HANDSHAKE_STATUS_WAIT_EXEC'一样使用两个状态表示
	case HC_HANDSHAKE_STATUS_ON_ERROR:
		HC_SendNAKHook(hc_status.errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_HandShakeStatus_Reset();
		break;
	}
	return;
}

void HC_TimeOutCheckHandle()
{
	FUNCTION_ENTRY_INFO("HC_TimeOutCheckHandle()\r\n");
	// TODO: 尚未完成
	return;
}

void HC_CheckAndExecuteHandle()
{
	if ((hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC))
	{
		HC_ExecuteHook(&hc_status);
	}
	return;
}

__WEAK void HC_SendACKHook(uint16_t errcode)
{
	#ifdef __HC_DEBUG
	printf("ACK\terrcode=%d\r\n", errcode);
	#endif // __HC_DEBUG
	return;
}

__WEAK void HC_SendNAKHook(uint16_t errcode)
{
	#ifdef __HC_DEBUG
	printf("NAK %#2x\terrcode=%d\r\n", errcode, errcode);
	#endif // __HC_DEBUG
	return;
}

__WEAK void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status)
{
	return;
}

__WEAK void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status)
{
	return;
}
