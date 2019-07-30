#include "includes.h"



 





void Resolve_LoRa_Protocol_Frame(uint8_t *dat,uint8_t len,Union_Lora_Protocol_MSG_Q *msg)
{
  uint8_t buf[64];
 
  if(len==0 || len>=64)
    msg->Str_Lora_Protocol_MSG_Q.Validity = false;//数据帧错误
  else
    memcpy(&buf[0],&*dat,len);          //接收无线的数据帧 
  
  
  msg->Str_Lora_Protocol_MSG_Q.Validity    = false;
  
  if((buf[0] == 0x68) && (Count_CheckSum(&buf[0],(len-1)) == buf[buf[1]+8]))//帧头和校验和是否正确
  {
    msg->Str_Lora_Protocol_MSG_Q.ControlCode.Byte = buf[2]; //提取控制码
    msg->Str_Lora_Protocol_MSG_Q.ChannleNum       = buf[3]; //信道编号 
    memcpy(&(msg->Str_Lora_Protocol_MSG_Q.NodeID[0]),&buf[4],4);//把地址域取出来 
    switch(msg->Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode)
    {
    case NetProtocol_Beacon:          //节点不用应答 beacon  
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = false;//节点不会上报beacon数据帧 所以无效
      }
      break;
    case NetProtocol_QueryGateway:    //网关查询应答
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;//数据帧有效
        
      }
      break;
    case NetProtocol_GateWayRegister: //集中器入网请求应答
      {
        if(buf[1] == 0x01)
        {
          msg->Str_Lora_Protocol_MSG_Q.Validity = true;//数据帧有效
          msg->Str_Lora_Protocol_MSG_Q.DeviceType = buf[8];//设备类型
        }
        else
        {
          msg->Str_Lora_Protocol_MSG_Q.Validity = false;//数据帧有效
        }
      }
      break;
    case NetProtocol_QuerySlot:       //集中器Comm信道应答失步查询
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;//数据帧有效
      }
      break;   
    case NetProtocol_BroadcastTime:   //集中器ping信道广播下发时隙
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = false;//节点不会上报广播数据帧 所以无效
      }
      break;
    case NetProtocol_PingType:        //节点上报网关 ping类型 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
      }
      break;
    case NetProtocol_PingData:        //集中器下发ping数据 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
        msg->Str_Lora_Protocol_MSG_Q.BufLen = buf[1];
        memcpy(&(msg->Str_Lora_Protocol_MSG_Q.Buf[0]),&buf[8],buf[1]);//把数据域全部读出来 等待下一步解析
      }
      break;
    case NetProtocol_PollingType:     //集中器应答 polling 类型
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
      }
      break;
    case NetProtocol_PollingData:     //集中器应答 polling 数据 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
        msg->Str_Lora_Protocol_MSG_Q.BufLen = buf[1];
        memcpy(&(msg->Str_Lora_Protocol_MSG_Q.Buf[0]),&buf[8],buf[1]);//把数据域全部读出来 等待下一步解析
      }
      break;
    default:
      break;
    }
  }
}


 




//组织lora协议 网关查询（上行） 入网请求（上行） 失步查询（上行） 数据抄读（上行应答） polling（上行）

void Combine_LoRa_Protocol_Frame(Str_Lora *tx ,Union_Lora_Protocol_MSG_Q *msg)
{
 
  B16_B08 Bata;
  
  switch(msg->Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode)
  {
  case NetProtocol_Beacon://集中器下发beacon  广播时隙滴答 和当前RTC 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x08;
      tx->buf[tx->len++] = 0x00;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      Bata.B16 = (msg->Str_Lora_Protocol_MSG_Q.SlotTick + BeaconSlotTickOffset) ;//滴答要增加16个 
      tx->buf[tx->len++] = Bata.B8[0];
      tx->buf[tx->len++] = Bata.B8[1];
 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Seconds;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Minutes;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Hours;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.DayOfMonth;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Month;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Year;
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_QueryGateway://集中器下发节点查询数据帧  下发节点的信号强度  
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x01;
      tx->buf[tx->len++] = 0x01;//上行 需要ACK
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.SignalStrength;//下发信号强度 
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_GateWayRegister:
    {
      tx->len = 0;
      if(msg->Str_Lora_Protocol_MSG_Q.BufLen == 0x0a)
      {
        tx->buf[tx->len++] = 0x68;
        tx->buf[tx->len++] = 0x0A;
        tx->buf[tx->len++] = 0x02;//控制域 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
        
        Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotTick + RegisterSlotTickOffset;//时隙滴答 
        tx->buf[tx->len++] = Bata.B8[0];
        tx->buf[tx->len++] = Bata.B8[1];  
        
        Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotNum;//时隙号 
        tx->buf[tx->len++] = Bata.B8[0];
        tx->buf[tx->len++] = Bata.B8[1]; 
        
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Seconds;
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Minutes;
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Hours;
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.DayOfMonth;
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Month;
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Year;
        tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
        tx->len++;
      }
      else//不同意入网
      {
        tx->buf[tx->len++] = 0x68;
        tx->buf[tx->len++] = 0x00;
        tx->buf[tx->len++] = 0x02;//控制域 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
        
        tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
        tx->len++;
      }  
    }
    break;
  case NetProtocol_QuerySlot:
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x0A;
      tx->buf[tx->len++] = 0x03;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      
      Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotTick;//时隙滴答 
      tx->buf[tx->len++] = Bata.B8[0];
      tx->buf[tx->len++] = Bata.B8[1]; 
 
      Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotNum;//时隙号 
      tx->buf[tx->len++] = Bata.B8[0];
      tx->buf[tx->len++] = Bata.B8[1]; 

      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Seconds;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Minutes;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Hours;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.DayOfMonth;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Month;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.Rtc.Year;
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_BroadcastTime:
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x02;
      tx->buf[tx->len++] = 0x04;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = 0xAA;
      tx->buf[tx->len++] = 0xAA;
      tx->buf[tx->len++] = 0xAA;
      tx->buf[tx->len++] = 0xAA;
      Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotTick + BroadcastSlotTickOffset;
      tx->buf[tx->len++] = Bata.B8[0];
      tx->buf[tx->len++] = Bata.B8[1];
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;      
  case NetProtocol_PingType://集中器下发ping类型唤醒节点 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x65;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_PingData://数据抄读 上报应答
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.BufLen;
      tx->buf[tx->len++] = 0x46;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      memcpy(&tx->buf[tx->len],&(msg->Str_Lora_Protocol_MSG_Q.Buf[0]),msg->Str_Lora_Protocol_MSG_Q.BufLen);
      tx->len = tx->len + msg->Str_Lora_Protocol_MSG_Q.BufLen;
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_PollingType://集中器应答节点的polling类型 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x07;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];

      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_PollingData://节点组织上报集中器的polling数据 645
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x08;//控制域 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //集中器信道编号 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];

      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  default:
    break;
  }
  
#ifdef  PrintfEnable
      printf("**发送数据**");
  
  for(uint8_t num=0;num<tx->len;num++)
  {
    printf ("%x,",tx->buf[num]);
  }
  printf("\n\r");
#endif
}



