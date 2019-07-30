#include "includes.h"




 
 
/*******************************************************************************
** Function name:           Meter_Lib_Init
** Descriptions:            ����ʼ��   
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Meter_Lib_Init(void)
{
  for( uint16_t loop = 0;loop<MeterLibMaxNum;loop++)
  {
    memset(&MeterLib[loop].MeterID[0],0,6);//���ID
    MeterLib[loop].CommNum  = 0 ;//���
    MeterLib[loop].DatLen   = 0 ;
    MeterLib[loop].TaskLoop = 0 ;
  }
}
 
uint8_t Seek_Lib_ID(uint8_t *id,uint16_t *stn)//���ݱ�� ����ʱ϶�� ������Ǯ��Ҫ����һ����û���ظ��ı��
{
  for( uint16_t tem = 0;tem < MeterLibMaxNum;tem++)
  {
    if((memcmp(&MeterLib[tem].MeterID[0],&*id,6)==0))//�ҵ���Ӧ���
    {
      *stn = tem;
      return(true);//���ص�ǰ��ʱ϶��
    }
  }
  
  return(false);
}





/*******************************************************************************
** Function name:           Insert_Meter_Lib
** Descriptions:            ����в���һ�����  ������� ֱ���ҵ�һ���յ�λ��  
** input parameters:        void
** output parameters:       void
** Returned value:          ��Ų����ȥ ������λ�ú� ���λ�úŵ������������ 
                            ��Ϊ����
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/ 
uint8_t Insert_Meter_Lib(uint8_t *id,uint16_t *stn)
{
  uint16_t loop = 0;
  uint16_t num  = 0;
  uint16_t tem  = 0;
  
  
  for(loop=0;loop<16;loop++)
  {
    for(num=0;num<29;num++)
    {
      tem = loop + num * 16;
      
      if(MeterLib[tem].CommNum == 0)
      {
        MeterLib[tem].CommNum  = 3 ;
        MeterLib[tem].DatLen   = 0 ;
        MeterLib[tem].TaskLoop = 0;
        memcpy(&MeterLib[tem].MeterID[0],&*id,6);//�����ڴ�
        *stn = tem ;
        return(true);
      }
    }
  }
  
  return(false);
}

/*******************************************************************************
** Function name:           Insert_Meter_Lib_DownData
** Descriptions:            ���ݱ�� ��645���ݲ���  ������645����֡ 68 17 00 10 52 00 00 68 01 02 34 23 A3 16
** input parameters:        id ����� buf ��ǰ�û��·�������֡ len������֡����
** output parameters:       �Ƿ����ɹ�
** Returned value:          true ����ɹ� false δ����ɹ� 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Insert_Meter_Lib_Data(uint8_t *id,uint8_t *buf,uint8_t len)
{
 
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if((memcmp(&MeterLib[num].MeterID[0],&*id,6)==0))//�ҵ���Ӧ���
    {
      memcpy(&MeterLib[num].DatBuf[0],&*buf,len);
      MeterLib[num].DatLen = len;
      return(true);
    }
  }
  
  return(false);
}
 
/*******************************************************************************
** Function name:           Maintenance_MeterLib
** Descriptions:            ���ϵͳά��  ������������Ϊ0  �����  ÿ�����ϸ�λǰ����
** input parameters:        void
** output parameters:       void
** Returned value:          ��Ų����ȥ ������λ�ú� ���λ�úŵ������������ 
                            ��Ϊ����
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Maintenance_MeterLib(void)
{
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if(MeterLib[num].CommNum == 0)
    {
      MeterLib[num].CommNum  = 0 ;//ͨ�Ŵ��� 
      MeterLib[num].DatLen   = 0 ;//
      MeterLib[num].TaskLoop = 0 ;
    }
  }
}


/*******************************************************************************
** Function name:           Count_Meter_Lib_Num
** Descriptions:            ͳ�Ʊ���б������
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint16_t Count_Meter_Lib_Num(void)
{
  uint16_t num=0;
  
  for(uint16_t num=0;num<MeterLibMaxNum;num++)
  {
    if(MeterLib[num].CommNum > 0)
    {
      num++;//����
    }
  }

  return(num);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData
** Descriptions:            ����ʱ϶��� ��ȡ��Ӧ���·�����֡  ���ǲ���ɾ������
** input parameters:        stn �� ʱ϶�� buf ��ʱ϶�Ŷ�Ӧ������֡  
** output parameters:       ����֡���� 
** Returned value:           �緵��0 ���ʾû��ȡ������
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_Data(uint16_t stn,uint8_t *buf)
{ 
  uint8_t len = 0;
  
  if(MeterLib[stn].DatLen > 0)
  {
    len = MeterLib[stn].DatLen;//�������֡���� 
    memcpy(&*buf,&MeterLib[stn].DatBuf[0],len); //������buf��
  }
  
  return(len);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData
** Descriptions:            ����ʱ϶��� ��ȡ��Ӧ���·�����֡  ���ǲ���ɾ������
** input parameters:        stn �� ʱ϶�� id ��ʱ϶�Ŷ�Ӧ��ID  num ����Ҫ�������������� 
** output parameters:       ����֡���� 
** Returned value:           �緵��0 ���ʾû��ȡ������
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_ID(uint16_t stn,uint8_t *id,uint8_t num)
{ 
 
  if(MeterLib[stn].CommNum == 0)//�˴��нڵ� 
  {
    return(false);
  }
  else
  {
    memcpy(&*id,&MeterLib[stn].MeterID[0],num); //������buf��
    return(true);
  }
}


uint8_t Empty_Meter_Lib_ID(uint16_t stn)
{ 
  if( stn < MeterLibMaxNum )
  {
    memset(&MeterLib[stn].MeterID[0],0,6);//���ID
    MeterLib[stn].CommNum  = 0 ;//���
    MeterLib[stn].DatLen   = 0 ;
    MeterLib[stn].TaskLoop = 0 ;
    return(true );
  }
  else
  {
    return(false);
  }
}

/*******************************************************************************
** Function name:           Empty_Meter_Lib_Data
** Descriptions:            ����ʱ϶��� ��ն�Ӧ���·�����֡  
** input parameters:        stn �� ʱ϶�� 
** output parameters:       ����֡���� 
** Returned value:           �緵��0 ���ʾû��ȡ������
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Empty_Meter_Lib_Data(uint16_t stn)
{ 
  if( stn < MeterLibMaxNum )
  {
    MeterLib[stn].DatLen = 0;//�������֡���� 
    return(true );
  }
  else
  {
    return(false);
  }
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_DownData_State
** Descriptions:            ����ʱ϶�� �����Ƿ����·�����
** input parameters:        stn �� ʱ϶��  
** output parameters:        
** Returned value:           �緵��0 ���ʾû��ȡ������
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
uint8_t Get_Meter_Lib_Data_State(uint16_t stn)
{ 
  if(MeterLib[stn].DatLen == 0)
  {
    return(false) ;
  }
  else
  {
    return(true) ;
  }
}


