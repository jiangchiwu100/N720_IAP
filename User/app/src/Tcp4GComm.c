#include "includes.h"

OS_STK AppTaskTCPCommStk[APP_TASK_TCPComm_STK_SIZE];	

void AppTaskTCPComm(void *p_arg)
{
    static uint32_t TCPCommDelay = 1000;
    (void)p_arg;	 /* ����������澯 */
    
    
    while (1) 
    {
        if(R_State(&SysState,TCPRegister) == true)	//4G TCP�Ѿ���ʼ���ɹ�
        {
            TCPCommDelay = TCP_Comm();//TCP_Comm
            if(TCPCommDelay == Task_Suspend)      //ͨѶʧ�� 3���Ӻ�����ͨѶ
            {
                OSTimeDly(3*60*1000);
            }
            else
            {
                OSTimeDly(TCPCommDelay);
            }
        }
        else
        {
            OSTimeDly(1000);
        }
    }
}



uint32_t TCP_Comm(void)
{
    static uint32_t Heartbeat_cnt = 0;
    uint32_t delay = 0;
    static Enum_TCP_Comm TCPCommStateMachine = Enum_TCP_Login_Terminal;
    static uint8_t RxTx_Loop = 0;
    INT8U   err; 
    
    static Str_GW3761_Comm tx;
    Str_GW3761 msg;
    static uint8_t TCP_RX_Loop = 0;
    switch(TCPCommStateMachine)
    {
      case Enum_TCP_Login_Terminal:    //��¼�ն�
        {
            delay = TCP_Login_Terminal();
            if(delay == Login_Success)  //��¼�ɹ� �ȴ���������
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            
            else if(delay == Login_Fail)     //��¼ʧ�� �������� ���³�ʼ��
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            
            else        //�м���� ��¼��δ���
            {
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
        }
        break;
      case Enum_TCP_Send_Heartbeat:    //����������
        {
            delay = TCP_HeartBeat();
            if(delay == HeartBeat_Success)      //���������ɹ� �ȴ���������
            {
                RxTx_Loop++;
                printf("\n\nHeartbeat_cnt:%d\n\n", Heartbeat_cnt++);
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            
            else if(delay == HeartBeat_Fail)    //��������ʧ�� ���³�ʼ��TCP
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            
            else                                //�м���� ������δ���
            {
                TCPCommStateMachine = Enum_TCP_Send_Heartbeat;
            }
        }
        break;
      case Enum_TCP_Comm_RX:        //TCP��������
        {
            OSSemPend(SempTCPRx, 6000, &err);
            if( err != OS_ERR_TIMEOUT ) //���յ����ݺ���д���
            {
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                RxTx_Loop = 0;  //���յ����� ��շ��ͱ�־
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg); //������վ����֡
                GW3761_Frame_Comb(&tx,&msg);                        //��֡
                Insert_N720_Message_Q(tx.buf, tx.len);              //���뷢�Ͷ���
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX; //���з���
            }
            else                        //���ճ�ʱ  ��վ�������·�  ��ʼ���Ϸ�������
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX;
                if(RxTx_Loop++ >= 10)//���շ���ѭ������ʮ�� �����ݽ��� ��ʼ����������
                {
                    RxTx_Loop = 0;
                    delay = 0;
                    TCPCommStateMachine = Enum_TCP_Send_Heartbeat;
                }
            }                
        }
        break;
      case Enum_TCP_Comm_TX:        //TCP��������
        {
            delay = TCP_Comm_TX();
            if(delay == CommTX_Empty)       //���Ͷ��п� ��ʼ��������
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            else if(delay == CommTX_Fail)       //����ʧ�� ���³�ʼ��ģ�� ��������
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            else if(delay == CommTX_Finish)        //��ǰ���ݷ������ ��������
            {
                RxTx_Loop = 0;  //������� ��շ��ͱ�־
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX;
            }
            else   													 //�м���� ���ڷ�������
            {
                TCPCommStateMachine = Enum_TCP_Comm_TX;
            }
        }
        break;
        
      default:
        break;
    }
    return delay;
}

