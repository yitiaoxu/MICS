#include "HostCommunication.h"
#include "sys.h"

#ifdef __HC_DEBUG
#include <stdio.h>
#include "ansicolorconsole.h"
#endif
// #include <stdio.h>
// #include <assert.h>

#include "asciiControlCode.h"
#include "HostCommunicationCommand.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"


// #define FUNCTION_ENTRY_INFO(_info)	// 忽略所有FUNCTION_ENTRY_INFO
#ifndef FUNCTION_ENTRY_INFO
	#if (defined(__stdio_h) || defined(_STDIO_H) || defined(_INC_STDIO))
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
/// @param next_errorcode 送入'hc_status.handshake_errcode'的值
__STATIC_FORCEINLINE void _hc_handshake_error(uint16_t next_handshake_status, uint16_t next_errorcode)
{
	// TODO: 是否要考虑开关中断？
	hc_status.handshake_errcode = next_errorcode;
	hc_status.handshake_status = next_handshake_status;
	return;
}

/// @brief 获取当前命令代码
/// @return 命令代码
/// @note 当前的设计中命令代码只有1个字节
/// @attention 需要考虑有效性，不总是有效
__STATIC_FORCEINLINE uint8_t _hc_get_cmd()
{
	return hc_status.cmdbuff[0];
}

/// @brief 检测当前无参数指令需要的状态是否合法
/// @param function_status 要求的功能状态
static void _hc_no_arg_command_at_function_status(uint16_t function_status)
{
	if (hc_status.function_status == function_status)
	{
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
	}
	else
	{
		_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
	}
	return;
}

/// @brief 检测当前带参数指令需要的状态是否合法，并设置参数需要的字节数
/// @param function_status 要求的功能状态
/// @param args_bytes 参数字节数
static void _hc_nbytes_args_command_at_function_status(uint16_t function_status, uint16_t args_bytes)
{
	if (hc_status.function_status == function_status)
	{
		hc_status.args_bytes_remain = args_bytes;
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_GET_ARGS;
	}
	else
	{
		_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
	}
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
	hc_status.handshake_errcode = HC_ErrCode_NoError;
	hc_status.datarecv_errcode = HC_ErrCode_NoError;
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

			// 这几个参数运行中唯一一处重置，不要轻易修改
			hc_status.cmdbuff_idx = 0;
			hc_status.args_bytes_remain = 0;
			hc_status.data_bytes_remain = 0;
			hc_status.data_buff_point = hc_status.data_buff_head;
		}
		else // 剩下的字符都忽略
		{
			change = false;
			#ifdef __HC_DEBUG
			printf("字符'%c'被忽略\r\n", ch);
			#endif
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_CMD:
	{
		// 以下代码仅考虑了命令长度为1字节、即本函数运行后GET_CMD状态必定跳转，因此均用ch指代命令

		// 特殊命令单独处理
		if (ch == HC_CMD_FuncStatusBackToStandBy)
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
		// assert(hc_status.cmdbuff_idx == 0);
		hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // 命令存入缓存
		// printf("<%d, %d, %d>\r\n", hc_status.cmdbuff_idx - 1, ch, _hc_get_cmd());

		// 根据命令值修改状态机状态
		// NOTE: 这里貌似有个问题，如果命令多余一个字节，这样写可能会出错
		switch (ch)
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
			_hc_nbytes_args_command_at_function_status(HC_FUNCTION_STATUS_OPERATE_STORAGE, 2);
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGE模式下的六字节参数命令
		case HC_CMD_WriteFlashAtAddr:
			_hc_nbytes_args_command_at_function_status(HC_FUNCTION_STATUS_OPERATE_STORAGE, 6);
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGE模式下的零字节参数命令
		case HC_CMD_Output64kBlock:
		case HC_CMD_Erase64kBlock:
		case HC_CMD_EraseFullChip:
		case HC_CMD_AskLastWriteResult:
		case HC_CMD_AskFlashStatus:
			_hc_no_arg_command_at_function_status(HC_FUNCTION_STATUS_OPERATE_STORAGE);
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
			hc_status.handshake_errcode = HC_ErrCode_Busy;
		}
		else // 剩下的字符都忽略
		{
			change = false;
			#ifdef __HC_DEBUG
			printf("字符'%c'被忽略\r\n", ch);
			#endif
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_WAIT_DATA:
	{
		// 若数据不满则继续填充
		if (hc_status.data_bytes_remain > 0)
		{
			*hc_status.data_buff_point++ = ch; // 数据存入缓存
			--hc_status.data_bytes_remain;
		}

		// 这里两个if不能合在一起，因为参数输入完成（data_bytes_remain==0）后要立刻
		// 转移状态，不能留到下一次读字符，否则会丢失这个字符。

		// 若数据填满则进入下一个状态
		if (hc_status.data_bytes_remain == 0)
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
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
// void HC_ErrorOccuredHandle(uint16_t handshake_errcode)
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
		HC_SendACKHook(hc_status.handshake_errcode);
		// 在这里转换状态
		switch (_hc_get_cmd())
		{
		case HC_CMD_WriteFlashAtAddr:
		{
			const HC_CMD_WriteFlashAtAddr_Args_t *const p_cmd = (const HC_CMD_WriteFlashAtAddr_Args_t *)&hc_status.cmdbuff;
			uint16_t length = p_cmd->length_m1 + 1;
			// printf("\tArgs: [%#x, %d]", p_cmd->addr, p_cmd->length_m1);
			hc_status.data_bytes_remain = length + 2;	// STX + data[] + ETX
			break;
		}
		default:
			break;
		}
		// 决定下一个状态
		if (hc_status.data_bytes_remain > 0)
		{	// 有数据需要接收
			hc_status.handshake_errcode = HC_ErrCode_NoError;			// NOTE: 仅当有新的数据接收命令时清空错误状态
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_DATA;
		}
		else
		{	// 无数据需要接收
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		}
		break;

	case HC_HANDSHAKE_STATUS_WAIT_DATA:
		// HC_ResponseCheckHandle()设计上会被主循环调用，并且串口中断时HC_GotCharHandle()永远不会被调用，
		// 因此超时判定放在此处比较合适

		// TODO: 超时判定
		break;

	// 上一次握手完成还未执行，新的命令就发送进来，此时应回复忙错误
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		if (hc_status.handshake_errcode == HC_ErrCode_NoError)
		{
			// 这条语句会在命令未执行时运行本函数时执行，一般来说，应当在
			// 调用HC_ResponseCheckHandle()后调用HC_CheckAndExecuteHandle()
			break;
		}
		HC_SendNAKHook(hc_status.handshake_errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_status.handshake_errcode = HC_ErrCode_NoError;
		break;

	// 握手中出现错误，回复上位机NAK
	// 'HC_HANDSHAKE_STATUS_ON_ERROR'状态在'HC_ResponseCheckHandle()'中即变回'HC_HANDSHAKE_STATUS_IDLE'，
	// 因此不需要像'HC_HANDSHAKE_STATUS_WAIT_EXEC'一样使用两个状态表示
	case HC_HANDSHAKE_STATUS_ON_ERROR:
		HC_SendNAKHook(hc_status.handshake_errcode);
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

__WEAK void HC_SendACKHook(uint16_t handshake_errcode)
{
	#ifdef __HC_DEBUG
	printf("ACK\terrcode=%d\r\n", handshake_errcode);
	#endif // __HC_DEBUG
	return;
}

__WEAK void HC_SendNAKHook(uint16_t handshake_errcode)
{
	#ifdef __HC_DEBUG
	printf("NAK %#2x\terrcode=%d\r\n", handshake_errcode, handshake_errcode);
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
