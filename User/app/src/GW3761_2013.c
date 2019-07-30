

#include "includes.h"



uint8_t GW3761_Bit_To_Bin(uint8_t dt2)
{
  uint8_t tempdt2 = 0;
  
  switch (dt2)
  {
  case 0x01:
    tempdt2 = 0x01;
    break;
  case 0x02:
    tempdt2 = 0x02;
    break;
  case 0x04:
    tempdt2 = 0x03;
    break;
  case 0x08:
    tempdt2 = 0x04;
    break;
  case 0x10:
    tempdt2 = 0x05;
    break;
  case 0x20:
    tempdt2 = 0x06;
    break;
  case 0x40:
    tempdt2 = 0x07;
    break;
  case 0x80:
    tempdt2 = 0x08;
    break;
  default:
    tempdt2 = 0x00;
    break;
  }
  return tempdt2;
}

/*******************************************************************************
** Function name:       //GW3762CalculateFn
** Descriptions:        //����FN
** input parameters:    /f ����ָ��
** Returned value:      //���ؼ����FN
** Used global variables:       //
** Calling modules:     //
** Created by:          //ZJ
** Created Date:        //2012/05/04
**------------------------------------------------------------------------------
** Modified by: //
** Modified date:       //
**------------------------------------------------------------------------------
******************************************************************************/
uint8_t GW3761_Cal_Fn(uint8_t *buf)
{
  uint8_t dt1 = 0, dt2 = 0;
  uint8_t dt = 0;
  
  dt2 = buf[0];
  dt1 = buf[1];
  dt = dt1 * 8 + GW3761_Bit_To_Bin(dt2);
  
  return dt;
}

