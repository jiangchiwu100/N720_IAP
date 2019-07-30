#ifndef __N720task_H
#define __N720task_H

#ifdef __cplusplus
extern "C" {
#endif 

#define Init_Success        0xfff1
#define Init_Fail           0xfff2
#define Ftp_Success         0xfff3
#define Ftp_Fail            0xfff4
#define TCP_Success         0xfff5
#define TCP_Fail            0xfff6
#define SMS_Success         0xfffA
#define SMS_Fail            0xfffB
#define Msg_Finish          0xfffC
#define Msg_Fail            0xfffD



#define SMS_Msg         1
#define TCP_Msg         2
  
typedef struct 
{
    uint8_t len;
    uint8_t buf[256];
}Str_TCP;



/**********************SMS��Ϣ�������״̬************************************/
typedef enum 
{
    SMS_Info_Set             = 1,//������Ϣ
    SMS_Software_Reset       = 2,//������
    SMS_Hardware_Reset       = 3,//Ӳ����λ
    SMS_IAP_Ctrl             = 4,//����IAP
    Ctrl_Error               = 5,//���������
    PWD_Error                = 6,//�����������
}Enum_SMS_Ctrl;
/****************************************************************************/

/************************Init���״̬***************************************/

/************************���״̬��**************************************/
typedef enum 
{ 
    Enum_N720_Check_AT          = 0,//���ATͨѶ
    Enum_N720_Check_Internet    = 1,//�����������
    Enum_N720_Init_State        = 2,//��ʼ��N720
    Enum_N720_Init_SMS          = 3,//��ʼ��SMS����
    Enum_N720_Init_TCP          = 4,//��ʼ��TCP����
    Enum_N720_JudgeAndGet       = 5,//�ȴ�������Ϣ
}Enum_N720_Init;


/************************��һ��״̬��**************************************/

typedef enum 
{ 
    Enum_N720_PowerOn_Disable   = 1,//����poweron���� �����ɹ� Ȼ����ʱ10s 
    Enum_N720_Check_Report      = 2,//���͡�AT����鴮���Ƿ���ͨ
    Enum_N720_FCDisplay         = 3,//�رջ���
    Enum_N720_Check_SIMCard     = 4,//���SIM��״̬   
    Enum_N720_Query_Net_Reg     = 5,//��ѯ����ע�����
    Enum_N720_Query_Net_Attach  = 6,//��ѯ���總�����
    Enum_N720_Enable_Reporting  = 7,//���������ϱ�
    Enum_N720_Set_APN           = 8,//��������ҵ�����(APN����) 
    Enum_N720_Set_USERPWD       = 9,//�����û���������
    Enum_N720_Net_Connection    = 10,//������������
    Enum_N720_Check_Connection  = 11,//��ѯ�����������
    Enum_N720_Init_Fail         = 12,//��ʼ��ʧ��    
}Enum_N720_State;

typedef enum 
{
    Enum_N720_Set_SMS_Mode      = 1,//���ö���ģʽ
    Enum_N720_Set_Char          = 2,//�����ַ���
    Enum_N720_Reporting_Mode    = 3,//���ö��ŵ��������ϱ�ģʽ
    Enum_N720_SMS_Fail          = 4,//SMS��ʼ��ʧ��
}Enum_N720_SMS;

typedef enum 
{ 
    Enum_N720_Close_TCP         = 1,//�ر�TCP����
    Enum_N720_Set_TCP           = 2,//����TCP���Ӳ���
    Enum_N720_Open_TCP          = 3,//��TCP����
    Enum_N720_TCP_Fail          = 4,//TCP��ʼ��ʧ��
}Enum_N720_TCP;

typedef enum
{
    Null_Type                   = 0,//����Ϣ
    SMS_Type                    = 1,//SMS��Ϣ
    TCP_Type                    = 2,//TCP��Ϣ
    Get_SMS_Success             = 3,//��ȡ���ųɹ�
    SMS_Error                   = 4,//�������
    Get_SMS_Fail                = 5,//��ȡ����ʧ��
    Get_TCP_Success             = 6,//��ȡTCPͨѶ���ݳɹ�
    TCP_Error                   = 7,//����TCP��Ϣ
    Get_TCP_Fail                = 8,//��ȡTCPͨѶ����ʧ��
}Enum_Message_JudgeAndGet;

/**************************************************************************/

extern OS_STK AppTaskN720InitStk[];
void AppTaskN720Init(void *p_arg);
uint32_t N720_Init(void);

uint32_t N720_State_Init(void);
uint32_t N720_SMS_Init(void);
uint32_t N720_TCP_Init(void);
uint32_t N720_JudgeAndGet(void);

extern OS_STK AppTaskSMSCommStk[];
void AppTaskSMSComm(void *p_arg);

void SMS_Comm(void);

#ifdef __cplusplus
}
#endif
#endif