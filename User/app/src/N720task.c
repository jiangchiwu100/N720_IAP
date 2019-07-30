#include "includes.h"

Enum_Message_JudgeAndGet JudgeAndGet;


OS_STK AppTaskSMSCommStk[APP_TASK_SMSComm_STK_SIZE];	
OS_STK AppTaskN720InitStk[APP_TASK_N720Init_STK_SIZE];	


void AppTaskSMSComm(void *p_arg)
{
    (void)p_arg;	 /* ����������澯 */
    INT8U   err;    
        
    while (1) 
    {
//        OSTimeDly(0);
        OSSemPend(SempSMSRx, 0, &err);
        if( err != OS_ERR_TIMEOUT )
        { 
            SMS_Comm();
        }
    }
}

void SMS_Comm(void)
{
    static Enum_SMS_Ctrl SMSCtrlStateMachine = 0;
    
    SMSCtrlStateMachine = SMS_MsgResolve(SMSbuf.buf, SMSbuf.len);
    
    switch(SMSCtrlStateMachine)
    {
      case SMS_Info_Set:            //���������Ϣ
        {
            SMSCtrlStateMachine = 0;
            CreateGateWayConfigInfoFile();      //д�뵽�ļ�ϵͳ��
            NVIC_SystemReset();
        }
        break;
      case SMS_Software_Reset:      //����
        {
            SMSCtrlStateMachine = 0;
            /******
            /
            /
            /
            /
            ******/
        }
        break;
      case SMS_Hardware_Reset:      //MCU��λ
        {
            SMSCtrlStateMachine = 0;
            NVIC_SystemReset();
        }
        break;
      case SMS_IAP_Ctrl:            //IAP����
        {
            SMSCtrlStateMachine = 0;
            iap_load_app();     //��ת��IAP
        }
        break;        
      default:
        SMSCtrlStateMachine = 0;
        break;
    }
}

void AppTaskN720Init(void *p_arg)
{
  static uint32_t N720InitDelay = 0;
  (void)p_arg;	 /* ����������澯 */
  
  while (1) 
  {
      N720InitDelay = N720_Init();//Init
      if(N720InitDelay == Task_Suspend)
      {
          OSTaskSuspend(APP_TASK_N720Init_PRIO);   
      }
      else
      {
          OSTimeDly(N720InitDelay);
      }
  }
}


