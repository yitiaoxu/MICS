#ifndef __HOSTCOMMUNICATIONERRORCODE_H
#define __HOSTCOMMUNICATIONERRORCODE_H

#define HC_ErrCode_NoError				0x00	// 无错误

#define HC_ErrCode_UnexpectedStatus		0x10	// 预料之外的状态
#define HC_ErrCode_Busy					0x11	// 正忙，无法执行指令

#define HC_ErrCode_UnknownCmd			0x100	// 未知指令
#define HC_ErrCode_ModeError			0x101	// 指令在当前模式无效

#endif // __HOSTCOMMUNICATIONERRORCODE_H
