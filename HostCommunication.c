#include "HostCommunication.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ansicolorconsole.h"

#include "asciiControlCode.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"


#ifndef   __WEAK
	#define __WEAK                                 __attribute__((weak))
#endif
#ifndef   __STATIC_FORCEINLINE
	#define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static __inline
#endif

// #define FUNCTION_ENTRY_INFO(_info)
#ifndef FUNCTION_ENTRY_INFO
	#ifdef __ANSICOLORCONSOLE_H
		#define FUNCTION_ENTRY_INFO(_info)	printf(ANSI_COLOR_FG_BRIGHT_GRAY _info ANSI_COLOR_RESET)
	#else
		#define FUNCTION_ENTRY_INFO(_info)	printf(_info)
	#endif // __ANSICOLORCONSOLE_H
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

void HC_Status_Init()
{
	hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
	hc_status.function_status = HC_FUNCTION_STATUS_STANDBY;
	hc_status.bytes_remain = 0;
	hc_status.can_count = 0;
	hc_status.cmdbuff_idx = 0;
	hc_status.errcode = HC_ErrCode_NoError;
	// printf("_hc_handshake_error = %#tx", &_hc_handshake_error);
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
		printf("��%d��CAN\n", hc_status.can_count);
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
		HC_Status_Init();
		#ifdef __HC_DEBUG
		printf("��CAN�ﵽ8�ζ�״̬��λ\n");
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
		hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // ������뻺��
		switch (ch)
		{
		// �˴��Ǹ�����Ĵ���
		case HC_CMD_StartSample:
		case HC_CMD_StopSample:
			// FIX: �����в�Ӧ�����������Ӧ��ר�Ž�һ��״̬
			if (hc_status.function_status == HC_FUNCTION_STATUS_STANDBY)
			{
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
				// hc_status.errcode = HC_ErrCode_ModeError;
				// hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			}
			break;
		case HC_CMD_SwitchReadStorageMode:
			switch (hc_status.function_status)
			{
			case HC_FUNCTION_STATUS_STANDBY:
				hc_status.function_status = HC_FUNCTION_STATUS_STORAGE_EXPORT;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
				break;
			case HC_FUNCTION_STATUS_STORAGE_EXPORT:
				hc_status.function_status = HC_FUNCTION_STATUS_STANDBY;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
				break;
			default:
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
				// hc_status.errcode = HC_ErrCode_ModeError;
				// hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
				break;
			}
			break;
		case HC_CMD_Specify4kSector:
			if (hc_status.function_status == HC_FUNCTION_STATUS_STORAGE_EXPORT)
			{
				hc_status.bytes_remain = 2;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_GET_ARGS;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
				// hc_status.errcode = HC_ErrCode_ModeError;
				// hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			}
			break;
		case HC_CMD_Output4kSector:
			if (hc_status.function_status == HC_FUNCTION_STATUS_STORAGE_EXPORT)
			{
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			}
			else
			{
				_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_ModeError);
				// hc_status.errcode = HC_ErrCode_ModeError;
				// hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			}
			break;
		default:
			_hc_handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_UnknownCmd);
			// hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			// hc_status.errcode = HC_ErrCode_UnknownCmd;
			break;
		}
		break;
	}
	case HC_HANDSHAKE_STATUS_GET_ARGS:
	{
		// ������������������
		if (hc_status.bytes_remain > 0)
		{
			hc_status.cmdbuff[hc_status.cmdbuff_idx++] = ch; // �������뻺��
			--hc_status.bytes_remain;
		}

		// ��������if���ܺ���һ����Ϊ����������ɣ�bytes_remain==0����Ҫ����
		// ת��״̬������������һ�ζ��ַ�������ᶪʧ����ַ���

		// �����������������һ��״̬
		if (hc_status.bytes_remain == 0)
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
		// hc_status.errcode = HC_ErrCode_UnexpectedStatus;
		// hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
		// __handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_UnexpectedStatus);
	}
	}

	return change;
}

void HC_CommandFinishHandle()
{
	// �����Ƿ�ִ�гɹ�������ִ����ɺ�ʹ�����������hc_status.handshake_status
	// ��ΪHC_HANDSHAKE_STATUS_IDLE��������޷������µ����
	hc_HandShakeStatus_Reset();
}


// // NOTE: Э����û�����
// // ����ִ���г��ִ��������״̬�Ա�֪ͨ��λ��
// void HC_ErrorOccuredHandle(uint16_t errcode)
// {
// 	hc_status.errcode = errcode;
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
		hc_status.errcode = HC_ErrCode_NoError;
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
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
	printf("ACK\terrcode=%d\n", errcode);
	#endif // __HC_DEBUG
	return;
}

__WEAK void HC_SendNAKHook(uint16_t errcode)
{
	#ifdef __HC_DEBUG
	printf("NAK %#2x\terrcode=%d\n", errcode, errcode);
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