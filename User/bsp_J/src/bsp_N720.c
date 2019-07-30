#include "includes.h"




void bsp_InitN720_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* ��1������GPIOʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
  /* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	/* ���븡��ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_RST;
  GPIO_Init(GPIO_PORT_N720_RST, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_PCTRL;
  GPIO_Init(GPIO_PORT_N720_PCTRL, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN_N720_ONOFF;
  GPIO_Init(GPIO_PORT_N720_ONOFF, &GPIO_InitStructure);
  
  
  
}
 
void N720_RST_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_RST->BSRR = GPIO_PIN_N720_RST;
  }
  else
  {
    GPIO_PORT_N720_RST->BRR  = GPIO_PIN_N720_RST;
  }
}

void N720_PCTRL_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_PCTRL->BSRR = GPIO_PIN_N720_PCTRL;
  }
  else
  {
    GPIO_PORT_N720_PCTRL->BRR  = GPIO_PIN_N720_PCTRL;
  }
}

void N720_ONOFF_GPIO(bool dat)
{
  if(dat == true)
  {
    GPIO_PORT_N720_ONOFF->BSRR = GPIO_PIN_N720_ONOFF;
  }
  else
  {
    GPIO_PORT_N720_ONOFF->BRR  = GPIO_PIN_N720_ONOFF;
  }
}

/*
***********************************************************************
�� �� ��: NB_iot_SendAT
����˵��: ��GSMģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
��    ��: _Str : AT�����ַ�����������ĩβ�Ļس�<CR>. ���ַ�0����
�� �� ֵ: ��
ע    �ͣ�\n = ���� \r = �س� 
strlen�����Ľ�����һ���������Ĺ����������ڴ��ĳ��λ��
���������ַ�����ͷ���м�ĳ��λ�ã�������ĳ����ȷ�����ڴ�����
��ʼɨ�裬ֱ��������һ���ַ���������'\0'Ϊֹ��Ȼ�󷵻ؼ�����
ֵ(���Ȳ�����'\0')
**********************************************************************
*/

void N720_SendAT(char *_Cmd)
{
  comClearRxFifo(COM_N720);	/* ���㴮�ڽ��ջ����� */	
  comClearTxFifo(COM_N720);	/* ���㴮�ڷ��ͻ����� */	
  
  comSendBuf(COM_N720, (uint8_t *)_Cmd, strlen(_Cmd));
  comSendBuf(COM_N720, "\r", 1);
  comSendBuf(COM_N720, "\n", 1);
  
}

/*
**********************************************************************
�� �� ��: N720_WaitResponse
����˵��: �ȴ�N720����ָ����Ӧ���ַ���. ����ȴ� OK
��    ��: reply  : Ӧ����ַ����� ���Ȳ��ó���255
TimeOut : ����ִ�г�ʱ��0��ʾһֱ�ȴ�. >����ʾ��ʱʱ�䣬��λ1ms
�� �� ֵ: 1 ��ʾ�ɹ�  0 ��ʾʧ��
*********************************************************************
*/
uint8_t N720_WaitResponse(uint8_t *reply,uint16_t TimeOut)
{
  uint8_t ucRxBuf[256]={0};
  INT8U   err;
  
  OSSemPend(SempN720Rx, TimeOut, &err);
    
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,reply) != NULL)
    {
      return(true);
    }
  }
   
  return(false);
}

