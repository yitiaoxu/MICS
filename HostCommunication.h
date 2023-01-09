#ifndef __HOSTCOMMUNICATION_H
#define __HOSTCOMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


#define HC_CMDBUFF_SIZE						16

#define HC_STATUS_RESET_BY_CAN_NUM			8		// 状态机复位所需接收到的CAN的数目
#define HC_STATUS_RESET_BY_IDLE				1000	// 状态机复位所需空闲时间

// 握手状态机
#define HC_HANDSHAKE_STATUS_IDLE			0		// 归位
#define HC_HANDSHAKE_STATUS_GET_CMD			1		// TODO
#define HC_HANDSHAKE_STATUS_GET_ARGS		2		// TODO
#define HC_HANDSHAKE_STATUS_WAIT_ACK		3		// 指令传输完毕等待回复ACK
#define HC_HANDSHAKE_STATUS_WAIT_EXEC		4		// 指令传输完毕等待执行
#define HC_HANDSHAKE_STATUS_ON_ERROR		5		// 出错等待处理

// 功能状态机
#define HC_FUNCTION_STATUS_STANDBY			0		// 就绪
#define HC_FUNCTION_STATUS_STORAGE_EXPORT	10		// 导出数据

typedef struct _HC_Status_t
{
	uint16_t handshake_status;			// 握手状态
	uint16_t function_status;			// 功能状态
	// uint16_t command_status;			// 指令状态
	uint16_t bytes_remain;				// 剩余参数
	uint16_t can_count;					// CAN字符计数
	uint16_t cmdbuff_idx;				// 命令缓存头指针
	uint16_t errcode;					// 错误状态及其代码
	uint8_t  cmdbuff[HC_CMDBUFF_SIZE];	// 命令缓存
} HostCommunicationStatus_t;

void HC_Status_Init(void);
void HC_HandShakeStatus_Reset(void);

/// @brief 取得hc_status的只读指针
/// @return &hc_status
const HostCommunicationStatus_t *HC_Status(void);

// /// @brief 判断握手信号是否处于HC_HANDSHAKE_STATUS_WAIT_EXEC状态
// /// @return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC)
// bool HC_CommandWaitToExecute(void);


/// @brief 串口得到的字符送入状态机驱动状态转移
/// @param ch 送入状态机的字符
void HC_GotCharHandle(uint8_t ch);

/// @brief 命令执行完毕后将握手状态机复位，以便接受下一条命令
void HC_CommandFinishHandle(void);

/// @brief 检查状态后回复上位机，调用'HC_SendACKHook'、'HC_SendNAKHook'和'HC_ErrorProcessHook'
void HC_ResponseCheckHandle(void);

/// @brief 检查是否超时，超时则调用复位
void HC_TimeOutCheckHandle(void);

/// @brief 检测并执行相应功能，调用HC_ExecuteHook(...)
void HC_CheckAndExecuteHandle(void);

/// @brief 实现ACK的发送
/// @param errcode 错误代码，为'hc_status.errcode'，理应为'HC_ErrCode_NoError'
void HC_SendACKHook(uint16_t errcode);

/// @brief 实现NAK的发送
/// @param errcode 错误代码，为hc_status.errcode
void HC_SendNAKHook(uint16_t errcode);

/// @brief 实现错误处理
/// @param p_hc_status 指向hc_status的指针
void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status);

/// @brief 执行的实现
/// @param p_hc_status
void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status);

#ifdef __cplusplus
}
#endif


#endif // __HOSTCOMMUNICATION_H
