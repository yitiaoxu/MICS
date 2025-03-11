#ifndef __HOSTCOMMUNICATION_H
#define __HOSTCOMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "PublicCache.h"

#define HC_CMDBUFF_SIZE						16		// ����ָ���ռ䣬�����ָ�����
#define HC_DATABUFF_SIZE		PUBLIC_CACHE_SIZE	// ���ݻ���ռ�

#define HC_STATUS_RESET_BY_CAN_NUM			8		// ״̬����λ������յ���CAN����Ŀ
#define HC_STATUS_RESET_BY_IDLE				1000	// ״̬����λ�������ʱ�䣨��λ��ms��

// ����״̬��
#define HC_HANDSHAKE_STATUS_IDLE			0		// ��λ
#define HC_HANDSHAKE_STATUS_GET_CMD			1		// �ȴ��������1�ֽڣ�
#define HC_HANDSHAKE_STATUS_GET_ARGS		2		// �ȴ�����������������������
#define HC_HANDSHAKE_STATUS_WAIT_ACK		3		// ָ�����ϵȴ��ظ�ACK
#define HC_HANDSHAKE_STATUS_WAIT_DATA		4		// �ȴ��������ݣ�һ����ݲ���������
#define HC_HANDSHAKE_STATUS_WAIT_EXEC		5		// ָ������ݴ�����ϵȴ�ִ��
#define HC_HANDSHAKE_STATUS_ON_ERROR		6		// ����ȴ�����

// ����״̬��
#define HC_FUNCTION_STATUS_STANDBY			0		// ����
#define HC_FUNCTION_STATUS_SAMPLING			8		// ���ڲ���
#define HC_FUNCTION_STATUS_OPERATE_STORAGE	10		// ����������ģʽ

/// @brief ��λ��ͨ��״̬��״̬��������
typedef volatile struct _HC_Status_t
{
	uint16_t handshake_status;			// ����״̬
	uint16_t function_status;			// ����״̬
	uint16_t args_bytes_remain;			// ʣ�����
	uint16_t data_bytes_remain;			// ʣ������
	uint16_t can_count;					// CAN�ַ�����
	uint16_t cmdbuff_idx;				// �����ͷָ��
	uint16_t handshake_errcode;			// ���ִ���״̬�������
	uint16_t datarecv_errcode;			// ���ݽ��մ���״̬�������
	uint32_t wait_tic;					// ͨ�ŵȴ���ʼʱ��ʱ��
	uint8_t *data_buff_head;			// ���ݻ���ͷָ��
	uint8_t *data_buff_point;			// ���ݻ���дָ��
	uint8_t  cmdbuff[HC_CMDBUFF_SIZE];	// �����
} HostCommunicationStatus_t;

/// @brief ��ʼ����λ��ͨ��״̬
void HC_Status_Reset(void);

/// @brief ��ʼ����λ��ͨ��״̬�������û���λ��
/// @param databuff ���ݻ���ָ��
void HC_Status_Init(uint8_t *databuff);
// void HC_HandShakeStatus_Reset(void);

/// @brief ȡ��hc_status��ֻ��ָ��
/// @return &hc_status
const HostCommunicationStatus_t *HC_Status(void);

// /// @brief �ж������ź��Ƿ���HC_HANDSHAKE_STATUS_WAIT_EXEC״̬
// /// @return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC)
// bool HC_CommandWaitToExecute(void);


/// @brief ���ڵõ����ַ�����״̬������״̬ת��
/// @param ch ����״̬�����ַ�
/// @return ״̬�Ƿ�ı�
/// @note ��Ҫ�ڽ����ַ����޸ĵ�״̬���ڴ�
bool HC_GotCharHandle(uint8_t ch);

/// @brief ����ִ����Ϻ�����״̬����λ���Ա������һ������
/// @attention �����Ƿ�ִ�гɹ�������ִ����ɺ�ʹ�����������hc_status.handshake_status��ΪHC_HANDSHAKE_STATUS_IDLE��������޷������µ����
void HC_CommandFinishHandle(void);

/// @brief ���״̬��ظ���λ��������'HC_SendACKHook'��'HC_SendNAKHook'��'HC_ErrorProcessHook'
/// @note ��Ҫ�ڻظ��ַ����޸ĵ�״̬���ڴ�
void HC_ResponseCheckHandle(void);

/// @brief ����Ƿ�ʱ����ʱ����ø�λ
void HC_TimeOutCheckHandle(void);

/// @brief ��Ⲣִ����Ӧ���ܣ�����HC_ExecuteHook(...)
void HC_CheckAndExecuteHandle(void);

/// @brief ʵ��ACK�ķ���
/// @param handshake_errcode ������룬Ϊ'hc_status.handshake_errcode'����ӦΪ'HC_ErrCode_NoError'
void HC_SendACKHook(uint16_t handshake_errcode);

/// @brief ʵ��NAK�ķ���
/// @param handshake_errcode ������룬Ϊhc_status.handshake_errcode
void HC_SendNAKHook(uint16_t handshake_errcode);

/// @brief ʵ�ִ�����
/// @param p_hc_status ָ��hc_status��ָ��
void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status);

/// @brief ִ�е�ʵ��
/// @param p_hc_status ָ��hc_status��ָ��
/// @note �����������״̬��������HC_GotCharHandle���
void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status);

/// @brief ��ȡʱ���
/// @return ��ǰʱ�̵�ʱ���
uint32_t HC_GetTimeHook(void);

/// @brief �ж��Ƿ�ʱ
/// @param tic ��ʼʱ��ʱ���
/// @param toc ����ʱ��ʱ���
/// @retval `true`: ��ʱ
/// @retval `false`: δ��ʱ
bool HC_TimeoutHook(uint32_t tic, uint32_t toc);

#ifdef __cplusplus
}
#endif


#endif // __HOSTCOMMUNICATION_H
