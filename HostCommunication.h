#ifndef __HOSTCOMMUNICATION_H
#define __HOSTCOMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "PublicCache.h"

#define HC_CMDBUFF_SIZE						16		// 解析指令缓存空间，比最大指令长即可
#define HC_DATABUFF_SIZE		PUBLIC_CACHE_SIZE	// 数据缓存空间

#define HC_STATUS_RESET_BY_CAN_NUM			8		// 状态机复位所需接收到的CAN的数目
#define HC_STATUS_RESET_BY_IDLE				1000	// 状态机复位所需空闲时间（单位：ms）

// 握手状态机
#define HC_HANDSHAKE_STATUS_IDLE			0		// 归位
#define HC_HANDSHAKE_STATUS_GET_CMD			1		// 等待输入命令（1字节）
#define HC_HANDSHAKE_STATUS_GET_ARGS		2		// 等待输入参数（根据命令决定）
#define HC_HANDSHAKE_STATUS_WAIT_ACK		3		// 指令传输完毕等待回复ACK
#define HC_HANDSHAKE_STATUS_WAIT_DATA		4		// 等待输入数据（一般根据参数决定）
#define HC_HANDSHAKE_STATUS_WAIT_EXEC		5		// 指令和数据传输完毕等待执行
#define HC_HANDSHAKE_STATUS_ON_ERROR		6		// 出错等待处理

// 功能状态机
#define HC_FUNCTION_STATUS_STANDBY			0		// 就绪
#define HC_FUNCTION_STATUS_SAMPLING			8		// 正在采样
#define HC_FUNCTION_STATUS_OPERATE_STORAGE	10		// 储存器操作模式

/// @brief 上位机通信状态机状态数据类型
typedef volatile struct _HC_Status_t
{
	uint16_t handshake_status;			// 握手状态
	uint16_t function_status;			// 功能状态
	uint16_t args_bytes_remain;			// 剩余参数
	uint16_t data_bytes_remain;			// 剩余数据
	uint16_t can_count;					// CAN字符计数
	uint16_t cmdbuff_idx;				// 命令缓存头指针
	uint16_t handshake_errcode;			// 握手错误状态及其代码
	uint16_t datarecv_errcode;			// 数据接收错误状态及其代码
	uint32_t wait_tic;					// 通信等待开始时的时刻
	uint8_t *data_buff_head;			// 数据缓存头指针
	uint8_t *data_buff_point;			// 数据缓存写指针
	uint8_t  cmdbuff[HC_CMDBUFF_SIZE];	// 命令缓存
} HostCommunicationStatus_t;

/// @brief 初始化上位机通信状态
void HC_Status_Reset(void);

/// @brief 初始化上位机通信状态，并设置缓存位置
/// @param databuff 数据缓存指针
void HC_Status_Init(uint8_t *databuff);
// void HC_HandShakeStatus_Reset(void);

/// @brief 取得hc_status的只读指针
/// @return &hc_status
const HostCommunicationStatus_t *HC_Status(void);

// /// @brief 判断握手信号是否处于HC_HANDSHAKE_STATUS_WAIT_EXEC状态
// /// @return (hc_status.handshake_status == HC_HANDSHAKE_STATUS_WAIT_EXEC)
// bool HC_CommandWaitToExecute(void);


/// @brief 串口得到的字符送入状态机驱动状态转移
/// @param ch 送入状态机的字符
/// @return 状态是否改变
/// @note 需要在接收字符后修改的状态均在此
bool HC_GotCharHandle(uint8_t ch);

/// @brief 命令执行完毕后将握手状态机复位，以便接受下一条命令
/// @attention 无论是否执行成功，命令执行完成后使用这个函数把hc_status.handshake_status设为HC_HANDSHAKE_STATUS_IDLE，否则会无法接收新的命令。
void HC_CommandFinishHandle(void);

/// @brief 检查状态后回复上位机，调用'HC_SendACKHook'、'HC_SendNAKHook'和'HC_ErrorProcessHook'
/// @note 需要在回复字符后修改的状态均在此
void HC_ResponseCheckHandle(void);

/// @brief 检查是否超时，超时则调用复位
void HC_TimeOutCheckHandle(void);

/// @brief 检测并执行相应功能，调用HC_ExecuteHook(...)
void HC_CheckAndExecuteHandle(void);

/// @brief 实现ACK的发送
/// @param handshake_errcode 错误代码，为'hc_status.handshake_errcode'，理应为'HC_ErrCode_NoError'
void HC_SendACKHook(uint16_t handshake_errcode);

/// @brief 实现NAK的发送
/// @param handshake_errcode 错误代码，为hc_status.handshake_errcode
void HC_SendNAKHook(uint16_t handshake_errcode);

/// @brief 实现错误处理
/// @param p_hc_status 指向hc_status的指针
void HC_ErrorProcessHook(HostCommunicationStatus_t *const p_hc_status);

/// @brief 执行的实现
/// @param p_hc_status 指向hc_status的指针
/// @note 本函数不检查状态，而是由HC_GotCharHandle检查
void HC_ExecuteHook(const HostCommunicationStatus_t *const p_hc_status);

/// @brief 获取时间戳
/// @return 当前时刻的时间戳
uint32_t HC_GetTimeHook(void);

/// @brief 判断是否超时
/// @param tic 起始时刻时间戳
/// @param toc 结束时刻时间戳
/// @retval `true`: 超时
/// @retval `false`: 未超时
bool HC_TimeoutHook(uint32_t tic, uint32_t toc);

#ifdef __cplusplus
}
#endif


#endif // __HOSTCOMMUNICATION_H
