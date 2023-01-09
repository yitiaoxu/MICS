#ifndef __HOSTCOMMUNICATION_H
#define __HOSTCOMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


#define HC_CMDBUFF_SIZE						16

#define HC_STATUS_RESET_BY_CAN_NUM			8		// ״̬����λ������յ���CAN����Ŀ
#define HC_STATUS_RESET_BY_IDLE				1000	// ״̬����λ�������ʱ��

// ����״̬��
#define HC_HANDSHAKE_STATUS_IDLE			0		// ��λ
#define HC_HANDSHAKE_STATUS_GET_CMD			1		// TODO
#define HC_HANDSHAKE_STATUS_GET_ARGS		2		// TODO
#define HC_HANDSHAKE_STATUS_WAIT_ACK		3		// ָ�����ϵȴ��ظ�ACK
#define HC_HANDSHAKE_STATUS_WAIT_EXEC		4		// ָ�����ϵȴ�ִ��
#define HC_HANDSHAKE_STATUS_ON_ERROR		5		// ����ȴ�����

// ����״̬��
#define HC_FUNCTION_STATUS_STANDBY			0		// ����
#define HC_FUNCTION_STATUS_STORAGE_EXPORT	10		// ��������

typedef struct _HC_Status_t
{
	uint16_t handshake_status;			// ����״̬
	uint16_t function_status;			// ����״̬
	// uint16_t command_status;			// ָ��״̬
	uint16_t bytes_remain;				// ʣ�����
	uint16_t can_count;					// CAN�ַ�����
	uint16_t cmdbuff_idx;				// �����ͷָ��
	uint16_t errcode;					// ����״̬�������
	uint8_t  cmdbuff[HC_CMDBUFF_SIZE];	// �����
} HostCommunicationStatus_t;

void HC_Status_Init(void);
void HC_HandShakeStatus_Reset(void);

/// @brief ȡ��hc_status��ֻ��ָ��
/// @return &hc_status
const HostCommunicationStatus_t *HC_Status(void);

// /// @brief �ж������ź��Ƿ���HC_HANDSHAKE_STATUS_WAIT_EXEC״̬
// /// @return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC)
// bool HC_CommandWaitToExecute(void);


/// @brief ���ڵõ����ַ�����״̬������״̬ת��
/// @param ch ����״̬�����ַ�
void HC_GotCharHandle(uint8_t ch);

/// @brief ����ִ����Ϻ�����״̬����λ���Ա������һ������
void HC_CommandFinishHandle(void);

/// @brief ���״̬��ظ���λ��������'HC_SendACKHook'��'HC_SendNAKHook'��'HC_ErrorProcessHook'
void HC_ResponseCheckHandle(void);

/// @brief ����Ƿ�ʱ����ʱ����ø�λ
void HC_TimeOutCheckHandle(void);

/// @brief ��Ⲣִ����Ӧ���ܣ�����HC_ExecuteHook(...)
void HC_CheckAndExecuteHandle(void);

/// @brief ʵ��ACK�ķ���
/// @param errcode ������룬Ϊ'hc_status.errcode'����ӦΪ'HC_ErrCode_NoError'
void HC_SendACKHook(uint16_t errcode);

/// @brief ʵ��NAK�ķ���
/// @param errcode ������룬Ϊhc_status.errcode
void HC_SendNAKHook(uint16_t errcode);

/// @brief ʵ�ִ�����
/// @param p_hc_status ָ��hc_status��ָ��
void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status);

/// @brief ִ�е�ʵ��
/// @param p_hc_status
void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status);

#ifdef __cplusplus
}
#endif


#endif // __HOSTCOMMUNICATION_H
