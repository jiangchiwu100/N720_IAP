

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
** Descriptions:        //计算FN
** input parameters:    /f 输入指针
** Returned value:      //返回计算出FN
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
  case DataForwarding:	//数据帧透传上行
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
      
      tx->buf[tx->len++] = msg->PortNum  ;//端口号 
      tx->buf[tx->len++] = msg->Len  ; //数据帧长度
      tx->buf[tx->len++] = 0x00 ; 
      
      memcpy(&tx->buf[tx->len],&(msg->Buf[0]),msg->Len);
      tx->len = tx->len + msg->Len;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++; 
    }
    break;
  case GetGateWayRTC:		//读取集中器时间
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
  case MainStationIP:   //读取集中器ip
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
  case GetGateWayVerNum://读取集中器版本
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
      
      tx->buf[tx->len++] = 0x58 ;//厂商代码
      tx->buf[tx->len++] = 0x4B ;
      tx->buf[tx->len++] = 0x53 ;
      tx->buf[tx->len++] = 0x44 ;
      
      tx->buf[tx->len++] = 0x0 ;//设备编号
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ; 
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;
      
      tx->buf[tx->len++] = 0x0 ;//终端软件版本号
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;//终端软件发布日期：日月年
      tx->buf[tx->len++] = 0x09 ;
      tx->buf[tx->len++] = 0x19 ;
      
      
      tx->buf[tx->len++] = 0x01 ;
      tx->buf[tx->len++] = 0x0 ;//终端配置容量信息码
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ; 
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      
      tx->buf[tx->len++] = 0x0 ;//终端通信协议.版本号
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      
      tx->buf[tx->len++] = 0x0 ;//终端硬件版本号
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x0 ;
      tx->buf[tx->len++] = 0x01 ;//终端硬件发布日期：日月年
      tx->buf[tx->len++] = 0x09 ;
      tx->buf[tx->len++] = 0x19 ;
      
      tx->buf[tx->len++] = Count_CheckSum(&(tx->buf[0]),tx->len);
      tx->buf[tx->len  ] = FrameEnd;
      tx->len++;
      
      
    }
    break;
  case Confirm:					//确认
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
  case Deny:   					//否认
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
  case GateWayHeartBeat://心跳
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
  case TerminalLogin:		//终端登录
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
  //接收无线的数据帧 
  if(len != 0 || len < GW3761_Frame_Max_Num)
  {
    memcpy(&buf[0],&*spoint,len);  
    
    if(buf[FrameStart0offset] == FrameStart)                                      //帧头0是否正确
    {
      if(buf[FrameStart1offset] == FrameStart)                                    //帧头1是否正确
      {
        framelen.B8[0] = buf[FrameLen0offset] ; 
        framelen.B8[1] = buf[FrameLen1offset] ; 
        framelen.B16   = framelen.B16>>2;                                         //数据帧长度
        
        if(buf[framelen.B16 + FrameStart1offset + 2] == FrameEnd)                 //数据帧尾是否正确
        {                                                                         //数据帧校验和是否正确
          if(buf[ framelen.B16 + FrameStart1offset + 1 ] == Count_CheckSum(&buf[FrameCtrloffset],framelen.B16))       
          {                                                                     //校验和是否正确                                                        
            //判断是那个数据项 并做相应处理
            dat->Afn = buf[ AFNOffset ];//应用层功能码
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
              case 0x00://确认 否认
                {
                  
                  switch(dat->Fn)
                  {
                  case 0x01: 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = Confirm;//全部确认
                    }
                    break;
                  case 0x02: 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = Deny;//全部否认
                    }
                    break;
                  default:
                    break;
                  }
                }
                break;
              case 0x01://复位
                {
                  
                }
                break;
              case 0x02://链路接口检测
                {
                  
                }
                break;
              case 0x03://中继站命令
                {
                  
                }
                break;
              case 0x04://设置参数
                {
                  switch( dat->Fn )
                  {
                  case 3://设置IP地址和端口号
                    {
                      
                    }
                    break;
                  case 10://加载表库
                    {
                      
                    }
                    break;
                  default:
                    break;
                  }
                }
                break;
              case 0x05://控制命令
                {
                  switch(dat->Fn)
                  {
                  case 49://下发时间
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = StopMeterReading;//启动轮抄 
                    }
                    break;
                  case 50://启动轮抄  
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = StartMeterReading;//启动轮抄
                    }
                    break;
                  default:
                    break; 
                  }
                }
                break;
              case 0x06://身份认证以及密钥协商
                {
                  
                }
                break;
              case 0x07://备用
                {
                  
                }
                break;
              case 0x08://请求被级联终端主动上报
                {
                  
                }
                break;
              case 0x09://读取集中器版本号
                {
                  switch( dat->Fn )
                  {
                  case 1:// 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = GetGateWayVerNum;//读取集中器版本号
                    }
                    break;
                  }
                }
                break;
              case 0x0A://查询参数
                {
                  switch( dat->Fn )
                  {
                  case 3:
                    {
                      dat->Validity    = true;
                      dat->FrameTypes  = MainStationIP;//查询主站IP 端口号
                    }
                    break;
                  case 10:////////////////////////////??????????????????????查询电表信息 
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
              case 0x0B://请求任务数据
                {
                  
                }
                break;
              case 0x0C://请求1类数据---实时数据
                {
                  switch( dat->Fn )
                  {
                  case 2://读取集中器时间 
                    {
                      dat->Validity = true;
                      dat->FrameTypes  = GetGateWayRTC;//读读取集中器时间
                    }
                    break;
                  }
                }
                break;
              case 0x0D://请求2类数据---历史数据
                {
                  
                }
                break;
              case 0x0E://请求2类数据---事件数据
                {
                  
                }
                break;
              case 0x0F://文件传输
                {
                  
                }
                break;
              case 0x10://数据转发
                {
                  switch(dat->Fn)
                  {
                  case 0x01://透明转发
                    {
                      dat->PortNum  = buf[18];
                      dat->Len  = buf[22];
                      memcpy(&(dat->Buf[0]),&buf[24],dat->Len);
                      dat->Validity = true;
                      dat->FrameTypes  = DataForwarding;//数据转发
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

