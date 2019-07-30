
#ifndef __GW3761_2013_H
#define __GW3761_2013_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
/*
  
AFN	FN	����	��������
      
//00	01	������	������ȷ��
//00	02	������	����������
  
//01	01	������	Ӳ����ʼ��������
//01	02	������	��������ʼ������������ļ�������          ���
//01	04	������	������ȫ�����ݳ�ʼ��������������ݡ�����  ��� IP �˿� ������ �ն��� ��� 
//04	03	������	������վIP�Ͷ˿�
  

  

 
��վ��������
  
0A	03	������	��ѯ��վIP�Ͷ˿�  ��վ----��ѯ  ������Ӧ�� --- IP��ַ  ��վ---ȷ��
09	01	������	��ѯ�������汾��  ��վ----��ѯ  ������Ӧ�� --- �汾��  ��վ---ȷ��
0C	02	������	��ȡ������ʱ��    ��վ----��ѯ  ������Ӧ�� --- ʱ��    ��վ---ȷ��
0A	10	������	��ѯ���          ��վ----��ѯ  ������Ӧ�� --- ���    ��վ---ȷ��
    
05	50	������	�����ֳ�          ��վ----�·�  ������Ӧ�� --- ȷ��
05	49	������	ֹͣ�ֳ�          ��վ----�·�  ������Ӧ�� --- ȷ��
10	01	��͸��ת���·���          ��վ----�·�  ������Ӧ�� ----ȷ��  
04	10	������	������ӱ��      ��վ----�·�  ������Ӧ�� --- ȷ��    
05	53	������	ɾ�����          ��վ----�·�  ������Ӧ�� --- ȷ��
  
����������վ    
                        �ն˵�¼  �������ϱ� -- ����  ��վӦ��---ȷ��         
                        ����      �������ϱ� -- ����  ��վӦ��---ȷ�� 
    
10	01	��	͸��ת���ϱ���        �������ϱ� -- ����  ��վӦ��---ȷ��
0D	10	������	�����ڵ��ϱ�      �������ϱ� -- ����  ��վӦ��---ȷ��

  
  1������
  2���ն˵�¼
  3: �ڵ������ϱ� 
  4����ȡ�������汾��
  5����ȡ������IP 
  6����ȡ������ʱ��
  7: �����ֳ�
  8��ֹͣ�ֳ�
  9������͸���·�
  
*/
  
  
  
  
  
  
  
  
#define GW3761_Frame_Max_Num    256
#define FrameStart              0x68
#define FrameEnd                0x16
  
#define FrameStart0offset       0x00
#define FrameStart1offset       0x05
#define FrameSerialNumH         0x07
#define FrameSerialNumL         0x08
#define FrameLen0offset         0x01//����֡����
#define FrameLen1offset         0x02//����֡����
#define FrameCtrloffset         0x06//������
#define FrameAddrOffset         0x09//��ַ��
#define AFNOffset               0x0C
#define DTOffset                0x10//��Ϣ��
#define DAOffset                0x0E//��Ϣ��
#define UserDataAreaAOffset     0x0A//��ַ��
#define UserDataAreaSAOffset    0x0A//Դ��ַ
#define UserDataAreaDAOffset    0x0A//Ŀ�ĵ�ַ
  //�û������� Ӧ�ù�����
#define UserDataAreaAFNAOffset  0x1C//�е�ַ������
#define UserDataAreaAFNNOffset  0x0A//�޵�ַ������
  //�û������� ���ݵ�Ԫ��ʶ
#define UserDataAreaFNAOffset   0x1D//�е�ַ������
#define UserDataAreaFNNOffset   0x0B//�޵�ַ������
  //�û������� ���ݵ�Ԫ