/*
**********************************************************************
�� �� ��: N720_Check_Report
����˵��: ����ģ�鴮��ͨѶ 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Check_Report(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT");      //AT���� 
    if( N720_WaitResponse("OK",1000) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Forbidden_Command_Display
����˵��: �رջ��Թ��� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Forbidden_Command_Display(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("ATE0");      //�رջ���
    if( N720_WaitResponse("OK",1000) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
�� �� ��: N720_Check_SIMCard
����˵��: ���SIM��״̬ 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
*********************************************************************
*/
uint8_t N720_Check_SIMCard(void)    //����SIM����ʼ����Ҫ��ʱ��Ͼã���ѭ����ѯ5�Σ�1s/��
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<5;num++)
  {
    N720_SendAT("AT+CPIN?");      //���SIM��״̬
    if( N720_WaitResponse("READY",1000) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Signal_Power
����˵��: ��ȡ�ź�ǿ�� 
��    ��: ��  
�� �� ֵ: �ź�ǿ�� ��0-31��
ע    �ͣ�
1�� ������ +CSQ: 99,99 ,�������źš� ��ѭ����ѯ40�Σ� 1s/�Σ�
2�� ������ +CSQ: 0< X <=31,�����ź�,����ѭ����ѯ��X < 12ʱ���ٽ�����һ��;
3�� ����ѯ40�κ������źţ� �򷵻� �� ������ߣ� ����ģ�顣
*********************************************************************
*/
uint8_t N720_Signal_Power(void)    //����SIM����ʼ����Ҫ��ʱ��Ͼã���ѭ����ѯ5�Σ�1s/�Σ�
{
  uint8_t rssi = 0;
  uint8_t ucRxBuf[32] = {0};
  INT8U   err;
  char cmd_str[16] = {0};
  
  N720_SendAT("AT+CSQ");      //��ȡ�ź�ǿ��
		
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
    {
      sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &rssi);    //��ȡ�ź�ǿ��
    }
  }
  
  return(rssi);
}

/*
**********************************************************************
�� �� ��: N720_Query_Network_Registration
����˵��: ��ѯ����ע����� 
��    ��: ��  
�� �� ֵ: ����ע����� ��1-5��
ע    �ͣ�
1�� ������: +CREG: 0,1"�� +CREG: 0,5"��ʾģ����ע����CS�� �ɽ�����һ����
2�� ������ +CREG:0,<0,2,3,4>"�� ��δע����CS�� ѭ����ѯ40�Σ� 1s/�Σ�
3�� ��ѭ����ѯ40�κ���ע�᲻�����磬 ���ز��� �� ����ģ�飻
*********************************************************************
*/
uint8_t N720_Query_Network_Registration(void)    
{
  uint8_t log = 0;
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[32] = {0};
  INT8U   err;
  
  N720_SendAT("AT+CREG?");      //��ѯ����ע�����
  
  OSSemPend(SempN720Rx, 5000, &err);
  if( err != OS_ERR_TIMEOUT )
  {
    comGetBuf(COM_N720, ucRxBuf);
    if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
    {
      sscanf(ucRxBuf, "%[^,],%d", cmd_str, &log);    //ע�����
    }
  }

  return(log);
}


/*
**********************************************************************
�� �� ��: N720_Query_Network_Attach
����˵��: ��ѯ���總����� 
��    ��: ��  
�� �� ֵ: ���總����� ��0-1��
ע    �ͣ�
1�� ������ +CGATT:1 �� �����總�ųɹ��� �ɽ�����һ����
2�� ������ +CGATT:0 �� ��ѭ����ѯ10�Σ� 1s/�Σ�
3�� ��ѯ10�κ� �Է��� +CGATT:0 �� �ɽ����������ţ� ���岽��Ϊ��
A�� ��ȥ���ţ� ����AT+CGATT=0�� �ȴ�����OK��
B����ȥ���ŷ���OK�� ���������ţ� AT+CGATT=1��
*********************************************************************
*/
uint8_t N720_Query_Network_Attach(void)    
{
  uint8_t attach = 0;
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  
  N720_SendAT("AT+CGATT?");      //��ѯ���總�����
  
  bsp_StartTimer(N720_BT_ID, 300);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
      {
        sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &attach);    //��ȡ�������
        if (attach != NULL)
        {
          break;
        }
      }
    }
    /* ��鶨ʱ��0ʱ���Ƿ� */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      attach = 0;
      break;/* 3�붨ʱ�����˳���״̬ */
    }
  }
  
  /* �������ʱ��Ӧ�ùرն�ʱ������Ϊ���ǻ�ռ�ú�̨����Դ */
  bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
  
  return(attach);
}

