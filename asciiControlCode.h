#ifndef __ASCIICONTROLCODE_H
#define __ASCIICONTROLCODE_H

#define ASCII_NUL 0x00	// 空字符（Null）
#define ASCII_SOH 0x01	// 标题开始
#define ASCII_STX 0x02	// 本文开始
#define ASCII_ETX 0x03	// 本文结束
#define ASCII_EOT 0x04	// 传输结束
#define ASCII_ENQ 0x05	// 请求
#define ASCII_ACK 0x06	// 确认回应
#define ASCII_BEL 0x07	// 响铃
#define ASCII_BS  0x08	// 退格
#define ASCII_HT  0x09	// 水平定位符号
#define ASCII_LF  0x0A	// 换行键
#define ASCII_VT  0x0B	// 垂直定位符号
#define ASCII_FF  0x0C	// 换页键
#define ASCII_CR  0x0D	// CR（字符）
#define ASCII_SO  0x0E	// 取消变换（Shift out）
#define ASCII_SI  0x0F	// 启用变换（Shift in）
#define ASCII_DLE 0x10	// 跳出数据通讯
#define ASCII_DC1 0x11	// 设备控制一（XON 激活软件速度控制）
#define ASCII_DC2 0x12	// 设备控制二
#define ASCII_DC3 0x13	// 设备控制三（XOFF 停用软件速度控制）
#define ASCII_DC4 0x14	// 设备控制四
#define ASCII_NAK 0x15	// 确认失败回应
#define ASCII_SYN 0x16	// 同步用暂停
#define ASCII_ETB 0x17	// 区块传输结束
#define ASCII_CAN 0x18	// 取消
#define ASCII_EM  0x19	// 连线介质中断
#define ASCII_SUB 0x1A	// 替换
#define ASCII_ESC 0x1B	// 退出键
#define ASCII_FS  0x1C	// 文件分割符
#define ASCII_GS  0x1D	// 组群分隔符
#define ASCII_RS  0x1E	// 记录分隔符
#define ASCII_US  0x1F	// 单元分隔符
#define ASCII_DEL 0x7F	// Delete字符

#endif // __ASCIICONTROLCODE_H