#define UserDataAreaDCNOffset   0x0D//�û������� ���ݵ�Ԫ
#define UserDataAreaMessageLenOffset 0x22//���ĳ���ƫ����
#define UserDataAreaMessageOffset    0x23//����ƫ����
  
  typedef enum 
  {
    Confirm          = 1,  //ȫ��ȷ�� 
    Deny             = 2,  //ȫ������
    GetGateWayVerNum = 3,  //��ȡ�豸�汾��
    GetGateWayRTC    = 4,  //��ȡ������ʱ��
    DataForwarding   = 5,  //����ת�� 
    ValveData        = 6,  //�����ϱ���������
    HeatMeterData    = 7,  //�ȱ��ϱ���������
    MainStationIP    = 8,  //��ѯ��վIP �� �˿ں�
    StartMeterReading= 9,  //�����ֳ�
    StopMeterReading = 10, //ֹͣ�ֳ�
    GateWayHeartBeat = 11, //���� 
    TerminalLogin    = 12, //��������¼
    LoraNetWorkLogin = 13, 
    
  } Enum_GW3761_State   ;
 
  typedef struct          
  {
    uint8_t   Validity  ;  //�Ϸ��� true �Ϸ�����֡  false  ��������֡
    uint8_t   ID[2]     ;  //������id
    uint16_t  FrameNum  ;  //����֡���
    uint8_t   FrameTypes;  //����֡����
    uint8_t   Dri ;        //����֡���� 
    uint8_t   Afn ;       
    uint8_t   Fn  ;
    uint8_t   Pn  ;     
    uint8_t   IP[6];
    uint8_t   DeviceType;//�豸����  
    uint8_t   DeviceID[6];//�豸���� 
    uint8_t   SignalStrength;
    Str_Calendar  Rtc  ;//��ǰʱ�� 
    uint8_t   PortNum  ;   //�˿ں� ���� AFN 10   6 �����Ƿ���  5�������ȱ�
    uint8_t   Buf[256] ;   //����֡
    uint8_t   Len ;        //����֡����   
  }Str_GW3761;
 
//  typedef union 
//  {
//    uint8_t Rtc[6];
//    struct          
//    {
//      uint8_t   Seconds ;
//      uint8_t   Minutes;
//      uint8_t   Hours;
//      uint8_t   Day;
//      uint8_t   Month:   5;
//      uint8_t   Week:    3;
//      uint8_t   Year;
//    }Str_GW3761_Rtc;
//  }Union_GW3761_Rtc;
  
  
  
  //������_����
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      uint8_t   FunctionCode:    4; //������
      bool      PWV:             1; //
      bool      ACD:             1;
      bool      PRM:             1;//PRM=1����ʾ��֡������������վ��PRM=0����ʾ��֡�������ԴӶ�վ��
      bool      DIR:             1;//0 ��վ����  1 �ն����� 
    }Str_GW3761_Ctrl_Up;
  }Union_GW3761_Ctrl_Up;
  
  //������_����
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      uint8_t   FunctionCode:   4;
      bool      FCV:            1;//FCV=1����ʾFCBλ��Ч��FCV=0����ʾFCBλ��Ч��
      bool      FCB:            1;
      bool      PRM:            1;
      bool      DIR :           1;
    }Str_GW3761_Ctrl_Down;
  }Union_GW3761_Ctrl_Down;
  
  /*����ͨѶģ������ģʽ��Ϣ���ݵ�Ԫ��ʽ*/
  
  
  //֡������SEQ����
  //֡������SEQΪ1�ֽڣ���������֮֡��Ĵ�����
  //�еı仯���������ܱ��ĳ������ƣ�������
  //����һ֡�ڴ��䣬��Ҫ�ֳɶ�֡����
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      uint8_t   PSEQ_RSEQ :     4;
      bool      CON :           1;
      bool      FIN:            1;
      bool      FIR:            1;
      bool      TpV :           1;
    }Str_GW3761_SEQ;
  }Union_GW3761_SEQ;
 
//��Ϣ��
  typedef union 
  {
    uint8_t R;
    struct          
    {
      bool RouteIdentifier:             1;
      bool AttachedNodeIdentifier:      1;
      bool CommunicationModuleIdentifier: 1;
      bool CollisionDetection:          1;
      uint8_t RelayLevel :              4;
    }Str_GW3762_R;
  }Union_GW3762_R;
  
typedef struct 
  {
    uint8_t len;
    uint8_t buf[256];
  }Str_GW3761_Comm;
  
void GW3761_Frame_Resolve(uint8_t *spoint,uint8_t len,Str_GW3761 *dat);
void GW3761_Frame_Comb(Str_GW3761_Comm *tx,Str_GW3761 *msg);
  
#ifdef __cplusplus
}
#endif
#endif
