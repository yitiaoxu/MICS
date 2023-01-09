#include "HostCommunication.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ansicolorconsole.h"

#include "asciiControlCode.h"
#include "HostCommunicationCommandCode.h"
#include "HostCommunicationErrorCode.h"


#define __weak __attribute__((weak))

static HostCommunicationStatus_t hc_status;


void HC_Status_Init()
{
	hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
	hc_status.function_status = HC_FUNCTION_STATUS_STANDBY;
	// hc_status.command_status = HC_COMMAND_STATUS_NULL;
	hc_status.bytes_remain = 0;
	hc_status.can_count = 0;
	hc_status.cmdbuff_idx = 0;
	hc_status.errcode = HC_ErrCode_NoError;
	return;
}

void HC_HandShakeStatus_Reset()
{
	hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
	// hc_status.bytes_remain = 0;
	// hc_status.cmdbuff_idx = 0;
	hc_status.errcode = HC_ErrCode_NoError;
	// TODO: __handshake_error
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


#define __handshake_error(next_status, errorcode) \
	hc_status.handshake_status = next_status;     \
	hc_status.errcode          = errorcode;
// TODO: ת��Ϊ�����ʽ

void HC_GotCharHandle(uint8_t ch)
{
	// uint8_t err;
	// ����8������CAN��λ
	if (hc_cancel_test(ch))
	{
		HC_Status_Init();
		#ifdef __HC_DEBUG
		printf("��CAN�ﵽ8�ζ�״̬��λ\n");
		#endif
		return;
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
		// ʣ�µ��ַ�������
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
				hc_status.errcode = HC_ErrCode_ModeError;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
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
				hc_status.errcode = HC_ErrCode_ModeError;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
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
				hc_status.errcode = HC_ErrCode_ModeError;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			}
			break;
		case HC_CMD_Output4kSector:
			if (hc_status.function_status == HC_FUNCTION_STATUS_STORAGE_EXPORT)
			{
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_ACK;
			}
			else
			{
				hc_status.errcode = HC_ErrCode_ModeError;
				hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			}
			break;
		default:
			hc_status.handshake_status = HC_HANDSHAKE_STATUS_ON_ERROR;
			hc_status.errcode = HC_ErrCode_UnknownCmd;
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
		// ʣ�µ��ַ�������
		break;
	}
	default:
	{
		// hc_status.errcode = HC_ErrCode_UnexpectedStatus;
		// hc_status.handshake_status = HC_HANDSHAKE_STATUS_IDLE;
		__handshake_error(HC_HANDSHAKE_STATUS_ON_ERROR, HC_ErrCode_UnexpectedStatus);
	}
	}

	return;
}

void HC_CommandFinishHandle()
{
	// �����Ƿ�ִ�гɹ�������ִ����ɺ�ʹ�����������hc_status.handshake_status
	// ��ΪHC_HANDSHAKE_STATUS_IDLE��������޷������µ����
	HC_HandShakeStatus_Reset();
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
	printf(ANSI_COLOR_FG_BRIGHT_GRAY "HC_ResponseCheckHandle()\r\n" ANSI_COLOR_RESET);

	switch (hc_status.handshake_status)
	{
	// ������ɣ��ظ���λ��ACK
	case HC_HANDSHAKE_STATUS_WAIT_ACK:
		HC_SendACKHook(hc_status.errcode);
		hc_status.handshake_status = HC_HANDSHAKE_STATUS_WAIT_EXEC;
		break;

	// ��һ��������ɻ�δִ�У��µ�����ͷ��ͽ�������ʱӦ�ظ�æ����
	case HC_HANDSHAKE_STATUS_WAIT_EXEC:
		HC_SendNAKHook(hc_status.errcode);
		HC_ErrorProcessHook(&hc_status);
		hc_status.errcode = HC_ErrCode_NoError;
		break;

	// �����г��ִ��󣬻ظ���λ��NAK
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
	printf(ANSI_COLOR_FG_BRIGHT_GRAY "HC_TimeOutCheckHandle()\r\n" ANSI_COLOR_RESET);
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

void __weak HC_SendACKHook(uint16_t errcode)
{
	printf("ACK\terrcode=%d\n", errcode);
	return;
}

void __weak HC_SendNAKHook(uint16_t errcode)
{
	printf("NAK %#2x\terrcode=%d\n", errcode, errcode);
	return;
}

void __weak HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status)
{
	return;
}

void __weak HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status)
{
	return;
}