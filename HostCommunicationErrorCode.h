#ifndef __HOSTCOMMUNICATIONERRORCODE_H
#define __HOSTCOMMUNICATIONERRORCODE_H


// NOTE: Э���д�������ʹ����1���ֽڣ����Ǵ���ؼ�����2���ֽڿ��ã���˴�����벻�ܳ���1�ֽ�

#define HC_ErrCode_NoError				0x00	// �޴���
#define HC_ErrCode_IDLE					0x00	// ����
#define HC_ErrCode_UnknownError			0xff	// δ֪����

#define HC_ErrCode_UnexpectedStatus		0x10	// Ԥ��֮���״̬
#define HC_ErrCode_Busy					0x11	// ��æ���޷�ִ��ָ��
#define HC_ErrCode_TimeOut				0x12	// �����涨ʱ�䣬ͨ��ʧ��
#define HC_ErrCode_ProtocolMismatch		0x13	// ���ܵ���������Э�鲻ƥ��

#define HC_ErrCode_UnknownCmd			0xa0	// δָ֪��
#define HC_ErrCode_InvalidCmd			0xa1	// ����ָ��
#define HC_ErrCode_ModeError			0xa2	// ָ���ڵ�ǰģʽ��Ч

#endif // __HOSTCOMMUNICATIONERRORCODE_H