void GW3761_Frame_Comb(Str_GW3761_Comm *tx,Str_GW3761 *msg)
{
  B16_B08 B16;
  
  switch(msg->FrameTypes)
  {
  case DataForwarding:	//����֡͸������
    {//68 3A 00 3A 00 68   88    00 0B 00 05 00   10 60 00 00 01 00    06  21 00  68 77 93 10 17 00 00 68 81 15 34 F3 43 39 35 34 4B 43 33 33 35 3C 93 3C 47 39 35 34 4B 33 77 B5 16 B0 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x4a;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x4a;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x88;
      
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x10 ;
      tx->buf[tx->len++] = 0x60 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = msg->PortNum  ;//�˿ں� 
      tx->buf[tx->len++] = msg->Len  ; //����֡����
      tx->buf[tx->len++] = 0x00 ; 
      
      memcpy(&tx->buf[tx->len],&(msg->Buf[0]),msg->Len);
      tx->len = tx->len + msg->Len;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++; 
    }
    break;
  case GetGateWayRTC:		//��ȡ������ʱ��
    {//68 4A 00 4A 00 68 88  00 13 00 05 00   0C 70 00 00 02 00    51 20 23 07 10 18   E1 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x4a;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x4a;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x88;
      
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x0c ;
      tx->buf[tx->len++] = 0x70 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x02 ;
      tx->buf[tx->len++] = 0x00 ;
      RS8025T_Get_Calendar_Time(&msg->Rtc);
      tx->buf[tx->len++] = msg->Rtc.Seconds;
      tx->buf[tx->len++] = msg->Rtc.Minutes;
      tx->buf[tx->len++] = msg->Rtc.Hours;
      tx->buf[tx->len++] = msg->Rtc.DayOfMonth;
      tx->buf[tx->len++] = msg->Rtc.Month;
      tx->buf[tx->len++] = msg->Rtc.Year;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;  
      
    }
    break;
  case LoraNetWorkLogin:
    {//68 62 00 62 00 68 88 00 00 00 00 06   0D 67 00 00 02 01    78 56 34 12 00 00   02   DD   00 00 00 00 F8 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x62;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x62;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x88;
      
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x0D ;
      tx->buf[tx->len++] = 0x67 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x02 ;
      tx->buf[tx->len++] = 0x01 ;
      
      memcpy(&tx->buf[tx->len],&(msg->DeviceID[0]),6);
      tx->len = tx->len + 6;
      tx->buf[tx->len++] = msg->SignalStrength ;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
    }
    break;
  case MainStationIP:   //��ȡ������ip
    { //68 A2 00 A2 00 68 88   00 00 00 0B 00   0A 70 00 00 04 00    B4 61 51 BE 25 4E  00 00 00 00 00 00 63 6D 6E 65 74 00 00 00 00 00 00 00 00 00 00 00 BF 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0xA2;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0xA2;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x88;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x0A ;
      tx->buf[tx->len++] = 0x70 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x04 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = msg->IP[0] ;
      tx->buf[tx->len++] = msg->IP[1] ;
      tx->buf[tx->len++] = msg->IP[2] ;
      tx->buf[tx->len++] = msg->IP[3] ;
      tx->buf[tx->len++] = msg->IP[4] ;
      tx->buf[tx->len++] = msg->IP[5] ;
      
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x63 ;
      tx->buf[tx->len++] = 0x6D ;
      tx->buf[tx->len++] = 0x6E ;
      tx->buf[tx->len++] = 0x65 ;
      tx->buf[tx->len++] = 0x74 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
    }
    break;
  case GetGateWayVerNum://��ȡ�������汾
    {       
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0xD6;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0xD6;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x88;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x09 ;
      tx->buf[tx->len++] = 0x70 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x58 ;//���̴���
      tx->buf[tx->len++] = 0x4B ;
      tx->buf[tx->len++] = 0x53 ;
      tx->buf[tx->len++] = 0x44 ;
      
      tx->buf[tx->len++] = 0x0 ;//�豸���
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ; 
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;
      
      tx->buf[tx->len++] = 0x0 ;//�ն�����汾��
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;//�ն�����������ڣ�������
      tx->buf[tx->len++] = 0x09 ;
      tx->buf[tx->len++] = 0x19 ;
      
      
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x0 ;//�ն�����������Ϣ��
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ; 
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      
      tx->buf[tx->len++] = 0x0 ;//�ն�ͨ��Э��.�汾��
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      
      tx->buf[tx->len++] = 0x0 ;//�ն�Ӳ���汾��
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;//�ն�Ӳ���������ڣ�������
      tx->buf[tx->len++] = 0x09 ;
      tx->buf[tx->len++] = 0x19 ;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
      
      
    }
    break;
  case Confirm:					//ȷ��
    {//68 36 00 36 00 68 80 00 05 00 05 00     00 70 00 00 01 00 01 FC 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x36;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x36;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x80;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x07 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
    }
    break;
  case Deny:   					//����
    { 
      //68 32 00 32 00 68 8b 00 00 01 00 00 00 60 00 00 02 00 ee 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x32;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x32;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x8b;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x06 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x02 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
    }
    break;
  case GateWayHeartBeat://����
    {
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x4A;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x4A;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0xC9;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x02 ;
      tx->buf[tx->len++] = 0x60 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x04 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = msg->Rtc.Seconds;
      tx->buf[tx->len++] = msg->Rtc.Minutes;
      tx->buf[tx->len++] = msg->Rtc.Hours;
      tx->buf[tx->len++] = msg->Rtc.DayOfMonth;
      tx->buf[tx->len++] = msg->Rtc.Month;
      tx->buf[tx->len++] = msg->Rtc.Year;
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
    }
    break;
  case TerminalLogin:		//�ն˵�¼
    {//68 32 00 32 00 68 C9 00 00 00 05 00 02 60 00 00 01 00 31 16
      tx->len = 0;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0x32;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x32;
      tx->buf[tx->len++] = 0x00;
      tx->buf[tx->len++] = 0x68;
      tx->buf[tx->len++] = 0xC9;
      B16.B16 = msg->FrameNum;
      tx->buf[tx->len++] = B16.B8[0] ;
      tx->buf[tx->len++] = B16.B8[1] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[0] ;
      tx->buf[tx->len++] = ConfigInfo.DevID[1] ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = 0x02 ;
      tx->buf[tx->len++] = 0x60 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x00 ;
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x00 ;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      
      tx->len++;
    }
    break;
  default:
    break;
  }
  
}

