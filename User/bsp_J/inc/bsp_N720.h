
#ifndef __bsp_N720_H
#define __bsp_N720_H

#ifdef __cplusplus
extern "C" {
#endif 
    
//#define bool _Bool
//#define true 1
//#define false 0
//    
//    
  
  
  
  
/* �����ڶ�Ӧ��RCCʱ�� */
#define RCC_ALL_N720 	(RCC_APB2Periph_GPIOC)

#define GPIO_PORT_N720_RST     GPIOC
#define GPIO_PIN_N720_RST      GPIO_Pin_1

#define GPIO_PORT_N720_PCTRL   GPIOC
#define GPIO_PIN_N720_PCTRL    GPIO_Pin_2

#define GPIO_PORT_N720_ONOFF   GPIOC
#define GPIO_PIN_N720_ONOFF    GPIO_Pin_3


  
  
  
  
  /* ����������仰, �����յ����ַ����͵����Դ���1 */

#define COM_DEBUG     COM1        /* ���Դ��� */
/*****************************************************/
#define COM_N720      COM2	  /* ѡ�񴮿� */
    
    
/* ����������仰, ѡ��SIM����Ӫ�� */
#define SIM_Operator    Mobile    //Mobile�ƶ�   Union��ͨ
    
#define N720_BT_ID	0//������ʱ��
#define N720_LT_ID	1//���ڶ�ʱ��

#define AT_CR		'\r'
#define AT_LF		'\n'
/* AT+CREG? ����Ӧ���е�����״̬���� 	��ǰ����ע��״̬  SIM800_GetNetStatus() */
enum
{
	CREG_NO_REG = 0,  	/* 0��û��ע�ᣬME���ڲ�û������ѰҪע����µ���Ӫ�� */
	CREG_LOCAL_OK = 1,	/* 1��ע���˱������� */
	CREG_SEARCH = 2,	/* 2��û��ע�ᣬ��MS������ѰҪע����µ���Ӫ�� */
	CREG_REJECT = 3,	/* 3��ע�ᱻ�ܾ� */
	CREG_UNKNOW = 4,	/* 4��δ֪ԭ�� */
	CREG_REMOTE_OK = 5, /* 5��ע������������ */
};



extern uint8_t check;
extern uint8_t check1;


void bsp_InitN720_GPIO(void);
void N720_RST_GPIO(bool dat);
void N720_PCTRL_GPIO(bool dat);
void N720_ONOFF_GPIO(bool dat);


  
void N720_SendAT(char *_Cmd);
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
uint8_t N720_Connect_Ftp(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2);
uint8_t N720_Check_Ftp_Connect(void);
uint32_t N720_Check_File_Size(uint8_t *file_name);
uint8_t N720_DownLoad_File(uint8_t *file_name, uint32_t addr, uint32_t size, uint8_t *buf);


uint8_t N720_Set_TCP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2);
uint8_t N720_Open_TCP(void);
uint8_t N720_Close_TCP(void);
uint8_t N720_TCP_Send(uint8_t *dat, uint8_t len);



#ifdef __cplusplus
}
#endif
#endif