uint32_t N720_Init(void)
{
    uint32_t delay = 0;
    static uint16_t getMsg_cnt = 1;
    static Enum_N720_Init N720InitAllStateMachine = Enum_N720_Check_AT;
    
    switch (N720InitAllStateMachine)
    {
      case Enum_N720_Check_AT:    //���ģ��ͨѶ
        {
			W_State(&SysState,N720Register,false);
            W_State(&SysState,TCPRegister,false);
            printf("*******Enum_N720_Check_AT**********\n\r");
            
            if (N720_Forbidden_Command_Display() == true)
            {
                N720InitAllStateMachine = Enum_N720_Check_Internet;   //ģ��ͨѶ�ɹ� ��ʼ�������
                delay = 1000;
            }
            else 
            {
                N720InitAllStateMachine = Enum_N720_Init_State;    //ģ��ͨѶʧ�� ��ʼ��ģ��
                delay = 1000;              
            }
        }
        break;   
      case Enum_N720_Check_Internet:    //���ģ������״̬
        {
			W_State(&SysState,N720Register,false);
            printf("*******Enum_N720_Check_Internet**********\n\r");
            
            if (N720_Check_Connection() == true)
            {
                N720InitAllStateMachine = Enum_N720_Init_SMS;   //ģ������ ֱ�ӿ�ʼ���Ͷ���
                delay = 1000;
            }
            else 
            {
                N720InitAllStateMachine = Enum_N720_Init_State;    //ģ�鲻���� ��ʼ��ģ��
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Init_State:     //��ʼ��N720״̬
        {
			W_State(&SysState,N720Register,false);
            printf("*******Enum_N720_Init_State**********\n\r");
            
            delay = N720_State_Init();
            if (delay == Init_Success)
            {
                N720InitAllStateMachine = Enum_N720_Init_SMS;   //ģ���ʼ����� ��ʼ��ʼ����������
                delay = 1000;
            }
			else if(delay == Init_Fail)//4G�й���   1����֮���ٽ��г�ʼ�� 
			{
				delay = 60000;
				N720InitAllStateMachine = Enum_N720_Check_AT;
			}
            else 
            {
                N720InitAllStateMachine = Enum_N720_Init_State;    //ģ���ʼ����δ��� ������ʼ�� 
            }
        }
        break;
      case Enum_N720_Init_SMS:      //��ʼ����������
        {
            printf("*******Enum_N720_Init_SMS**********\n\r");
            
            delay = N720_SMS_Init();
            if (delay     == SMS_Success)       //�������ó�ʼ����� ��ʼ��ʼ��TCP
            {                
//				W_State(&SysState,N720Register,true);
                N720InitAllStateMachine = Enum_N720_Init_TCP;                    
                delay = 1000;
            }
            else if (delay == SMS_Fail)         //�������ó�ʼ��ʧ�� ��ʼ��ʼ��N720
            {
				W_State(&SysState,N720Register,false);
				delay = 1000;
                N720InitAllStateMachine = Enum_N720_Init_State;     
            }
            else
            {
				W_State(&SysState,N720Register,false);
                N720InitAllStateMachine = Enum_N720_Init_SMS;//�������ó�ʼ����δ��� ������ʼ�� 
            }
        }
        break;
      case Enum_N720_Init_TCP:      //��ʼ��TCP����
        {
            printf("*******Enum_N720_Init_TCP**********\n\r");
            
            if(_memcmp(&ConfigInfo.TcpIpPort[0] , 0 ,22) == false)//���IP��ַ���ǿ� �ͽ�������TCP  
            {
                delay = N720_TCP_Init();
                if(delay      == TCP_Success)    //TCP��ʼ���ɹ� ��ʼ����� ����
                {
                    W_State(&SysState,TCPRegister,true);
                    N720InitAllStateMachine = Enum_N720_JudgeAndGet;
                    delay = 0;
                }
                else if(delay == TCP_Fail)       //˵��IP��ַ���� ������������������  ���ſ���ʹ��
                {
                    N720InitAllStateMachine = Enum_N720_JudgeAndGet;     //TCP���ó�ʼ��ʧ�� ��ʼ��ʼ��N720
                    delay = 0;                
                }
                else
                {
                    N720InitAllStateMachine = Enum_N720_Init_TCP;    //TCP���ó�ʼ����δ��� ������ʼ��                 
                }                
            }
            else //�������շ�TCP���� �ȴ�������Ϣ
            {
				delay = 1000;   
                N720InitAllStateMachine = Enum_N720_JudgeAndGet;            
            }
        }
        break;
      case Enum_N720_JudgeAndGet:
        {            
            delay = N720_JudgeAndGet();    
            

            
            if(delay == Msg_Finish)     //��Ϣ������� ��������
            {
                printf("*******Enum_N720_Judge_Msg_Type : %d**********\n\r", getMsg_cnt++);

                delay = 0;
                N720InitAllStateMachine = Enum_N720_JudgeAndGet;
            }            
            else if(delay == Msg_Fail)  //��Ϣ����ʧ�� ͨѶ���� ��ʼ��ģ��      
            {
                delay = 3000;
                N720InitAllStateMachine = Enum_N720_Check_AT;
            }            
            else                        //�м���� ��������
            {
                N720InitAllStateMachine = Enum_N720_JudgeAndGet;
            }            
        }
        break;
      default:
        break; 
    }
    
    return delay;
}



/**********  N720_SendAT("AT+CMGD=0,4");   //������ж���**/
uint32_t N720_State_Init(void)//������ʱʱ��  ms
{
    uint32_t delay = 0;
    static Enum_N720_State N720InitStateMachine = Enum_N720_PowerOn_Disable;
    static uint8_t N720InitLoop = 0;
    static uint8_t AttachLoop = 0;
    static uint8_t Reset_flag = 0;
    
    switch(N720InitStateMachine)
    {
      case Enum_N720_PowerOn_Disable:     //����poweron���� �����ɹ� Ȼ����ʱ15s 
        { 
            N720InitLoop ++;            
            if (N720InitLoop < 2)
            {
                N720_RST_GPIO(false);
                delay = 1000;
                printf("********%d*******************\n\r", N720InitStateMachine);
            }
            else 
            {
                Reset_flag ++;      //��������
                N720InitLoop = 0;
                N720_RST_GPIO(true);
                N720InitStateMachine = Enum_N720_Check_Report;
                printf("********%d*******************\n\r", N720InitStateMachine);
                delay = 15000;                
            }
            
            if (Reset_flag > 3)     //��������3�� ��ʼ��ʧ��
            {
				N720InitLoop = 0;
				AttachLoop = 0;
				Reset_flag = 0;
				delay = Init_Fail; 
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
            }
            
        }
        break;
      case Enum_N720_Check_Report:    //�����͡�AT����鴮���Ƿ���ͨ
        {
            if (N720_Check_Report() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_FCDisplay;     //����ͨѶ�ɹ� �رջ���
                delay = 1000;
            }
            else
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;   //ʧ�� ����ģ��
                delay = 1000;
            }	
        }
        break;
      case Enum_N720_FCDisplay:         //�رջ���
        {
            if (N720_Forbidden_Command_Display() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Check_SIMCard;     //���SIM��
                delay = 1000;
                
            }
            else 
            {
                N720InitStateMachine = Enum_N720_Check_Report;      //�ٴμ�⴮��ͨѶ
                delay = 1000;
            }
            
        }
        break;
      case Enum_N720_Check_SIMCard:         //���SIM��
        {
            if (N720_Check_SIMCard() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Query_Net_Reg;     //�������ע��
                delay = 1000;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;   //ʧ�� ����ģ��
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Query_Net_Reg:     //�������ע��
        {
            if (N720_Query_Network_Registration() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Query_Net_Attach;     //������總��
                delay = 500;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;    //ʧ�� ����ģ��
                delay = 1000;              
            }
        }
        break;  
      case Enum_N720_Query_Net_Attach:      //������總��
        {
            
            if (N720_Query_Network_Attach() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                AttachLoop = 0;
                N720InitStateMachine = Enum_N720_Enable_Reporting;     //���������ϱ�
                delay = 500;
            }
            else 
            {
                AttachLoop++;       //����ʧ�ܼ���
                if (N720_Active_Attachment() == true)   //������������
                {
                    N720InitStateMachine = Enum_N720_Query_Net_Attach;  //�ɹ� ��鸽��״̬
                    delay = 3000;
                }
                if (AttachLoop >= 10)
                {
					AttachLoop = 0;
                    N720InitStateMachine = Enum_N720_PowerOn_Disable;   //ʧ�� ����ģ��
                }
                
            }
            
        }
        break;
      case Enum_N720_Enable_Reporting:      
        {
            if (N720_Enable_Reporting() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Set_APN;     //����APN����
                delay = 300;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;      //ʧ�� ����ģ��
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Set_APN:
        {
            if (N720_Set_APN() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Set_USERPWD;     //����APN�û�������
                delay = 300;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Set_USERPWD:
        {
            if (N720_Set_USERPWD() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Net_Connection;     //��������
                delay = 300;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Net_Connection:
        {
            if (N720_Network_Connection() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
                
                N720InitStateMachine = Enum_N720_Check_Connection;     //�������
                delay = 300;
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Check_Connection:
        {
            if (N720_Check_Connection() == true)
            {
                printf("********%d*******************\n\r", N720InitStateMachine);
				N720InitLoop = 0;
				AttachLoop = 0;
				Reset_flag = 0;
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
                delay = Init_Success;   //�����ɹ� ��ʼ�����
            }
            else 
            {
                N720InitStateMachine = Enum_N720_PowerOn_Disable;
                delay = 1000;              
            }
        }
        break;
      default:
        break;
    }
    return(delay);
}

uint32_t N720_TCP_Init(void)
{
    uint32_t delay = 0;
    static Enum_N720_TCP N720TCPStateMachine = Enum_N720_Set_TCP;
    static uint8_t data[6] = {0};
    static uint8_t TCP_Init_flag = 0;
    
    switch(N720TCPStateMachine)
    {
      case Enum_N720_Close_TCP:
        {
            TCP_Init_flag++;
            if (N720_Close_TCP() == true)
            {
                printf("********%d*******************\n\r", N720TCPStateMachine);
                
                N720TCPStateMachine = Enum_N720_Set_TCP;
                delay = 1000;
            }
            else 
            {
                N720TCPStateMachine = Enum_N720_Set_TCP;
                delay = 1000;              
            }
            if(TCP_Init_flag > 3)
            {
				delay = TCP_Fail;  
                TCP_Init_flag = 0;
                N720TCPStateMachine = Enum_N720_Close_TCP;
            }
        }
        break;
      case Enum_N720_Set_TCP:
        {
            if (N720_Set_TCP(ConfigInfo.TcpIpPort) == true)
            {
                printf("********%d*******************\n\r", N720TCPStateMachine);
                printf("%s\n", data);
                N720TCPStateMachine = Enum_N720_Open_TCP;
                delay = 300;
            }
            else 
            {
                N720TCPStateMachine = Enum_N720_Close_TCP;
                delay = 1000;              
            }
        }
        break;
      case Enum_N720_Open_TCP:
        {
            if (N720_Open_TCP() == true)
            {
                printf("********%d*******************\n\r", N720TCPStateMachine);
                TCP_Init_flag = 0;
                N720TCPStateMachine = Enum_N720_Close_TCP;
                delay = TCP_Success;
            }
            else 
            {
                N720TCPStateMachine = Enum_N720_Close_TCP;
                delay = 1000;              
            }            
        }
        break;
      default:
        break;
    }
    
    return delay;
}

uint32_t N720_SMS_Init(void)
{
    uint32_t delay = 0;
    static Enum_N720_SMS N720SMSStateMachine = Enum_N720_Set_SMS_Mode;
    static uint8_t SMS_Init_flag = 0;
    
    switch (N720SMSStateMachine)
    {
      case Enum_N720_Set_SMS_Mode:      //���ö���ģʽ
        {
            printf("**********Enum_N720_Set_SMS_Mode*************\n");
            SMS_Init_flag++;            //��ʼ��SMS����
            if (N720_Set_SMS_Mode() == true)
            {
                delay = 1000;
                N720SMSStateMachine = Enum_N720_Set_Char;
            }
            else 
            {
                delay = 1000;
                N720SMSStateMachine = Enum_N720_Set_SMS_Mode;                
            }
            
            if(SMS_Init_flag > 3)       //��ʼ���������� ʧ��
            {
                SMS_Init_flag = 0;
                delay = SMS_Fail;
                N720SMSStateMachine = Enum_N720_Set_SMS_Mode;
            }
        }
        break;
      case Enum_N720_Set_Char:      //�����ַ���
        {
            printf("**********Enum_N720_Set_Char*************\n");
                        
            if (N720_Set_Char() == true)
            {
                delay = 1000;
                N720SMSStateMachine = Enum_N720_Reporting_Mode;
            }
            else 
            {
                delay = 1000;
                N720SMSStateMachine = Enum_N720_Set_SMS_Mode;                
            }            
        }
        break;
      case Enum_N720_Reporting_Mode:        //���ö��ŵ��������ϱ�ģʽ
        {
            printf("**********Enum_N720_Reporting_Mode*************\n");
            
            if (N720_Reporting_Mode() == true)
            {
				SMS_Init_flag = 0;
                delay = SMS_Success;
                N720SMSStateMachine = Enum_N720_Set_SMS_Mode;
            }
            else 
            {
                delay = 1000;
                N720SMSStateMachine = Enum_N720_Set_SMS_Mode;                
            }
        }
        break;
      default:
        break;
    }
    return delay;
}


uint32_t N720_JudgeAndGet(void)
{
    uint32_t delay = 0;
    static uint8_t Msg_Num = 0;
    static Enum_Message_JudgeAndGet JudgeAndGetStateMachine = Null_Type;
    
    static uint8_t Get_SMS_flag = 0;
    static uint8_t Get_TCP_flag = 0;
    
    switch(JudgeAndGetStateMachine)
    {
      case Null_Type:           //����Ϣ �ȴ���Ϣ����
        {
            JudgeAndGetStateMachine = N720_Judge_Type(&Msg_Num);     //�ж���Ϣ���� ��ȡ��ϢID��
            delay = 0;
        }
        break;
      case SMS_Type:            //��ȡSMS��Ϣ
        {
            Get_SMS_flag++;     
            JudgeAndGetStateMachine = N720_SMS_RX(Msg_Num, SMSbuf.buf, &SMSbuf.len);//��ȡSMS��Ϣ
            delay = 0;
        }
        break;
      case TCP_Type:            //��ȡTCP��Ϣ
        {
            Get_TCP_flag++;
            JudgeAndGetStateMachine = N720_TCP_RX(TCPbuf.buf, &TCPbuf.len);         //��ȡTCP��Ϣ
            delay = 0;
        }
        break;
      case Get_SMS_Success:     //����SMS��Ϣ�ɹ�
        {
            OSSemPost(SempSMSRx);     //�����ź��� SMS 
            N720_SMS_Delete();
            delay = Msg_Finish;
            JudgeAndGetStateMachine = Null_Type;
        }
        break;
      case SMS_Error:           //����SMS��Ϣ����
        {
            N720_SMS_Delete();      //���������Ϣ
            delay = Msg_Finish;
            JudgeAndGetStateMachine = Null_Type;
        }
        break;
      case Get_SMS_Fail:        //����SMS��Ϣʧ�� ����3��
        {
            if(Get_SMS_flag > 3)
            {
                Get_SMS_flag = 0;
                delay = Msg_Fail;
                JudgeAndGetStateMachine = Null_Type;
            }
            else
            {
                delay = 0;
                JudgeAndGetStateMachine = SMS_Type;     //�ٴλ�ȡSMS��Ϣ
            }
        }
        break;
      case Get_TCP_Success:     //����TCP��Ϣ�ɹ�
        {
            OSSemPost(SempTCPRx);     //�����ź��� TCP 
            delay = Msg_Finish;
            JudgeAndGetStateMachine = Null_Type;
        }
        break;
      case TCP_Error:           //����TCP��Ϣ����
        {
            delay = Msg_Finish;
            JudgeAndGetStateMachine = Null_Type;
        }
        break;
      case Get_TCP_Fail:        //����TCP��Ϣʧ�� ����3��
        {
            if(Get_TCP_flag > 3)
            {
                Get_TCP_flag = 0;
                delay = Msg_Fail;
                JudgeAndGetStateMachine = Null_Type;
            }
            else
            {
                delay = 0;
                JudgeAndGetStateMachine = TCP_Type;     //�ٴλ�ȡTCP��Ϣ
            }
        }
        break;
        
        
      default:
        break;
    }
    
    
    
    
    
    
    
    return delay;
}