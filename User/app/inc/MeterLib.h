
#ifndef __MeterLib_H
#define __MeterLib_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
//    typedef struct
//    { 
//      uint16_t  TaskID        ;//����ִ�е��������кţ�0��ʾû��������ִ�У����ͳɹ�ʱ=������������кţ����ճɹ�ʱ���ݸ����кŶ�ȡ������֡���͸�ǰ�û�
//      uint8_t   TaskLoop      ;//��ĳһ�������ѭ��ִ�д���  ���3�� ������ִ�Լ3�� ��Ҫ����������
//      uint8_t   len           ;
//      uint8_t   buf[64]       ;//�����ֺ�ʱ�������ͳɹ�������Ϊ������ĳ�ʱ������ÿ�ּ��-1�����==0���ǰ�û��ظ�ʧ��
//    }Str_PingTask        ;// ���Ҫ��
  
  
  
 
   typedef  struct
    { 
      uint8_t    MeterID[6]    ;//���ַ
      uint8_t    CommNum       ;//ͨ�ųɹ����� ���CommNum����0 ����˵���˴�Ϊ��
      uint16_t   TaskID        ;//����� 
      uint8_t    TaskLoop      ;//����ѭ��ִ�д���  ���ܴ���3�� �������3�ξͰ���ɾ���� 
      uint8_t    DatLen        ;//�·����ݵĳ���  �������0 ����û������
      uint8_t    DatBuf[64]    ;//�·�����
    }Str_MeterLib_Parm         ;// ���Ҫ��
 
 
  
 
  

void ReadTest(void);
void WriteTest(uint8_t _Value);

void Maintenance_MeterLib(void);
uint8_t Seek_Lib_ID(uint8_t *id,uint16_t *stn);
uint8_t Empty_Meter_Lib_ID(uint16_t stn);
uint16_t Count_Meter_Lib_Num(void);
uint8_t Insert_Meter_Lib(uint8_t *id,uint16_t *stn);
uint8_t Insert_Meter_Lib_Data(uint8_t *id,uint8_t *buf,uint8_t len);
uint8_t Get_Meter_Lib_Data(uint16_t stn,uint8_t *buf);
uint8_t Get_Meter_Lib_ID(uint16_t stn,uint8_t *id,uint8_t num);

uint8_t Get_Meter_Lib_Data_State(uint16_t stn);
uint8_t Empty_Meter_Lib_Data(uint16_t stn);

#ifdef __cplusplus
}
#endif
#endif