void GW3761_Frame_Resolve(uint8_t *spoint,uint8_t len,Str_GW3761 *dat)
{
  static uint8_t buf[GW3761_Frame_Max_Num]={0};
  
  static B16_B08 framelen;
  static B16_B08 B16 ;
  
  dat->Validity = false;
  dat->Len      = 0;
  //�������ߵ�����֡ 
  if(len != 0 || len < GW3761_Frame_Max_Num)
  {
    memcpy(&buf[0],&*spoint,len);  
    
    if(buf[FrameStart0offset] == FrameStart)                                      //֡ͷ0�Ƿ���ȷ
    {
      if(buf[FrameStart1offset] == FrameStart)                                    //֡ͷ1�Ƿ���ȷ
      {
        framelen.B8[0] = buf[FrameLen0offset] ; 
        framelen.B8[1] = buf[FrameLen1offset] ; 
        framelen.B16   = framelen.B16>>2;                                         //����֡����
        
        if(buf[framelen.B16 + FrameStart1offset + 2] == FrameEnd)                 //����֡β�Ƿ���ȷ
        {                                                                         //����֡У����Ƿ���ȷ
          if(buf[ framelen.B16 + FrameStart1offset + 1 ] == Count_CheckSum(&buf[FrameCtrloffset],framelen.B16))       
          {                                                                     //У����Ƿ���ȷ                                                        
            //�ж����Ǹ������� ������Ӧ����
            dat->Afn = buf[ AFNOffset ];//Ӧ�ò㹦����
            dat->Fn  = GW3761_Cal_Fn( &buf[ DTOffset ] );
            dat->Pn  = GW3761_Cal_Fn( &buf[ DAOffset ] );
            dat->ID[0]=buf[FrameAddrOffset];
            dat->ID[1]=buf[FrameAddrOffset + 1];
            
            if((dat->ID[0] == ConfigInfo.DevID[0]) && (dat->ID[1] == ConfigInfo.DevID[1]))
            {
              
              B16.B8[1]= buf[FrameSerialNumH];
              B16.B8[0]= buf[FrameSerialNumL];
              dat->FrameNum = B16.B16;
              switch(dat->Afn)
              {
              case 0x00://ȷ�� ����
                {
                  
                  switch(dat->Fn)
                  {
                  case 0x01: 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = Confirm;//ȫ��ȷ��
                    }
                    break;
                  case 0x02: 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = Deny;//ȫ������
                    }
                    break;
                  default:
                    break;
                  }
                }
                break;
              case 0x01://��λ
                {
                  
                }
                break;
              case 0x02://��·�ӿڼ��
                {
                  
                }
                break;
              case 0x03://�м�վ����
                {
                  
                }
                break;
              case 0x04://���ò���
                {
                  switch( dat->Fn )
                  {
                  case 3://����IP��ַ�Ͷ˿ں�
                    {
                      
                    }
                    break;
                  case 10://���ر��
                    {
                      
                    }
                    break;
                  default:
                    break;
                  }
                }
                break;
              case 0x05://��������
                {
                  switch(dat->Fn)
                  {
                  case 49://�·�ʱ��
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = StopMeterReading;//�����ֳ� 
                    }
                    break;
                  case 50://�����ֳ�  
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = StartMeterReading;//�����ֳ�
                    }
                    break;
                  default:
                    break; 
                  }
                }
                break;
              case 0x06://�����֤�Լ���ԿЭ��
                {
                  
                }
                break;
              case 0x07://����
                {
                  
                }
                break;
              case 0x08://���󱻼����ն������ϱ�
                {
                  
                }
                break;
              case 0x09://��ȡ�������汾��
                {
                  switch( dat->Fn )
                  {
                  case 1:// 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = GetGateWayVerNum;//��ȡ�������汾��
                    }
                    break;
                  }
                }
                break;
              case 0x0A://��ѯ����
                {
                  switch( dat->Fn )
                  {
                  case 3:
                    {
                      dat->Validity    = true;
                      dat->FrameTypes  = MainStationIP;//��ѯ��վIP �˿ں�
                    }
                    break;
                  case 10:////////////////////////////??????????????????????��ѯ�����Ϣ 
                    {
                      dat->Len  = 0;
                      memcpy(&(dat->Buf[0]),&buf[24],dat->Len);
                      
                      dat->Validity    = true;
                      dat->FrameTypes  = MainStationIP;// 
                    }
                    break;
                  }
                }
                break;
              case 0x0B://������������
                {
                  
                }
                break;
              case 0x0C://����1������---ʵʱ����
                {
                  switch( dat->Fn )
                  {
                  case 2://��ȡ������ʱ�� 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = GetGateWayRTC;//����ȡ������ʱ��
                    }
                    break;
                  }
                }
                break;
              case 0x0D://����2������---��ʷ����
                {
                  
                }
                break;
              case 0x0E://����2������---�¼�����
                {
                  
                }
                break;
              case 0x0F://�ļ�����
                {
                  
                }
                break;
              case 0x10://����ת��
                {
                  switch(dat->Fn)
                  {
                  case 0x01://͸��ת��
                    {
                      dat->PortNum  = buf[18];
                      dat->Len  = buf[22];
                      memcpy(&(dat->Buf[0]),&buf[24],dat->Len);
                      dat->Validity = true;
                      dat->FrameTypes  = DataForwarding;//����ת��
                    }
                    break;
                  default:
                    break; 
                  }
                }
                break;
              default:
                break;
              }
              
              
            }
          }
        } 
      }
    }
  }
}

