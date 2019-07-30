
#include "includes.h"
//0-3   id 
//4-4   ������ʶ  0 δ����  0xff �Ѿ�����
//5-5   ����     
//6-127 ����֡
static uint8_t MeterLib[MeterLibraryMaxNum][128] ; //128��ÿ����6�ֽڱ��
static uint8_t MeterLibDataNum = 0;
static uint8_t MeterLibEquipmentNum=0;

/*******************************************************************************
** Function name:           Meter_Lib_Init
** Descriptions:            ����ʼ��  ��Ҫ40ms���
** input parameters:        void
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Meter_Lib_Init(void)
{
  memset(&MeterLib[0][0] ,0x00,(MeterLibraryMaxNum*128));//����
  MeterLibDataNum=0;
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
uint8_t Insert_Meter_Lib(uint8_t *id)
{
  uint8_t num = 0;
  uint8_t  tem[4]={0};
  
  for(num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&tem[0],&MeterLib[num][0],4)==0))//��һ����λ�� 
    {
      memset(&MeterLib[num][0],0x00,128);//����
      memcpy(&MeterLib[num][0],&*id,4);//�����ڴ�
      MeterLibEquipmentNum++;
      break;
    }
  }

  return(num);
}

/*******************************************************************************
** Function name:           Reset_Meter_Lib
** Descriptions:            ���ñ����ĳһ��λ��  �����   
** input parameters:        void
** output parameters:       void
** Returned value:          ��Ų����ȥ ������λ�ú�
** Created by:              ������
** Created Date:            2016-2-22       
*******************************************************************************/
void Reset_Meter_Lib(uint8_t num)
{
  memset(&MeterLib[num][0],0x00,128);//��Ӧ�������
  if(MeterLibEquipmentNum>0)
  {
    MeterLibEquipmentNum--;
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
uint8_t Count_Meter_Lib_Num(void)
{
  return(MeterLibEquipmentNum);
}

/*
������645����֡ 68 17 00 10 52 00 00 68 01 02 34 23 A3 16

����������
*/
void Insert_Meter_Lib_DataItem(uint8_t *buf,uint8_t len)
{
  uint8_t recvbuf[128]={0};
  
  memcpy(&recvbuf[0],&*buf,len);
  
  for(uint8_t num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&recvbuf[1],&MeterLib[num][0],4)==0))//�ҵ���Ӧ���
    {
      MeterLibDataNum++;
      MeterLib[num][5] = len;
      memset(&MeterLib[num][6],0x00,122);        //����
      memcpy(&MeterLib[num][6],&recvbuf[0],len); //�����ڴ�
      break;
    }
  }
}


uint8_t Get_Meter_Lib_DataItem(uint8_t *buf,uint8_t node)
{ 
  uint8_t len=0;
  
  len = MeterLib[node][5];
  if(len>0)
  {
    memcpy(&*buf,&MeterLib[node][6],len); //������buf��
    memset(&MeterLib[node][5],0x00,123);  //����
    if(MeterLibDataNum > 0)
    {
      MeterLibDataNum--;
    }
  }
  return(len);
}

uint8_t Get_Meter_Lib_DataItem_Num(void)
{
  return( MeterLibDataNum );
}






  
/*******************************************************************************
** Function name:           Get_Meter_Lib_NodeNum
** Descriptions:            ���ݱ��ȡ�ýڵ��  ������վ��Ҫĳһ����Ķ�������  ����ͨ���˷���ȡ��
** input parameters:        void
** output parameters:       void
** Returned value:          ��Ų����ȥ ������λ�ú�
** Created by:              ������
** Created Date:            2016-2-22    STM32  72M��Ƶ  ����128���� ��Ҫ125u   
*******************************************************************************/
int8_t Get_Meter_Lib_NodeNum(uint8_t *id)
{
  uint8_t num=0;
  
  for(num=0;num<MeterLibraryMaxNum;num++)
  {
    if((memcmp(&id[0],&MeterLib[num][0],4)==0))
    {
      break;
    }
  }
  
  return(num);
}

/*******************************************************************************
** Function name:           Get_Meter_Lib_MeterNum
** Descriptions:            ���ݽڵ��� ȡ�ö�Ӧ���
** input parameters:        void
** output parameters:       void
** Returned value:          ��Ų����ȥ ������λ�ú�
** Created by:              ������
** Created Date:            2016-2-22    STM32  72M��Ƶ  ����128���� ��Ҫ125u   
*******************************************************************************/
void Get_Meter_Lib_MeterNum(uint8_t *id,uint8_t nodenum)
{
  memcpy(&*id,&MeterLib[nodenum][0],4);
}


















