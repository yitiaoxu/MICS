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


// #define FUNCTION_ENTRY_INFO(_info)	// ��������FUNCTION_ENTRY_INFO
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

/// @brief ͬʱ�ı�����״̬�ʹ������
/// @param next_handshake_status ����'hc_status.handshake_status'��ֵ
/// @param next_errorcode ����'hc_status.handshake_errcode'��ֵ
__STATIC_FORCEINLINE void _hc_handshake_error(uint16_t next_handshake_status, uint16_t next_errorcode)
{
	// TODO: �Ƿ�Ҫ���ǿ����жϣ�
	hc_status.handshake_errcode = next_errorcode;
	hc_status.handshake_status = next_handshake_status;
	return;
}

/// @brief ��ȡ��ǰ�������
/// @return �������
/// @note ��ǰ��������������ֻ��1���ֽ�
/// @attention ��Ҫ������Ч�ԣ���������Ч
__STATIC_FORCEINLINE uint8_t _hc_get_cmd()
{
	return hc_status.cmdbuff[0];
}

/// @brief ��⵱ǰ�޲���ָ����Ҫ��״̬�Ƿ�Ϸ�
/// @param function_status Ҫ��Ĺ���״̬
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

/// @brief ��⵱ǰ������ָ����Ҫ��״̬�Ƿ�Ϸ��������ò�����Ҫ���ֽ���
/// @param function_status Ҫ��Ĺ���״̬
/// @param args_bytes �����ֽ���
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

// ���������CAN����
bool hc_cancel_test(uint8_t ch)
{
	if (ch == ASCII_CAN)
		hc_status.can_count++;
	else
		hc_status.can_count = 0;

	#ifdef __HC_DEBUG
	if (hc_status.can_count)
		printf("��%d��CAN\r\n", hc_status.can_count);
	#endif

	return (bool)(hc_status.can_count >= HC_STATUS_RESET_BY_CAN_NUM);
}


