#ifndef __HOSTCOMMUNICATIONCOMMANDCODE_H
#define __HOSTCOMMUNICATIONCOMMANDCODE_H

#define HC_CMD_NOP						0x01	// 空指令
#define HC_CMD_INVALID0					0x00	// 无效指令
#define HC_CMD_INVALID1					0xFF	// 无效指令

#define HC_CMD_FuncStatusBackToStandBy	0x1b	// 功能状态回到就绪

#define HC_CMD_StartSample				0x10	// 开始采样
#define HC_CMD_StopSample				0x11	// 停止采样

#define HC_CMD_SwitchOperateStorageMode	0x20	// 操作Flash模式
#define HC_CMD_Specify64kBlock			0x21	// 指定64K块编号
#define HC_CMD_Output64kBlock			0x22	// 导出当前64K块
#define HC_CMD_Erase64kBlock			0x23	// 擦除当前64K块
#define HC_CMD_EraseFullChip			0x2F	// 全片擦除

#endif // __HOSTCOMMUNICATIONCOMMANDCODE_H
