#include "includes.h"

OS_STK AppTaskTCPCommStk[APP_TASK_TCPComm_STK_SIZE];	

void AppTaskTCPComm(void *p_arg)
{
    static uint32_t TCPCommDelay = 1000;
    (void)p_arg;	 /* 避免编译器告警 */
    
    
    while (1) 
    {
        if(R_State(&SysState,TCPRegister) == true)	//4G TCP已经初始化成功
        {
            TCPCommDelay = TCP_Comm();//TCP_Comm
            if(TCPCommDelay == Task_Suspend)      //通讯失败 3分钟后重新通讯
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
      case Enum_TCP_Login_Terminal:    //登录终端
        {
            delay = TCP_Login_Terminal();
            if(delay == Login_Success)  //登录成功 等待接收数据
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            
            else if(delay == Login_Fail)     //登录失败 挂起任务 重新初始化
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            
            else        //中间过程 登录还未完成
            {
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
        }
        break;
      case Enum_TCP_Send_Heartbeat:    //发送心跳包
        {
            delay = TCP_HeartBeat();
            if(delay == HeartBeat_Success)      //发送心跳成功 等待接收数据
            {
                RxTx_Loop++;
                printf("\n\nHeartbeat_cnt:%d\n\n", Heartbeat_cnt++);
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            
            else if(delay == HeartBeat_Fail)    //发送心跳失败 重新初始化TCP
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            
            else                                //中间过程 心跳还未完成
            {
                TCPCommStateMachine = Enum_TCP_Send_Heartbeat;
            }
        }
        break;
      case Enum_TCP_Comm_RX:        //TCP接收数据
        {
            OSSemPend(SempTCPRx, 6000, &err);
            if( err != OS_ERR_TIMEOUT ) //接收到数据后进行处理
            {
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                RxTx_Loop = 0;  //接收到数据 清空发送标志
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg); //解析主站数据帧
                GW3761_Frame_Comb(&tx,&msg);                        //组帧
                Insert_N720_Message_Q(tx.buf, tx.len);              //填入发送对列
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX; //进行发送
            }
            else                        //接收超时  主站无数据下发  开始向上发送数据
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX;
                if(RxTx_Loop++ >= 10)//接收发送循环超过十次 无数据交互 开始发送心跳包
                {
                    RxTx_Loop = 0;
                    delay = 0;
                    TCPCommStateMachine = Enum_TCP_Send_Heartbeat;
                }
            }                
        }
        break;
      case Enum_TCP_Comm_TX:        //TCP发送数据
        {
            delay = TCP_Comm_TX();
            if(delay == CommTX_Empty)       //发送队列空 开始接收数据
            {
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_RX;
            }
            else if(delay == CommTX_Fail)       //发送失败 重新初始化模块 挂起任务
            {
                delay = Task_Suspend;
                TCPCommStateMachine = Enum_TCP_Login_Terminal;
            }
            else if(delay == CommTX_Finish)        //当前数据发送完成 继续发送
            {
                RxTx_Loop = 0;  //发送完成 清空发送标志
                delay = 0;
                TCPCommStateMachine = Enum_TCP_Comm_TX;
            }
            else   													 //中间过程 正在发送数据
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
      case Enum_TCP_Send_Login:     //发送登录帧
        {
            RS8025T_Get_Calendar_Time(&rtc);
            
            msg.Rtc = rtc;
            msg.FrameTypes = TerminalLogin;            
            msg.FrameNum = GW3761DatFrameSeqNum;            
            
            GW3761_Frame_Comb(&tx,&msg);
            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//数据发送成功 马上解析接收数据
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                delay = 0;
                TCPLoginStateMachine = Enum_Login_Rec_Reply;
            }
            else                                    //数据发送失败后 重发数据
            {                
                delay = 1000;
                TCPLoginStateMachine = Enum_TCP_Send_Login;
            }
            if(Login_Loop++ >= 3)//登录三次不成功 登录失败
            {
                Login_Loop = 0;
                delay = Login_Fail;
                TCPLoginStateMachine = Enum_TCP_Send_Login;
            }
        }
        break;
      case Enum_Login_Rec_Reply:    //接收回应数据帧
        {
            OSSemPend(SempTCPRx, 5000, &err);
            if( err != OS_ERR_TIMEOUT )//等待主站数据帧 
            {  
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                /****************接收到数据后进行处理********************/
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                //                Sx1278_Lora_Tx_Data(&tx.buf[0],tx.len,Sx1278_Calculated_Cad_Preamble(750));//前导码 750ms
                if(msg.FrameTypes == Confirm)//收到确认帧 登录终端成功
                {
                    Login_Loop = 0;
                    delay = Login_Success;
                    TCPLoginStateMachine = Enum_TCP_Send_Login;
                }
                else                        //确认帧有误 再次发送登录帧
                {
                    delay = 1000;
                    TCPLoginStateMachine = Enum_TCP_Send_Login;
                }
            }
            else                        //等待超时 主站无响应 重新发送登录帧                
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
      case Enum_TCP_Send_HeartBeat:     //发送心跳帧
        {
            RS8025T_Get_Calendar_Time(&rtc);
            
            msg.Rtc = rtc;
            msg.FrameTypes = GateWayHeartBeat;            
            msg.FrameNum = GW3761DatFrameSeqNum;            
            
            GW3761_Frame_Comb(&tx,&msg);
            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//数据发送成功 马上解析接收数据
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                delay = 0;
                TCPHeartBeatStateMachine = Enum_HeartBeat_Rec_Reply;
            }
            else                                    //数据发送失败后 重发数据
            {                
                delay = 1000;
                TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
            }
            if(HeartBeat_Loop++ >= 3)//心跳三次不成功 和主站失去连接
            {
                HeartBeat_Loop = 0;
                delay = HeartBeat_Fail;
                TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
            }
        }
        break;
      case Enum_HeartBeat_Rec_Reply:    //接收回应数据帧
        {
            OSSemPend(SempTCPRx, 5000, &err);
            if( err != OS_ERR_TIMEOUT )//等待主站数据帧 
            {  
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                /****************接收到数据后进行处理********************/
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                //                Sx1278_Lora_Tx_Data(&tx.buf[0],tx.len,Sx1278_Calculated_Cad_Preamble(750));//前导码 750ms
                if(msg.FrameTypes == Confirm)//收到确认帧 发送心跳成功
                {
                    HeartBeat_Loop = 0;
                    delay = HeartBeat_Success;
                    TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
                }
                else                        //确认帧有误 再次发送心跳帧
                {
                    delay = 1000;
                    TCPHeartBeatStateMachine = Enum_TCP_Send_HeartBeat;
                }
            }
            else                        //等待超时 主站无响应 重新发送心跳帧                
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
      case Enum_Read_Send_Queue:    //读取发送队列
        {
            tx.len = Get_N720_Message_Q_Dat(tx.buf); 
            if(tx.len != false)//判断发送队列中数据是否为空 不为空则马上进行发送
            {
                delay = 0;
                TCPTxStateMachine = Enum_TCP_Send_Data;
            }
            else               //为空 退出状态机
            {
                delay = CommTX_Empty;
                TX_Loop = 0;
                TCPTxStateMachine = Enum_Read_Send_Queue;
            }            
        }
        break;
      case Enum_TCP_Send_Data:      //发送数据
        {            
            if(N720_TCP_Send(tx.buf, tx.len) == true)//数据发送成功 马上解析接收数据
            {
                printf("\n\nT:");
                for(uint8_t i = 0; i <= tx.len; i++)
                    printf("%x ", tx.buf[i]);
                printf("\n");
                
                TX_Loop = 0;
                delay = 0;
                TCPTxStateMachine = Enum_Wait_Rec_Reply;
            }
            else                                     //发送失败 再次发送
            {
                TX_Loop++;
                delay = 1000;
                TCPTxStateMachine = Enum_TCP_Send_Data;
            }
            
            if(TX_Loop > 3)//发送尝试发送超过3次 失败 模块出现问题 重新初始化
            {
                TX_Loop = 0;
                delay = CommTX_Fail;
                TCPTxStateMachine = Enum_Read_Send_Queue;
            }
        }
        break;
      case Enum_Wait_Rec_Reply:     //等待主站回复确认帧
        {
            OSSemPend(SempTCPRx, 5000, &err);    
            if( err != OS_ERR_TIMEOUT )//接收到数据后进行处理
            { 
                printf("\n\nR:");
                for(uint8_t i = 0; i <= TCPbuf.len; i++)
                    printf("%x ", TCPbuf.buf[i]);
                printf("\n");
                
                GW3761_Frame_Resolve(TCPbuf.buf, TCPbuf.len, &msg);
                
                if(msg.FrameTypes == Confirm)   //收到确认帧 数据发送成功
                {
                    //                    printf("\ntx.len:%d\n", tx.len);
                    TX_Loop = 0;
                    delay = CommTX_Finish;
                    TCPTxStateMachine = Enum_Read_Send_Queue;
                }
                else                            //失败 再次发送数据
                {
                    TX_Loop++;
                    delay = 1000;
                    TCPTxStateMachine = Enum_TCP_Send_Data;
                }
            }
            else                      //接收超时 没有收到数据
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
//      //接收无线的数据帧 
//  if(len != 0 || len < GW3761_Frame_Max_Num)
//  {
//    memcpy(&buf[0],&*spoint,len);
//    if(msg.Validity == true)//数据有效
//    {
//        if(msg.ID[0] == ConfigInfo.ID[0] && msg.ID[1] == ConfigInfo.ID[1])//ID正确
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
终端登录
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
心跳上行 
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