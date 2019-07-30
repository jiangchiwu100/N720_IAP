#ifndef __Tcp4GComm_H
#define __Tcp4GComm_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#define Login_Success       0xfff1
#define Login_Fail          0xfff2
#define HeartBeat_Success   0xfff3
#define HeartBeat_Fail      0xfff4
    
#define CommTX_Empty        0xfff5 
#define CommTX_Finish       0xfff6   
#define CommTX_Fail         0xfff7    
#define CommRX_Finish       0xfff8
#define CommRX_Fail         0xfff9
#define CommRx_Empty        0xfffA
    
typedef enum 
{
    Enum_TCP_Send_Login         = 1,//���͵�¼֡
    Enum_Login_Rec_Reply        = 2,//���ջ�Ӧ����֡
} Enum_TCP_Login;

typedef enum 
{
    Enum_TCP_Send_HeartBeat     = 1,//��������
    Enum_HeartBeat_Rec_Reply    = 2,//���ջ�Ӧ����֡
} Enum_TCP_HeartBeat;

typedef enum 
{
    Enum_Read_Send_Queue        = 1,//��ȡ���Ͷ���
    Enum_TCP_Send_Data          = 2,//��������
    Enum_Wait_Rec_Reply         = 3,//�ȴ����ջ�Ӧ����֡
} Enum_TCP_TX;

typedef enum 
{ 
    Enum_TCP_Login_Terminal    = 1,//��½�ն�
    Enum_TCP_Send_Heartbeat    = 2,//��������
    Enum_TCP_Comm_RX           = 3,//ͨѶ���ݽ���
    Enum_TCP_Comm_TX           = 4,//ͨѶ���ݷ���    
} Enum_TCP_Comm;

extern OS_STK AppTaskTCPCommStk[];
void AppTaskTCPComm(void *p_arg);
uint32_t TCP_Comm(void);


uint32_t TCP_Login_Terminal(void);
uint32_t TCP_HeartBeat(void);
uint32_t TCP_Comm_TX(void);

#ifdef __cplusplus
}
#endif
#endif