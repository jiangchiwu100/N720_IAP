
#include "DriverLib.h"





bool Message_Empty(Str_Message *msgpoint)
{
  if(msgpoint->In == msgpoint->Out)
    return(true);
  else
    return(false);
}

uint8_t Message_Array_Num(Str_Message *msgpoint)
{
  return(msgpoint->MsgNum);
}

void W_Message_Array_List(Str_Message *msgpoint, uint8_t data)
{
  if(msgpoint->Len[msgpoint->In] >= MegArr_List_Max)
    msgpoint->Len[msgpoint->In]=0;
  msgpoint->Msg[msgpoint->In][msgpoint->Len[msgpoint->In]] = data;
  msgpoint->Len[msgpoint->In]++;
}


bool W_Message_Array_Line(Str_Message *msgpoint)
{
  if((msgpoint->In+1)%MegArr_Line_Max == msgpoint->Out)//队满
    return(false);
  else
  {
    msgpoint->In = (msgpoint->In+1) % MegArr_Line_Max;
    msgpoint->Len[msgpoint->In] = 0;
    msgpoint->MsgNum++;
    return(true);
  }
}

uint8_t R_Message_Array_Line(Str_Message *msgpoint, uint8_t *data)
{
  uint8_t outnum=0;
  if(Message_Empty(msgpoint))//如果消息数组为空 
  {return(false);}
  else
  { 
    memcpy(&*data,&(msgpoint->Msg[msgpoint->Out][0]),msgpoint->Len[msgpoint->Out]);//接收无线的数据帧 
    outnum = msgpoint->Len[msgpoint->Out];
    msgpoint->Out = (msgpoint->Out+1)%MegArr_Line_Max;
    msgpoint->MsgNum--;
    return(outnum);
  }
}
