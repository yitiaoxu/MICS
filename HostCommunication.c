#include "HostCommunication.h"
#include "sys.h"

// #include <stdio.h>

// #include "ansicolorconsole.h"

#include "asciiControlCode.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"


// #define FUNCTION_ENTRY_INFO(_info)	// ��������FUNCTION_ENTRY_INFO
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

/// @brief ͬʱ�ı�����״̬�ʹ������
/// @param next_handshake_status ����'hc_status.handshake_status'��ֵ
/// @param next_errorcode ����'hc_status.errcode'��ֵ
__STATIC_FORCEINLINE void _hc_handshake_error(uint16_t next_handshake_status, uint16_t next_errorcode)
{
	// TODO: �Ƿ�Ҫ���ǿ����жϣ�
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

			// ����������������Ψһһ�����ã���Ҫ�����޸�
			hc_status.cmdbuff_idx = 0;
			hc_status.bytes_remain = 0;
		}
		else // ʣ�µ��ַ�������
		{
			change = false;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_CMD:
	{
		if (ch == HC_CMD_FuncStatusBackToStandBy) // �������������
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

		hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // ������뻺��

		switch (ch) // �˴��Ǹ�����Ĵ���
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
		// HC_FUNCTION_STATUS_OPERATE_STORAGEģʽ�µ����ֽڲ�������
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
			hc_status.errcode = HC_ErrCode_Busy;
		}
		else // ʣ�µ��ַ�������
		{
			change = false;
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
	// ������ɣ��ظ���λ��ACK
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
		HC_SendACKHook(hc_status.errcode);
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		break;

	// ��һ��������ɻ�δִ�У��µ�����ͷ��ͽ�������ʱӦ�ظ�æ����
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		if (hc_status.errcode == HC_ErrCode_NoError)
		{
			// ��������������δִ��ʱ���б�����ʱִ�У�һ����˵��Ӧ����
			// ����HC_ResponseCheckHandle()�����HC_CheckAndExecuteHandle()
			break;
		}
		HC_SendNAKHook(hc_status.errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_status.errcode = HC_ErrCode_NoError;
		break;

	// �����г��ִ��󣬻ظ���λ��NAK
	// 'HC_HANDSHAKE_STATUS_ON_ERROR'״̬��'HC_ResponseCheckHandle()'�м����'HC_HANDSHAKE_STATUS_IDLE'��
	// ��˲���Ҫ��'HC_HANDSHAKE_STATUS_WAIT_EXEC'һ��ʹ������״̬��ʾ
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