uint32_t TCP_Login_Terminal(void)
{
    uint32_t delay = 0;
    INT8U   err;
    static uint8_t Login_Loop = 0;
    static Enum_TCP_Login TCPLoginStateMachine = Enum_TCP_Send_Login;
    
    Str_GW3761_Comm tx;
    Str_GW3761 msg;
    Str_Calendar rtc;
    
    switch(TCPLoginStateMachine)
    {
      case Enum_TCP_Send_Login:     //���͵�¼֡
        {
            RS8025T_Get_Calendar_Time(&rtc);
            
            msg.Rtc = rtc;
            msg.FrameTypes = TerminalLogin;            
            msg.FrameNum = GW3761DatFrameSeqNum;            
            
            GW3761_Frame_Comb(&tx,&msg);
            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//���ݷ��ͳɹ� ���Ͻ�����������
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                delay = 0;
                TCPLoginStateMachine = Enum_Login_Rec_Reply;
            }
            else                                    //���ݷ���ʧ�ܺ� �ط�����
            {                
                delay = 1000;
                TCPLoginStateMachine = Enum_TCP_Send_Login;
            }
            if(Login_Loop++ >= 3)//��¼���β��ɹ� ��¼ʧ��
            {
                Login_Loop = 0;
                delay = Login_Fail;
                TCPLoginStateMachine = Enum_TCP_Send_Login;
            }
        }
        break;
      case Enum_Login_Rec_Reply:    //���ջ�Ӧ����֡
        {
            OSSemPend(SempTCPRx, 5000, &err);
            if( err != OS_ERR_TIMEOUT )//�ȴ���վ����֡ 
            {  
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                /****************���յ����ݺ���д���********************/
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                //                Sx1278_Lora_Tx_Data(&tx.buf[0],tx.len,Sx1278_Calculated_Cad_Preamble(750));//ǰ���� 750ms
                if(msg.FrameTypes == Confirm)//�յ�ȷ��֡ ��¼�ն˳ɹ�
                {
                    Login_Loop = 0;
                    delay = Login_Success;
                    TCPLoginStateMachine = Enum_TCP_Send_Login;
                }
                else                        //ȷ��֡���� �ٴη��͵�¼֡
                {
                    delay = 1000;
                    TCPLoginStateMachine = Enum_TCP_Send_Login;
                }
            }
            else                        //�ȴ���ʱ ��վ����Ӧ ���·��͵�¼֡                
            {
                delay = 1000;
                TCPLoginStateMachine = Enum_TCP_Send_Login;
            }
        }
        break;
      default:
        break;
    }
    return delay;
}


uint32_t TCP_HeartBeat(void)
{
    uint32_t delay = 0;
    INT8U   err;
    static uint8_t HeartBeat_Loop = 0;
    static Enum_TCP_HeartBeat TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
    
    Str_GW3761_Comm tx;
    Str_GW3761 msg;
    Str_Calendar rtc;
    
    switch(TCPHeartBeatStateMachine)
    {
      case Enum_TCP_Send_HeartBeat:     //��������֡
        {
            RS8025T_Get_Calendar_Time(&rtc);
            
            msg.Rtc = rtc;
            msg.FrameTypes = GateWayHeartBeat;            
            msg.FrameNum = GW3761DatFrameSeqNum;            
            
            GW3761_Frame_Comb(&tx,&msg);
            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//���ݷ��ͳɹ� ���Ͻ�����������
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                delay = 0;
                TCPHeartBeatStateMachine = Enum_HeartBeat_Rec_Reply;
            }
            else                                    //���ݷ���ʧ�ܺ� �ط�����
            {                
                delay = 1000;
                TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
            }
            if(HeartBeat_Loop++ >= 3)//�������β��ɹ� ����վʧȥ����
            {
                HeartBeat_Loop = 0;
                delay = HeartBeat_Fail;
                TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
            }
        }
        break;
      case Enum_HeartBeat_Rec_Reply:    //���ջ�Ӧ����֡
        {
            OSSemPend(SempTCPRx, 5000, &err);
            if( err != OS_ERR_TIMEOUT )//�ȴ���վ����֡ 
            {  
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                /****************���յ����ݺ���д���********************/
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                //                Sx1278_Lora_Tx_Data(&tx.buf[0],tx.len,Sx1278_Calculated_Cad_Preamble(750));//ǰ���� 750ms
                if(msg.FrameTypes == Confirm)//�յ�ȷ��֡ ���������ɹ�
                {
                    HeartBeat_Loop = 0;
                    delay = HeartBeat_Success;
                    TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
                }
                else                        //ȷ��֡���� �ٴη�������֡
                {
                    delay = 1000;
                    TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
                }
            }
            else                        //�ȴ���ʱ ��վ����Ӧ ���·�������֡                
            {
                delay = 1000;
                TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
            }
        }
        break;
      default:
        break;
    }
    return delay;
}

