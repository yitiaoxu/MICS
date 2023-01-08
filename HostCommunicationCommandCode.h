#ifndef __HOSTCOMMUNICATIONCOMMANDCODE_H
#define __HOSTCOMMUNICATIONCOMMANDCODE_H

#define HC_CMD_StartSample				0x10	// 开始采样
#define HC_CMD_StopSample				0x11	// 停止采样

#define HC_CMD_SwitchReadStorageMode	0x20	// 读Flash模式
#define HC_CMD_Specify4kSector			0x21	// 指定4K扇区编号
#define HC_CMD_Output4kSector 			0x22	// 导出当前4K扇区

#endif // __HOSTCOMMUNICATIONCOMMANDCODE_H
