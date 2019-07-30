#ifndef __GateWayVariable_H
#define __GateWayVariable_H

#ifdef __cplusplus
extern "C" {
#endif 
    
typedef struct
{
    uint8_t len         ;
    uint8_t buf[256]    ;
}Str_SMS_Buf            ;
  
typedef struct
{
    uint8_t len         ;
    uint8_t buf[256]    ;
}Str_TCP_Buf            ;

typedef struct
{ 
    uint8_t   PhoneNum[11]      ;//�ֻ�����
    uint8_t   DevID[2]          ;//�豸ID
    uint8_t   LoraChannel       ;//�ŵ���
    uint8_t   TcpIpPort[22]     ;//TCP IP �˿�
    uint8_t   IpPort[6]         ;//IP �˿�
}Str_ConfigInfo_PARM            ;//

typedef union
{
    uint8_t buf[96];
    struct
    { 
        uint8_t   IAPState[2]       ;//IAP��־λ
        uint8_t   PhoneNum[11]      ;//�ֻ�����
        uint8_t   FtpIpPort[22]     ;//FTP IP �˿�
        uint8_t   FtpName[16]       ;//FTP�������û���
        uint8_t   FtpPwd[16]        ;//FTP����������
        uint8_t   FtpFileName[16]   ;//�����̼��ļ���
    }Str_BootLoader_PARM        ;//
}Union_BootLoader_PARM          ;//


//extern Union_BootLoader_PARM BootLoader;
//extern Union_ConfigInfo_PARM ConfigInfo;
extern Str_ConfigInfo_PARM ConfigInfo;
extern Str_TCP_Buf TCPbuf;     //TCP��Ϣ���
extern Str_SMS_Buf SMSbuf;     //SMS��Ϣ���

/* ������ ȫ�ֱ���  ���豣�����ļ�ϵͳ���� */
extern uint32_t  SysState  ;
extern Str_MeterLib_Parm MeterLib[] exram;
extern uint8_t  N720MessageNum[ ] exram  ; 
extern uint8_t  N720MessageQue[ ] exram  ; 
extern uint16_t GW3761DatFrameSeqNum     ;





void DiskFormatting(void);
void ViewRootDir(void);

void CreateGateWayConfigInfoFile(void);
void ReadGateWayConfigInfoFile(void);

void CreateGateWayMeterLibFile(void);
void ReadGateWayMeterLibFile(void);

void GateWay_Variable_Init(void);

#ifdef __cplusplus
}
#endif
#endif