uint32_t TCP_Comm_TX(void)
{
    uint32_t delay = 0;
    static Enum_TCP_TX TCPTxStateMachine = Enum_Read_Send_Queue;
    static uint8_t TX_Loop = 0;
    static Str_TCP tx;
    INT8U   err;
    Str_GW3761 msg;
    switch(TCPTxStateMachine)
    {
      case Enum_Read_Send_Queue:    //��ȡ���Ͷ���
        {
            tx.len = Get_N720_Message_Q_Dat(tx.buf); 
            if(tx.len != false)//�жϷ��Ͷ����������Ƿ�Ϊ�� ��Ϊ�������Ͻ��з���
            {
                delay = 0;
                TCPTxStateMachine = Enum_TCP_Send_Data;
            }
            else               //Ϊ�� �˳�״̬��
            {
                delay = CommTX_Empty;
                TX_Loop = 0;
                TCPTxStateMachine = Enum_Read_Send_Queue;
            }            
        }
        break;
      case Enum_TCP_Send_Data:      //��������
        {            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//���ݷ��ͳɹ� ���Ͻ�����������
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                TX_Loop = 0;
                delay = 0;
                TCPTxStateMachine = Enum_Wait_Rec_Reply;
            }
            else                                     //����ʧ�� �ٴη���
            {
                TX_Loop++;
                delay = 1000;
                TCPTxStateMachine = Enum_TCP_Send_Data;
            }
            
            if(TX_Loop > 3)//���ͳ��Է��ͳ���3�� ʧ�� ģ��������� ���³�ʼ��
            {
                TX_Loop = 0;
                delay = CommTX_Fail;
                TCPTxStateMachine = Enum_Read_Send_Queue;
            }
        }
        break;
      case Enum_Wait_Rec_Reply:     //�ȴ���վ�ظ�ȷ��֡
        {
            OSSemPend(SempTCPRx, 5000, &err);    
            if( err != OS_ERR_TIMEOUT )//���յ����ݺ���д���
            { 
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                
                if(msg.FrameTypes == Confirm)   //�յ�ȷ��֡ ���ݷ��ͳɹ�
                {
                    //                    printf("\ntx.len:%d\n", tx.len);
                    TX_Loop = 0;
                    delay = CommTX_Finish;
                    TCPTxStateMachine = Enum_Read_Send_Queue;
                }
                else                            //ʧ�� �ٴη�������
                {
                    TX_Loop++;
                    delay = 1000;
                    TCPTxStateMachine = Enum_TCP_Send_Data;
                }
            }
            else                      //���ճ�ʱ û���յ�����
            {
                TX_Loop++;
                delay = 1000;
                TCPTxStateMachine = Enum_TCP_Send_Data;
            }
        }
        break;
      default:
        break;
    }   
    return delay;
}

//uint8_t GW3761_Frame(uint8_t *spoint,uint8_t len,Str_GW3761_Comm *tx)
//{
//    static uint8_t buf[GW3761_Frame_Max_Num]={0};
//    Str_GW3761 msg;
//      //�������ߵ�����֡ 
//  if(len != 0 || len < GW3761_Frame_Max_Num)
//  {
//    memcpy(&buf[0],&*spoint,len);
//    if(msg.Validity == true)//������Ч
//    {
//        if(msg.ID[0] == ConfigInfo.ID[0] && msg.ID[1] == ConfigInfo.ID[1])//ID��ȷ
//        {
//            switch(msg.FrameTypes)
//            {
//                
//            }
//        }
//    }
//    
//    
//  }
//
//
//}



/*
�ն˵�¼
//    Str_GW3761 msg;
//    static Str_GW3761_Comm tx;
//    Str_Calendar rtc;
//    
//    RS8025T_Get_Calendar_Time(&rtc);
//    msg.FrameTypes = TerminalLogin;
//    msg.Rtc = rtc;
//    msg.FrameNum = GW3761DatFrameSeqNum;
//    
//    GateWayID[0]=0;
//    GateWayID[1]=5;
//    GW3761_Frame_Comb(&tx,&msg);
0D 0A 24 4D 59 4E 45 54 52 45 41 44 3A 20 30 2C 32 30 0D 0A 
*/

/*
�������� 
//    Str_GW3761 msg;
//    static Str_GW3761_Comm tx;
//    Str_Calendar rtc;
//    
//    RS8025T_Get_Calendar_Time(&rtc);
//    msg.FrameTypes = GateWayHeartBeat;
//    msg.Rtc = rtc;
//    msg.FrameNum = GW3761DatFrameSeqNum;
//    
//    GateWayID[0]=0;
//    GateWayID[1]=5;
//    GW3761_Frame_Comb(&tx,&msg);

*/