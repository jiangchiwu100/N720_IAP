#include "includes.h"



 





void Resolve_LoRa_Protocol_Frame(uint8_t *dat,uint8_t len,Union_Lora_Protocol_MSG_Q *msg)
{
  uint8_t buf[64];
 
  if(len==0 || len>=64)
    msg->Str_Lora_Protocol_MSG_Q.Validity = false;//����֡����
  else
    memcpy(&buf[0],&*dat,len);          //�������ߵ�����֡ 
  
  
  msg->Str_Lora_Protocol_MSG_Q.Validity    = false;
  
  if((buf[0] == 0x68) && (Count_CheckSum(&buf[0],(len-1)) == buf[buf[1]+8]))//֡ͷ��У����Ƿ���ȷ
  {
    msg->Str_Lora_Protocol_MSG_Q.ControlCode.Byte = buf[2]; //��ȡ������
    msg->Str_Lora_Protocol_MSG_Q.ChannleNum       = buf[3]; //�ŵ���� 
    memcpy(&(msg->Str_Lora_Protocol_MSG_Q.NodeID[0]),&buf[4],4);//�ѵ�ַ��ȡ���� 
    switch(msg->Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode)
    {
    case NetProtocol_Beacon:          //�ڵ㲻��Ӧ�� beacon  
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = false;//�ڵ㲻���ϱ�beacon����֡ ������Ч
      }
      break;
    case NetProtocol_QueryGateway:    //���ز�ѯӦ��
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;//����֡��Ч
        
      }
      break;
    case NetProtocol_GateWayRegister: //��������������Ӧ��
      {
        if(buf[1] == 0x01)
        {
          msg->Str_Lora_Protocol_MSG_Q.Validity = true;//����֡��Ч
          msg->Str_Lora_Protocol_MSG_Q.DeviceType = buf[8];//�豸����
        }
        else
        {
          msg->Str_Lora_Protocol_MSG_Q.Validity = false;//����֡��Ч
        }
      }
      break;
    case NetProtocol_QuerySlot:       //������Comm�ŵ�Ӧ��ʧ����ѯ
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;//����֡��Ч
      }
      break;   
    case NetProtocol_BroadcastTime:   //������ping�ŵ��㲥�·�ʱ϶
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = false;//�ڵ㲻���ϱ��㲥����֡ ������Ч
      }
      break;
    case NetProtocol_PingType:        //�ڵ��ϱ����� ping���� 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
      }
      break;
    case NetProtocol_PingData:        //�������·�ping���� 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
        msg->Str_Lora_Protocol_MSG_Q.BufLen = buf[1];
        memcpy(&(msg->Str_Lora_Protocol_MSG_Q.Buf[0]),&buf[8],buf[1]);//��������ȫ�������� �ȴ���һ������
      }
      break;
    case NetProtocol_PollingType:     //������Ӧ�� polling ����
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
      }
      break;
    case NetProtocol_PollingData:     //������Ӧ�� polling ���� 
      {
        msg->Str_Lora_Protocol_MSG_Q.Validity = true;
        msg->Str_Lora_Protocol_MSG_Q.BufLen = buf[1];
        memcpy(&(msg->Str_Lora_Protocol_MSG_Q.Buf[0]),&buf[8],buf[1]);//��������ȫ�������� �ȴ���һ������
      }
      break;
    default:
      break;
    }
  }
}


 




//��֯loraЭ�� ���ز�ѯ�����У� �����������У� ʧ����ѯ�����У� ���ݳ���������Ӧ�� polling�����У�

void Combine_LoRa_Protocol_Frame(Str_Lora *tx ,Union_Lora_Protocol_MSG_Q *msg)
{
 
  B16_B08 Bata;
  
  switch(msg->Str_Lora_Protocol_MSG_Q.ControlCode.Str_LoRa_Ctrl.FunctionCode)
  {
  case NetProtocol_Beacon://�������·�beacon  �㲥ʱ϶�δ� �͵�ǰRTC 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x08;
      tx->buf[tx->len++] = 0x00;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      tx->buf[tx->len++] = 0xaa;
      Bata.B16 = (msg->Str_Lora_Protocol_MSG_Q.SlotTick + BeaconSlotTickOffset) ;//�δ�Ҫ����16�� 
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
  case NetProtocol_QueryGateway://�������·��ڵ��ѯ����֡  �·��ڵ���ź�ǿ��  
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x01;
      tx->buf[tx->len++] = 0x01;//���� ��ҪACK
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.SignalStrength;//�·��ź�ǿ�� 
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
        tx->buf[tx->len++] = 0x02;//������ 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
        
        Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotTick + RegisterSlotTickOffset;//ʱ϶�δ� 
        tx->buf[tx->len++] = Bata.B8[0];
        tx->buf[tx->len++] = Bata.B8[1];  
        
        Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotNum;//ʱ϶�� 
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
      else//��ͬ������
      {
        tx->buf[tx->len++] = 0x68;
        tx->buf[tx->len++] = 0x00;
        tx->buf[tx->len++] = 0x02;//������ 
        tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
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
      tx->buf[tx->len++] = 0x03;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      
      Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotTick;//ʱ϶�δ� 
      tx->buf[tx->len++] = Bata.B8[0];
      tx->buf[tx->len++] = Bata.B8[1]; 
 
      Bata.B16 = msg->Str_Lora_Protocol_MSG_Q.SlotNum;//ʱ϶�� 
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
      tx->buf[tx->len++] = 0x04;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
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
  case NetProtocol_PingType://�������·�ping���ͻ��ѽڵ� 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x65;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];
      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_PingData://���ݳ��� �ϱ�Ӧ��
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.BufLen;
      tx->buf[tx->len++] = 0x46;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
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
  case NetProtocol_PollingType://������Ӧ��ڵ��polling���� 
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x07;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[0];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[1];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[2];
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.NodeID[3];

      tx->buf[tx->len  ] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->len++;
    }
    break;
  case NetProtocol_PollingData://�ڵ���֯�ϱ���������polling���� 645
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x08;//������ 
      tx->buf[tx->len++] = msg->Str_Lora_Protocol_MSG_Q.ChannleNum; //�������ŵ���� 
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
      printf("**��������**");
  
  for(uint8_t num=0;num<tx->len;num++)
  {
    printf ("%x,",tx->buf[num]);
  }
  printf("\n\r");
#endif
}