bool HC_GotCharHandle(uint8_t ch)
{
	// TODO: ��'HC_HANDSHAKE_STATUS_WAIT_EXEC'��'HC_HANDSHAKE_STATUS_ON_ERROR'״̬ʱ�����ô˺������ƺ����Ա�������ȴ�ִ�к����ִ���ʱ����ENQ��Ϊ�Ĳ�ͬ

	bool change = true;

	// ����8������CAN��λ
	if (hc_cancel_test(ch))
	{
		HC_Status_Reset();
		#ifdef __HC_DEBUG
		printf("��CAN�ﵽ8�ζ�״̬��λ\r\n");
		#endif // __HC_DEBUG
		return change;
	}

	switch (hc_status.handshake_status) // ���ݵ�ǰ״̬��������
	{
	case HC_HANDSHAKE_STATUS_IDLE:
	{
		if (ch == ASCII_ENQ) // ��ʶ��ENQ
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_GET_CMD;

			// �⼸������������Ψһһ�����ã���Ҫ�����޸�
			hc_status.cmdbuff_idx = 0;
			hc_status.args_bytes_remain = 0;
			hc_status.data_bytes_remain = 0;
			hc_status.data_buff_point = hc_status.data_buff_head;
		}
		else // ʣ�µ��ַ�������
		{
			change = false;
			#ifdef __HC_DEBUG
			printf("�ַ�'%c'������\r\n", ch);
			#endif
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_CMD:
	{
		// ���´���������������Ϊ1�ֽڡ������������к�GET_CMD״̬�ض���ת����˾���chָ������

		// �������������
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
		hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // ������뻺��
		// printf("<%d, %d, %d>\r\n", hc_status.cmdbuff_idx - 1, ch, _hc_get_cmd());

		// ��������ֵ�޸�״̬��״̬
		// NOTE: ����ò���и����⣬����������һ���ֽڣ�����д���ܻ����
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
		// HC_FUNCTION_STATUS_OPERATE_STORAGEģʽ�µĶ��ֽڲ�������
		case HC_CMD_Specify64kBlock:
			_hc_nbytes_args_command_at_function_status(HC_FUNCTION_STATUS_OPERATE_STORAGE, 2);
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGEģʽ�µ����ֽڲ�������
		case HC_CMD_WriteFlashAtAddr:
			_hc_nbytes_args_command_at_function_status(HC_FUNCTION_STATUS_OPERATE_STORAGE, 6);
			break;
		// HC_FUNCTION_STATUS_OPERATE_STORAGEģʽ�µ����ֽڲ�������
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
		// ������������������
		if (hc_status.args_bytes_remain > 0)
		{
			hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // �������뻺��
			--hc_status.args_bytes_remain;
		}

		// ��������if���ܺ���һ����Ϊ����������ɣ�args_bytes_remain==0����Ҫ����
		// ת��״̬������������һ�ζ��ַ�������ᶪʧ����ַ���

		// �����������������һ��״̬
		if (hc_status.args_bytes_remain == 0)
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
	{
		if (ch == ASCII_ENQ) // ��ʶ��ENQ
		{
			hc_status.handshake_errcode = HC_ErrCode_Busy;
		}
		else // ʣ�µ��ַ�������
		{
			change = false;
			#ifdef __HC_DEBUG
			printf("�ַ�'%c'������\r\n", ch);
			#endif
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_WAIT_DATA:
	{
		// �����ݲ�����������
		if (hc_status.data_bytes_remain > 0)
		{
			*hc_status.data_buff_point++ = ch; // ���ݴ��뻺��
			--hc_status.data_bytes_remain;
		}

		// ��������if���ܺ���һ����Ϊ����������ɣ�data_bytes_remain==0����Ҫ����
		// ת��״̬������������һ�ζ��ַ�������ᶪʧ����ַ���

		// �����������������һ��״̬
		if (hc_status.data_bytes_remain == 0)
		{
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_ON_ERROR:
	{
		// ����״̬���ڴ���״̬���������ַ���ֱ�ӱ�����
		// ״̬Ҳ�����б仯
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


// // NOTE: Э����û�����
// // ����ִ���г��ִ��������״̬�Ա�֪ͨ��λ��
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
	// ������ɣ��ظ���λ��ACK
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
		HC_SendACKHook(hc_status.handshake_errcode);
		// ������ת��״̬
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
		// ������һ��״̬
		if (hc_status.data_bytes_remain > 0)
		{	// ��������Ҫ����
			hc_status.handshake_errcode = HC_ErrCode_NoError;			// NOTE: �������µ����ݽ�������ʱ��մ���״̬
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_DATA;
		}
		else
		{	// ��������Ҫ����
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		}
		break;

	case HC_HANDSHAKE_STATUS_WAIT_DATA:
		// HC_ResponseCheckHandle()����ϻᱻ��ѭ�����ã����Ҵ����ж�ʱHC_GotCharHandle()��Զ���ᱻ���ã�
		// ��˳�ʱ�ж����ڴ˴��ȽϺ���

		// TODO: ��ʱ�ж�
		break;

	// ��һ��������ɻ�δִ�У��µ�����ͷ��ͽ�������ʱӦ�ظ�æ����
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		if (hc_status.handshake_errcode == HC_ErrCode_NoError)
		{
			// ��������������δִ��ʱ���б�����ʱִ�У�һ����˵��Ӧ����
			// ����HC_ResponseCheckHandle()�����HC_CheckAndExecuteHandle()
			break;
		}
		HC_SendNAKHook(hc_status.handshake_errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_status.handshake_errcode = HC_ErrCode_NoError;
		break;

	// �����г��ִ��󣬻ظ���λ��NAK
	// 'HC_HANDSHAKE_STATUS_ON_ERROR'״̬��'HC_ResponseCheckHandle()'�м����'HC_HANDSHAKE_STATUS_IDLE'��
	// ��˲���Ҫ��'HC_HANDSHAKE_STATUS_WAIT_EXEC'һ��ʹ������״̬��ʾ
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
	// TODO: ��δ���
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
