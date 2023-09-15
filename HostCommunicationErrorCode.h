#ifndef __HOSTCOMMUNICATIONERRORCODE_H
#define __HOSTCOMMUNICATIONERRORCODE_H


// NOTE: 协议中错误代码仅使用了1个字节，但是储存控件中有2个字节可用，因此错误代码不能超过1字节

#define HC_ErrCode_NoError				0x00	// 无错误
#define HC_ErrCode_IDLE					0x00	// 空闲
#define HC_ErrCode_UnknownError			0xff	// 未知错误

#define HC_ErrCode_UnexpectedStatus		0x10	// 预料之外的状态
#define HC_ErrCode_Busy					0x11	// 正忙，无法执行指令
#define HC_ErrCode_TimeOut				0x12	// 超过规定时间，通信失败
#define HC_ErrCode_ProtocolMismatch		0x13	// 接受到的数据与协议不匹配

#define HC_ErrCode_UnknownCmd			0xa0	// 未知指令
#define HC_ErrCode_InvalidCmd			0xa1	// 错误指令
#define HC_ErrCode_ModeError			0xa2	// 指令在当前模式无效

#endif // __HOSTCOMMUNICATIONERRORCODE_H
