/********************************************************************
                       VIIC_C51.C
  �˳�����I2C����ƽ̨������ʽ�����ƽ̨���ĵײ��C�ӳ���,�緢������
����������,Ӧ��λ����,���ṩ�˼���ֱ����������Ĳ������������ܷ����
���û��������Ӳ���չ.....  
  
    ע��:�����ǲ��������ʱ�ķ�������SCL����,�̶Ը߾���Ƶ��Ҫ�� 
һ�����޸�....(������1us��������,������Ƶ��ҪС��12MHZ)

********************************************************************/
    
#include "includes.h"        

   

void IIC_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = SCL_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(SCL_Port,&GPIO_InitStructure);  
  
  GPIO_InitStructure.GPIO_Pin = SDA_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(SDA_Port,&GPIO_InitStructure);  
  
}



/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  
����:       ����I2C����,������I2C��ʼ����.
  
********************************************************************/
void Start_IIc(void)
{
  SDA_HIGH();   /*������ʼ�����������ź�*/
  Delay_us(1);
  SCL_HIGH();
  Delay_us(5);  /*��ʼ��������ʱ�����4.7us,��ʱ*/   
  SDA_LOW();    /*������ʼ�ź�*/
  Delay_us(5);  /* ��ʼ��������ʱ�����4��s*/      
  SCL_LOW();    /*ǯסI2C���ߣ�׼�����ͻ�������� */
  Delay_us(2);
}

/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  
����:       ����I2C����,������I2C��������.
  
********************************************************************/
void Stop_IIc(void)
{
  SDA_LOW();    /*���ͽ��������������ź�*/
  Delay_us(1);  /*���ͽ���������ʱ���ź�*/
  SCL_HIGH();   /*������������ʱ�����4��s*/
  Delay_us(5);
  SDA_HIGH();   /*����I2C���߽����ź�*/
  Delay_us(4);
}

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: void  SendByte(uchar c);
����:  ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
     ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0 ��)     
     ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
bool SendByte(uint8_t c)
{
  uint8_t BitCnt;
  bool ack;
 
  for(BitCnt=0;BitCnt<8;BitCnt++)/*Ҫ���͵����ݳ���Ϊ8λ*/
  {
    if((c<<BitCnt)&0x80)
      SDA_HIGH();       /*�жϷ���λ*/
    else  
      SDA_LOW();                
    Delay_us(1);
    SCL_HIGH();         /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
    Delay_us(5);        /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/                  
    SCL_LOW(); 
  }
  Delay_us(2);
  SDA_HIGH();           /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
  Delay_us(2);  
  SCL_HIGH();
  Delay_us(3);
  if(ReadSDA()==1)
    ack=0;     
  else 
    ack=1;              /*�ж��Ƿ���յ�Ӧ���ź�*/
  SCL_LOW();
  Delay_us(2);
 
  return(ack);
}

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: uchar  RcvByte();
����:  �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
     ���������Ӧ������  
********************************************************************/	
uint8_t  RcvByte(void)
{
  uint8_t retc=0;
  uint8_t BitCnt;
  
  
  SDA_HIGH();           /*��������Ϊ���뷽ʽ*/
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    Delay_us(1);           
    SCL_LOW();          /*��ʱ����Ϊ�ͣ�׼����������λ*/
    Delay_us(5);        /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/        
    SCL_HIGH();         /*��ʱ����Ϊ��ʹ��������������Ч*/
    Delay_us(2);
    retc=retc<<1;
    if(ReadSDA()==1)
      retc++;           /*������λ,���յ�����λ����retc�� */
    Delay_us(2);
  }
  SCL_LOW();    
  Delay_us(2);
 
  
  return(retc);
}

/********************************************************************
                     Ӧ���Ӻ���
ԭ��:  void Ack_I2c(bit a);
 
����:����������Ӧ���ź�,(������Ӧ����Ӧ���ź�)
********************************************************************/
void Ack_IIc(const bool ack)
{
  if( ack == 0 )
    SDA_LOW();          /*�ڴ˷���Ӧ����Ӧ���ź� */
  else 
    SDA_HIGH();
  Delay_us(3);  
  SCL_HIGH();
  Delay_us(5);          /*ʱ�ӵ͵�ƽ���ڴ���4��s*/         
  SCL_LOW();            /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
  Delay_us(2);   
}






/*******************************************************************
                    �����ӵ�ַ���������ֽ����ݺ���               
����ԭ��: bit  ISendByte(uchar sla,ucahr c);  
����:     ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ����,��������ַsla.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
bool ISendByte(const uint8_t sla,const uint8_t c)
{
  bool ack;

  
  Start_IIc();          /*��������*/
  ack = SendByte(sla);  /*����������ַ*/
  if(ack==0)return(0);
  ack = SendByte(c);    /*��������*/
  if(ack==0)return(0);
  Stop_IIc();           /*��������*/ 
  
  
  return(1);
}



/*******************************************************************
                    �����ӵ�ַ�������Ͷ��ֽ����ݺ���               
����ԭ��: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba������������sָ������ݣ�����no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
bool ISendStr(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t no)
{
  uint8_t i;
  bool ack;
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  Start_IIc();          /*��������*/
  ack = SendByte(sla);  /*����������ַ*/
  if(ack==0)return(0);
  ack = SendByte(suba); /*���������ӵ�ַ*/
  if(ack==0)return(0);
  
  for(i=0;i<no;i++)
  {   
    ack = SendByte(*s); /*��������*/
    if(ack==0)return(0);
    s++;
  } 
  Stop_IIc();           /*��������*/ 
  
  OS_CPU_SR_Restore(cpu_sr);
  
  return(1);
}






/*******************************************************************
                    �����ӵ�ַ�������ֽ����ݺ���               
����ԭ��: bit  IRcvByte(uchar sla,ucahr *c);  
����:     ���������ߵ����͵�ַ�������ݣ��������ߵ�ȫ����,��������
          ַsla������ֵ��c.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
bool IRcvByte(uint8_t sla,uint8_t *c)
{
  bool ack;
  OS_CPU_SR  cpu_sr = 0;
  
  cpu_sr = OS_CPU_SR_Save();
  
  Start_IIc();          /*��������*/
  ack = SendByte(sla+1);/*����������ַ*/
  if(ack==0)return(0);
  *c=RcvByte();         /*��ȡ����*/
  Ack_IIc(1);           /*���ͷǾʹ�λ*/
  Stop_IIc();           /*��������*/ 
  
  OS_CPU_SR_Restore(cpu_sr);
  
  return(1);
}



/*******************************************************************
                    �����ӵ�ַ������ȡ���ֽ����ݺ���               
����ԭ��: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,�����ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba�����������ݷ���sָ��Ĵ洢������no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
bool IRcvStr(uint8_t sla,uint8_t suba,uint8_t *s,uint8_t no)
{
  uint8_t i;
  bool ack;
  
  Start_IIc();          /*��������*/
  ack = SendByte(sla);  /*����������ַ*/
  if(ack==0)return(0);
  ack = SendByte(suba); /*���������ӵ�ַ*/
  if(ack==0)return(0);
  
  Start_IIc();
  ack = SendByte(sla+1);
  if(ack==0)return(0);
  
  for(i=0;i<no-1;i++)
  {   
    *s=RcvByte();       /*��������*/
    Ack_IIc(0);         /*���;ʹ�λ*/  
    s++;
  } 
  *s=RcvByte();
  Ack_IIc(true);        /*���ͷ�Ӧλ*/
  Stop_IIc();           /*��������*/ 
  return(1);
}


                        /*    ���      */

