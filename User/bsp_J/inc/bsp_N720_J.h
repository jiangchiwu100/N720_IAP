#ifndef __bsp_N720_H
#define __bsp_N720_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "N720task.h"
  
/* 按键口对应的RCC时钟 */
#define RCC_ALL_N720 	(RCC_APB2Periph_GPIOC)

#define GPIO_PORT_N720_RST     GPIOC
#define GPIO_PIN_N720_RST      GPIO_Pin_1

#define GPIO_PORT_N720_PCTRL   GPIOC
#define GPIO_PIN_N720_PCTRL    GPIO_Pin_2

#define GPIO_PORT_N720_ONOFF   GPIOC
#define GPIO_PIN_N720_ONOFF    GPIO_Pin_3


/*定义下面这句话, 将把收到的字符发送到调试串口1*/

#define COM_DEBUG     COM1    /* 调试串口 */
/*****************************************************/
#define COM_N720      COM2	  /* 选择串口 */
    
    
/* 定义下面这句话, 选择SIM卡运营商 */
#define SIM_Operator    Union    //Mobile移动   Union联通
    
/*定义接收短信手机号码*/    
#define Phone_Number "18888642945"
#define N720_BT_ID	0//基本定时器
#define N720_LT_ID	1//周期定时器

#define AT_CR		'\r'
#define AT_LF		'\n'
    
/* AT+CREG? 命令应答中的网络状态定义 	当前网络注册状态  SIM800_GetNetStatus() */
enum
{
    CREG_NO_REG     = 0,  	/* 0：没有注册，ME现在并没有在搜寻要注册的新的运营商 */
    CREG_LOCAL_OK   = 1,	/* 1：注册了本地网络 */
    CREG_SEARCH     = 2,	/* 2：没有注册，但MS正在搜寻要注册的新的运营商 */
    CREG_REJECT     = 3,	/* 3：注册被拒绝 */
    CREG_UNKNOW     = 4,	/* 4：未知原因 */
    CREG_REMOTE_OK  = 5,    /* 5：注册了漫游网络 */
};

#define MsgPwd "123456"
#define MsgPhone1 "18888642945"
#define MsgPhone2 "19910615217"





typedef struct
{
	char Manfacture[12];	/* 厂商 SIMCOM_Ltd */
	char Model[12];			/* 型号 SIM800 */
	char Revision[15 + 1];	/* 固件版本 R13.08 */
	//char IMEI[15 + 1];		/* IMEI 码 需要通过AT+GSN获得 */
}SIM800_INFO_T;


void bsp_InitN720_GPIO(void);
void N720_RST_GPIO(bool dat);
void N720_PCTRL_GPIO(bool dat);
void N720_ONOFF_GPIO(bool dat);

  
void N720_SendAT(char *_Cmd);
void N720_SendData(char *_Str, uint8_t Len);
uint8_t N720_WaitResponse(uint8_t *reply,uint16_t TimeOut);

uint8_t N720_Check_Report(void);
uint8_t N720_Forbidden_Command_Display(void);
uint8_t N720_Check_SIMCard(void);
uint8_t N720_Signal_Power(void);
uint8_t N720_Query_Network_Registration(void);
uint8_t N720_Query_Network_Attach(void);
uint8_t N720_Active_Attachment(void);
uint8_t N720_Get_Serial_Number(uint8_t *num);
uint8_t N720_Get_IMEI(uint8_t *imei);
uint8_t N720_Set_APN(void);
uint8_t N720_Set_USERPWD(void);
uint8_t N720_Enable_Reporting(void);
uint8_t N720_Network_Connection(void);

uint8_t N720_Check_Connection(void);
uint8_t N720_Close_Ftp(void);
uint8_t N720_Connect_Ftp(uint8_t *ip_port, uint8_t *ftp_name, uint8_t *ftp_pwd);
uint8_t N720_Check_Ftp_Connect(void);
uint32_t N720_Check_File_Size(uint8_t *file_name);
uint32_t N720_DownLoad_File(uint8_t *buf);

uint8_t N720_Set_TCP(uint8_t *ip_port);
uint8_t N720_Open_TCP(void);
uint8_t N720_Close_TCP(void);
uint8_t N720_TCP_Send(uint8_t *dat, uint16_t len);

uint8_t N720_Set_SMS_Mode(void);
uint8_t N720_Set_Char(void);
uint8_t N720_Reporting_Mode(void);
uint8_t N720_SMS_Delete(void);
uint8_t N720_SMS_Send(uint8_t *buf);

Enum_Message_JudgeAndGet N720_TCP_RX(uint8_t *buf, uint8_t *len);
Enum_Message_JudgeAndGet N720_SMS_RX(uint8_t state, uint8_t *buf, uint8_t *len);
Enum_Message_JudgeAndGet N720_Judge_Type(uint8_t *num);

Enum_SMS_Ctrl SMS_MsgResolve(uint8_t *buf, uint8_t len);


#ifdef __cplusplus
}
#endif
#endif
