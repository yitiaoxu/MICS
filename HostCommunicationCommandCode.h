#ifndef __HOSTCOMMUNICATIONCOMMANDCODE_H
#define __HOSTCOMMUNICATIONCOMMANDCODE_H

#define HC_CMD_NOP						0x01	// ��ָ��
#define HC_CMD_INVALID0					0x00	// ��Чָ��
#define HC_CMD_INVALID1					0xFF	// ��Чָ��

#define HC_CMD_FuncStatusBackToStandBy	0x1b	// ����״̬�ص�����

#define HC_CMD_StartSample				0x10	// ��ʼ����
#define HC_CMD_StopSample				0x11	// ֹͣ����

#define HC_CMD_SwitchOperateStorageMode	0x20	// ����Flashģʽ
#define HC_CMD_Specify64kBlock			0x21	// ָ��64K����
#define HC_CMD_Output64kBlock			0x22	// ������ǰ64K��
#define HC_CMD_Erase64kBlock			0x23	// ������ǰ64K��
#define HC_CMD_Specify4kSector			0x24	// ָ��4K�������
#define HC_CMD_Output4kSector			0x25	// ������ǰ4K������Ԥ����
#define HC_CMD_Erase4kSector			0x26	// ������ǰ4K����
#define HC_CMD_AskLastWriteResult		0x28	// ѯ���ϴ�д���Ƿ����
#define HC_CMD_AskFlashStatus			0x29	// ѯ��Flash�Ƿ�æ
#define HC_CMD_WriteFlashAtAddr			0x2a	// ��ָ����ַд������
#define HC_CMD_EraseFullChip			0x2F	// ȫƬ����

#endif // __HOSTCOMMUNICATIONCOMMANDCODE_H
