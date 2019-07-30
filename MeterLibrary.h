
#ifndef __MeterLibrary_H
#define __MeterLibrary_H


#ifdef __cplusplus
extern "C" {
#endif 
  
  //���������� 128
  //ÿ����ռ��3��page 128������Ҫ384ҳ  ǰ0--114ҳ����������� 
  
  //0ҳ---7ҳ  ��ű��  ÿҳ���256�ֽ� 32���� 8ҳһ�����Դ�� 8*32 = 256����

  //8ҳ��ʼ��������������� �����غ� �ȵ� 
  
/*

  115ҳ
ҳ��ַ x*256

  */
#define MeterLibAddr(NodeNum)     ( NodeNum  * 8  ) 
  
#define MeterLibraryMaxNum      128
#define MeterLibraryPageOffset  114    
  
#define PageAddr( x )   ( (MeterLibraryPageOffset + x * 3 ) * 256 )//��x�����ҳ��ַ
#define MeterDataItem( x , y ) ( PageAddr( x ) + (y-1) * 128 )//��x����ĵ�y������֡
  
//���ݷֲ�˵�� �� ÿ������3������ҳ  ��һ��128�ֽ�
  
  
  /*���������������eeprom�е�λ�õ�ַ   0-41 
  NodeNum  PageNum
  0  - 41     0
  42 - 83     1  
  84 - 125    2  
  126-        3  
  */
  

  // ��n����ַ              MeterDataItem(n)
  // ��n����ַ ��x��������  MeterDataItem(n) + MeterDataItem(x)
  // ��n����ַ �ն���       MeterDataItem(n) + MeterDayFreeze()
  
  /*
  ʵ�� �Ե�ַ0��ʼ�����ݷ���
  MeterAddr(0)       0  ----127  ���   ����֡����  ����ʱ���  ͨ��ʧ�ܴ���  
  MeterDataItem(0)   128----255  �����վ�·�������֡ 0 
  MeterDataItem(1)   256----383  �����վ�·�������֡ 1 
  MeterDataItem(2)   384----511  �����վ�·�������֡ 2 
  MeterDayFreeze     512----639  ���ÿ����ն���
  reserve            640----767  ����
  MeterAddr(1)       768   
  */
  
  /*
  MeterAddr(0)       0  ----127  �����ַ����
  ͨ��ʧ�ܴ���---����3�ξ�ɾ��������� 
  
  0--3 id
  4    ����֡����
  5--6 ����ʱ���
  7    ͨ��ʧ�ܴ���
  */
  
  typedef union 
  {
    uint8_t Meter[128];
    struct          
    {
      uint8_t  Id[4];            //�ڵ���
      uint8_t  Data[124];
    }Str_MeterLib;
  }Union_Meter_Lib;
  

  
void    Meter_Lib_Init(void);
uint8_t Count_Meter_Lib_Num(void);

uint8_t Insert_Meter_Lib(uint8_t *id);
void    Reset_Meter_Lib(uint8_t num);

int8_t  Get_Meter_Lib_NodeNum(uint8_t *id);
void    Get_Meter_Lib_MeterNum(uint8_t *id,uint8_t nodenum);

void    Insert_Meter_Lib_DataItem(uint8_t *buf,uint8_t len);
uint8_t Get_Meter_Lib_DataItem(uint8_t *buf,uint8_t node);
uint8_t Get_Meter_Lib_DataItem_Num(void);


#ifdef __cplusplus
}
#endif
#endif