/*
**********************************************************************
�� �� ��: N720_Active_Attachment
����˵��: ģ�������������總�� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ��
ע    �ͣ���ȥ���ţ��ٽ��и���
*********************************************************************
*/
uint8_t N720_Active_Attachment(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT+CGATT=0");      //����ȥ����
    if( N720_WaitResponse("OK",300) == true)
    {
      N720_SendAT("AT+CGATT=1");      //���總��
      if( N720_WaitResponse("OK",300) == true)
      {
        flag = 1;
        break;
      }
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}



/*
**********************************************************************
�� �� ��: N720_Get_Serial_Number
����˵��: ��ȡ SIM �����к� 
��    ��: num  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע    �ͣ�
*********************************************************************
*/
uint8_t N720_Get_Serial_Number(uint8_t *num)    
{
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYCCID");      //��ȡ SIM �����к� 
  
  bsp_StartTimer(N720_BT_ID, 300);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
      {
        sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, num);    //��ȡ�������
        flag = 1;
        break;
      }
      else 
      {
        flag = 0;
      }
    }
    /* ��鶨ʱ��0ʱ���Ƿ� */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      flag = 0;
      break;/* 3�붨ʱ�����˳���״̬ */
    }
  }
  
  /* �������ʱ��Ӧ�ùرն�ʱ������Ϊ���ǻ�ռ�ú�̨����Դ */
  bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
  
  return flag;
}

/*
**********************************************************************
�� �� ��: N720_Get_IMEI
����˵��: ��ȡģ��IMEI�� 
��    ��: *imei  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע    �ͣ�
*********************************************************************
*/
uint8_t N720_Get_IMEI(uint8_t *imei)    
{
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  uint8_t flag = 0;
  
  N720_SendAT("AT+CGSN");      //��ȡ SIM �����к� 
  
  bsp_StartTimer(N720_BT_ID, 300);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
      {
        sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, imei);    //��ȡ�������
        flag = 1;
        break;
      }
      else 
      {
        flag = 0;
      }
    }
    /* ��鶨ʱ��0ʱ���Ƿ� */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      flag = 0;
      break;/* 3�붨ʱ�����˳���״̬ */
    }
  }
  
  /* �������ʱ��Ӧ�ùرն�ʱ������Ϊ���ǻ�ռ�ú�̨����Դ */
  bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
  
  return flag;
}


/*
**********************************************************************
�� �� ��: N720_Set_APN
����˵��: ����APN���� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ����ģ����ʹ�õ�SIM���� ���ö�Ӧ��APN
*********************************************************************
*/
uint8_t N720_Set_APN(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
#if     SIM_Operator == Mobile
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"CMNET\"");      //����APN����
#else
    N720_SendAT("AT$MYNETCON=0,\"APN\",\"3gnet\"");      //����APN����
#endif
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Set_USERPWD
����˵��: ����APN�û��������� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ��������ҵ���û��������룬 ��ͬ��������ҵ���û��������벻ͬ��
2�� ����ָ��������ã�
*********************************************************************
*/
uint8_t N720_Set_USERPWD(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETCON=0,\"USERPWD\",\"gsm,1234\"");      //�����û���������
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Enable_Reporting
����˵��: ���������ϱ� 
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ���ڲ�Э��ջ�£� ���������ϱ����ܣ�
2�� ģ����Զ��ϱ����缤� ���ݶ�ȡ�� ��·�Ͽ�����Ϣ��
*********************************************************************
*/
uint8_t N720_Enable_Reporting(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETURC=1");      //���������ϱ�
    
    if( N720_WaitResponse("OK",300) == true)
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Network_Connection
����˵��: ������������
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� �������Ӻ�ȴ����룬 �ڼ䲻Ҫִ�������κβ����� ��ʱʱ��Ϊ30s;
2�� ������OK���ʾָ��ִ�гɹ��� ��ȴ������Ļ��룬 ���жϼ����������ӳɹ����
3�� ������$MYURCACT: 0,1,"IP"����ERROR 902�� �򼤻��������ӳɹ�,�ɽ�����һ����
4�� ����ʱʱ�䵽��δ����$MYURCACT: 0,1,"IP"�� ���������3�Σ� 10s/�Σ�
5�� �����¼���3����δ���IP�� ������ģ�飻
*********************************************************************
*/
uint8_t N720_Network_Connection(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYNETACT=0,1");      //������������
    
    if( (N720_WaitResponse("$MYURCACT",10000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
�� �� ��: N720_Check_Connection
����˵��: ��ѯ��������
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������$MYNETACT: 0,1,"IP"�� �����������Ѽ�� ��δ�Ͽ���
2�� ������$MYNETACT: 0,1,"0,0,0,0"�� ������������δ��� �ɼ�����ѯ5�Σ� 1s/�Σ�
3�� ����ѯ5�Σ� �Է���$MYNETACT: 0,1,"0,0,0,0"�� �����¼����������ӣ�
*********************************************************************
*/
uint8_t N720_Check_Connection(void)
{
  uint8_t ucRxBuf[128] = {0};
  char cmd_str[16] = {0};
  char ip_str[16] = {0};
  uint8_t flag = 0;
  for (uint8_t i = 0; i < 5; i++)
  {
    N720_SendAT("AT$MYNETACT?");      //��ѯ�������� 
    
    bsp_StartTimer(N720_BT_ID, 1000);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
    W_State(&SysState,N720RcvOK,false);
    while (1)
    {			
      if(R_State(&SysState,N720RcvOK) == true)
      {
        W_State(&SysState,N720RcvOK,false);
        comGetBuf(COM_N720, ucRxBuf);
        if(strstr(ucRxBuf,"OK") != NULL)   //�ظ�OK
        {
          sscanf(ucRxBuf, "%[^\"]\"%[^\"]", cmd_str, ip_str);    //��ȡip��
          //flag = 1;
          break;
        }
        else 
        {
          flag = 0;
        }
      }
      /* ��鶨ʱ��0ʱ���Ƿ� */
      if (bsp_CheckTimer(N720_BT_ID))
      {
        flag = 0;
        break;/* 3�붨ʱ�����˳���״̬ */
      }
    }
    /* �������ʱ��Ӧ�ùرն�ʱ������Ϊ���ǻ�ռ�ú�̨����Դ */
    bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
    
    if(strstr(ip_str,"0.0.0.0") == NULL)
    {
      flag = 1;
      break;
    }
    else 
    {
      flag = 0;
    }
  }
  return flag;
}

/*
**********************************************************************
�� �� ��: N720_Close_Ftp
����˵��: �ر�Ftp����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� �ȹ�һ�����FTP����,���⽨��FTP����ʧ�ܣ�
2�� ����ָ��Ի�������жϣ� �յ������ ��ʱ5-20s���ɽ�����һ����
*********************************************************************
*/
uint8_t N720_Close_Ftp(void)
{
  uint8_t flag = 0;
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT("AT$MYFTPCLOSE");      //�ر�Ftp����
    
    if( (N720_WaitResponse("OK",3000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Connect_Ftp
����˵��: ��Ftp����
��    ��: ip1��ip2��ip3��ip4��port1��port2  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ���������ȴ����룬 ��ʱʱ��Ϊ10s�� �ڼ䲻Ҫִ�������κβ�����
2�� ������ OK ,��ʾ��������ɹ��� ���Խ�����һ����
3�� ������ ERROR:<�������> �� �ο���1���¿�������2s/�Σ��� �ظ�����3�Σ�
4�� ���ظ�����3����ʧ�ܣ� �򷵻ز��� ��ѯģ���Ƿ񼤻��������ӣ�
*********************************************************************
*/
uint8_t N720_Connect_Ftp(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2)
{
  uint8_t flag = 0;
  uint8_t cmd_str[64] = {0};
  B16_B08 Port;
  
  
  Port.B8[0] = port1;
  Port.B8[1] = port2;
  
  snprintf(cmd_str, 64, "AT$MYFTPOPEN=0,\"%d.%d.%d.%d:%d\",\"ftp\",\"ftp\",1,30,1", ip1, ip2, ip3, ip4, Port.B16);
  
  for(uint8_t num=0;num<3;num++)
  {
    N720_SendAT(cmd_str);      //��Ftp����
    
    if( (N720_WaitResponse("OK",10000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  
  return(flag);
}


/*
**********************************************************************
�� �� ��: N720_Check_Ftp_Connect
����˵��: ��ѯFTP������״̬
��    ��: 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������ $MYFTPOPEN:0 ���ʾFTP�����Ѿ������� ���Խ�����һ����
2�� ������ ERROR ���ʾFTP����δ������ �����½������ӣ�
*********************************************************************
*/
uint8_t N720_Check_Ftp_Connect(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYFTPOPEN?");      //��ѯFTP������״̬
  
  if( (N720_WaitResponse("$MYFTPOPEN",300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Check_File_Size
����˵��: ��ѯFtp���������ļ���С
��    ��: * file_name �ļ�����   
�� �� ֵ: ����1��ʾ�ɹ�  false��ʾʧ�� 
ע�ͣ�
1�� ������ $MYFTPSIZE�� <File_length> ,��ɹ���ȡFTP�ļ���С�� �ɽ�����һ����
2�� ������ ERROR �� ����Ҫ������ѯ10�Σ�1s/�Σ���
3�� ����ѯ10�Σ� ��δ���� $MYFTPSIZE�� <File_length> �� �򷵻ز�ѯFTP����״̬��
*********************************************************************
*/
uint32_t N720_Check_File_Size(uint8_t *file_name)
{
  uint32_t file_size = 0;
  uint8_t cmd_str[32] = {0};
  uint8_t ucRxBuf[128] = {0};
  
  snprintf(cmd_str, 32, "AT$MYFTPSIZE=\"%s\"", file_name);
  
  N720_SendAT(cmd_str);      //��ѯFtp���������ļ���С
  
  bsp_StartTimer(N720_BT_ID, 1000);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
  W_State(&SysState,N720RcvOK,false);
  while (1)
  {			
    if(R_State(&SysState,N720RcvOK) == true)
    {
      W_State(&SysState,N720RcvOK,false);
      comGetBuf(COM_N720, ucRxBuf);
      
      if(strstr(ucRxBuf,"$MYFTPSIZE") != NULL)   //
      {
        sscanf(ucRxBuf, "%[^ ] %d", cmd_str, &file_size);    //��ȡ�ļ���С��Ϣ
        if (file_size != NULL)
        {
          break;
        }
      }
    }
    /* ��鶨ʱ��0ʱ���Ƿ� */
    if (bsp_CheckTimer(N720_BT_ID))
    {
      file_size = 0;
      break;/* 3�붨ʱ�����˳���״̬ */
    }
    
  }
  
  /* �������ʱ��Ӧ�ùرն�ʱ������Ϊ���ǻ�ռ�ú�̨����Դ */
  bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
  
  return(file_size);
  
}

/*
**********************************************************************
�� �� ��: N720_DownLoad_File
����˵��: ��������
��    ��: * file_name �ļ�����  addr �ļ���ʼ��ַ   size ����     *buf ���ص�����
�� �� ֵ: true��ʾ�ɹ�  false��ʾʧ�� 
ע�ͣ�
1�� ��ָ�ֶ���֧�ֶַ����أ�
��<data_offset>,<data_Length>��ȱʡʱ���������ļ���
����ȱʡ<data_Length>ʱ�� ����<data_offset>���ļ���β��ȫ�����ݣ�
2�� ������ CONNECT ,��ʼ�����������ݣ�
3�� Ҳ��ʹ�� +++ �˳�FTP�ϴ�ģʽ;
3�� ���͸�ָ��� ������ ERROR: <�������> ,��ο���1�������������ݣ�
*********************************************************************
*/

uint8_t check = 0;
uint8_t check1 = 0;
uint8_t N720_DownLoad_File(uint8_t *file_name, uint32_t addr, uint32_t size, uint8_t *buf)
{
  uint8_t flag = 1;
  uint8_t cmd_str[48] = {0};
  uint8_t ucRxBuf[3*1024] = {0};
  uint16_t data_len = 0;
  uint16_t data_size = 0;
  uint16_t data_size_last = 0;
  snprintf(cmd_str, 48, "AT$MYFTPGET=\"%s\",%d,%d", file_name, addr, size);
  
  N720_SendAT(cmd_str);      //��������
  
  
  if( (N720_WaitResponse("CONNECT",1000) == true))
  {
    bsp_StartTimer(N720_BT_ID, 10000);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
    W_State(&SysState,N720RcvOK,false);
    comClearRxFifo(COM_N720);	/* ���㴮�ڽ��ջ����� */	
    while (1)
    {			
      if(R_State(&SysState,N720RcvOK) == true)
      {
        W_State(&SysState,N720RcvOK,false);
        data_len = comGetBuf(COM_N720, ucRxBuf);
        
        if (data_len >= size)
        {
          memcpy(&(buf[0]), ucRxBuf, data_len-6);//�������ߵ�����֡
          flag = 1;
          break;
        }
        
        else 
        {
          data_size_last = data_size;
          data_size += data_len;
          if (data_size >= size)
          {
            memcpy(&(buf[0 + data_size_last]), ucRxBuf, data_len-6);//�������ߵ�����֡
            flag = 1;
            break;
          }
          
          memcpy(&(buf[0 + data_size_last]), ucRxBuf, data_len);//�������ߵ�����֡
          
        }
        
        comClearRxFifo(COM_N720);	/* ���㴮�ڽ��ջ����� */	 
      }
      /* ��鶨ʱ��0ʱ���Ƿ� */
      if (bsp_CheckTimer(N720_BT_ID))
      {
        flag = 0;
        break;/* 3�붨ʱ�����˳���״̬ */
      }
    }  
  }
  else
  {
    flag = 0;
  }
  bsp_StopTimer(N720_BT_ID);	 //���ζ�ʱ�������ʱ����һ�κ󣬿��Բ��عر�
  
  return(flag);
  
}




/*
**********************************************************************
�� �� ��: N720_Set_TCP
����˵��: ����TCP��·��Ϣ
��    ��: ip1��ip2��ip3��ip4��port1��port2  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�����͸�ָ�������ء�OK������ָ��ͳɹ���������һ��
*********************************************************************
*/
uint8_t N720_Set_TCP(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint8_t port1, uint8_t port2)
{
  uint8_t flag = 0;
  uint8_t cmd_str[48] = {0};
  B16_B08 Port;
  
  
  Port.B8[0] = port1;
  Port.B8[1] = port2;
  
  snprintf(cmd_str, 48, "AT$MYNETSRV=0,0,0,0,\"%d.%d.%d.%d:%d\"", ip1, ip2, ip3, ip4, Port.B16);
  
  N720_SendAT(cmd_str);      //����TCP��·��Ϣ
  
  if( (N720_WaitResponse("OK",300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }
  
  return(flag);
}

/*
**********************************************************************
�� �� ��: N720_Open_TCP
����˵��: ��TCP����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
1�� ������ $MYNETOPEN: 0,2000 ���ʾTCP�����Ѿ������� ���Խ�����һ����
2�� ������ ERROR ���ʾFTP����δ�����������½������ӣ�
*********************************************************************
*/
uint8_t N720_Open_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETOPEN=0");      //��TCP����
  
  for(uint8_t num=0;num<3;num++)
  {
    if( (N720_WaitResponse("2000",10000) == true))
    {
      flag = 1;
      break;
    }
    else
    {
      flag = 0;
    }
  }
  return(flag);
}


/*
**********************************************************************
�� �� ��: N720_Close_TCP
����˵��: �ر�TCP����
��    ��: ��  
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
*********************************************************************
*/
uint8_t N720_Close_TCP(void)
{
  uint8_t flag = 0;
  
  N720_SendAT("AT$MYNETCLOSE=0");      //�ر�TCP����
  
  if( (N720_WaitResponse(NULL,300) == true))
  {
    flag = 1;
  }
  else
  {
    flag = 0;
  }
  return(flag);
}


/*
**********************************************************************
�� �� ��: N720_TCP_Send
����˵��: ����TCP����
��    ��:  *buf Ҫ���͵�����  len ���ݳ��� 
�� �� ֵ: true��ʾ�ɹ�  false ��ʾʧ�� 
ע�ͣ�
*********************************************************************
*/
uint8_t N720_TCP_Send(uint8_t *dat, uint8_t len)
{
  uint8_t flag = 0;
  uint8_t cmd_str[32] = {0};
  
  
  snprintf(cmd_str, 32, "AT$MYNETWRITE=0,%d", len);
  N720_SendAT(cmd_str);      //����TCP����
  
  if( (N720_WaitResponse("$MYNETWRITE",10000) == true))
  {
    N720_SendAT(dat);      //��������
    if ( N720_WaitResponse("OK",1000) == true )
    {
      flag = 1;
    }
  }
  else
  {
    flag = 0;
  }
  return(flag);
}




