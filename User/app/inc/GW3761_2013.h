
#ifndef __GW3761_2013_H
#define __GW3761_2013_H

#ifdef __cplusplus
extern "C" {
#endif 
    
 
/*
  
AFN	FN	类型	操作名称
      
//00	01	集中器	集中器确认
//00	02	集中器	集中器否认
  
//01	01	集中器	硬件初始化、重启
//01	02	集中器	数据区初始化、清空配置文件、重启          清除
//01	04	集中器	参数及全体数据初始化、清空所有数据、重启  清除 IP 端口 数据区 日冻结 表库 
//04	03	集中器	设置主站IP和端口
  

  

 
主站到集中器
  
0A	03	集中器	查询主站IP和端口  主站----问询  集中器应答 --- IP地址  主站---确认
09	01	集中器	查询集中器版本号  主站----问询  集中器应答 --- 版本号  主站---确认
0C	02	集中器	读取集中器时间    主站----问询  集中器应答 --- 时间    主站---确认
0A	10	集中器	查询表库          主站----问询  集中器应答 --- 表库    主站---确认
    
05	50	集中器	启动轮抄          主站----下发  集中器应答 --- 确认
05	49	集中器	停止轮抄          主站----下发  集中器应答 --- 确认
10	01	表透明转发下发：          主站----下发  集中器应答 ----确认  
04	10	集中器	发送添加表库      主站----下发  集中器应答 --- 确认    
05	53	集中器	删除表库          主站----下发  集中器应答 --- 确认
  
集中器到主站    
                        终端登录  集中器上报 -- 数据  主站应答---确认         
                        心跳      集中器上报 -- 数据  主站应答---确认 
    
10	01	表	透明转发上报：        集中器上报 -- 数据  主站应答---确认
0D	10	集中器	入网节点上报      集中器上报 -- 数据  主站应答---确认

  
  1：心跳
  2：终端登录
  3: 节点入网上报 
  4：读取集中器版本号
  5：读取集中器IP 
  6：读取集中器时间
  7: 启动轮抄
  8：停止轮抄
  9：数据透传下发
  
*/
  
  
  
  
  
  
  
  
#define GW3761_Frame_Max_Num    256
#define FrameStart              0x68
#define FrameEnd                0x16
  
#define FrameStart0offset       0x00
#define FrameStart1offset       0x05
#define FrameSerialNumH         0x07
#define FrameSerialNumL         0x08
#define FrameLen0offset         0x01//数据帧长度
#define FrameLen1offset         0x02//数据帧长度
#define FrameCtrloffset         0x06//控制域
#define FrameAddrOffset         0x09//地址域
#define AFNOffset               0x0C
#define DTOffset                0x10//信息域
#define DAOffset                0x0E//信息域
#define UserDataAreaAOffset     0x0A//地址域
#define UserDataAreaSAOffset    0x0A//源地址
#define UserDataAreaDAOffset    0x0A//目的地址
  //用户数据区 应用功能码
#define UserDataAreaAFNAOffset  0x1C//有地址域的情况
#define UserDataAreaAFNNOffset  0x0A//无地址域的情况
  //用户数据区 数据单元标识
#define UserDataAreaFNAOffset   0x1D//有地址域的情况
#define UserDataAreaFNNOffset   0x0B//无地址域的情况
  //用户数据区 数据单元
#define UserDataAreaDCNOffset   0x0D//用户数据区 数据单元
#define UserDataAreaMessageLenOffset 0x22//报文长度偏移量
#define UserDataAreaMessageOffset    0x23//报文偏移量
  
  typedef enum 
  {
    Confirm          = 1,  //全部确认 
    Deny             = 2,  //全部否认
    GetGateWayVerNum = 3,  //读取设备版本号
    GetGateWayRTC    = 4,  //读取集中器时间
    DataForwarding   = 5,  //数据转发 
    ValveData        = 6,  //阀门上报类型数据
    HeatMeterData    = 7,  //热表上报类型数据
    MainStationIP    = 8,  //查询主站IP 和 端口号
    StartMeterReading= 9,  //启动轮抄
    StopMeterReading = 10, //停止轮抄
    GateWayHeartBeat = 11, //心跳 
    TerminalLogin    = 12, //集中器登录
    LoraNetWorkLogin = 13, 
    
  } Enum_GW3761_State   ;
 
  typedef struct          
  {
    uint8_t   Validity  ;  //合法性 true 合法数据帧  false  错误数据帧
    uint8_t   ID[2]     ;  //集中器id
    uint16_t  FrameNum  ;  //数据帧序号
    uint8_t   FrameTypes;  //数据帧类型
    uint8_t   Dri ;        //数据帧方向 
    uint8_t   Afn ;       
    uint8_t   Fn  ;
    uint8_t   Pn  ;     
    uint8_t   IP[6];
    uint8_t   DeviceType;//设备类型  
    uint8_t   DeviceID[6];//设备类型 
    uint8_t   SignalStrength;
    Str_Calendar  Rtc  ;//当前时间 
    uint8_t   PortNum  ;   //端口号 用于 AFN 10   6 好像是阀门  5好像是热表
    uint8_t   Buf[256] ;   //数据帧
    uint8_t   Len ;        //数据帧长度   
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
  
  
  
  //控制域_上行
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      uint8_t   FunctionCode:    4; //功能码
      bool      PWV:             1; //
      bool      ACD:             1;
      bool      PRM:             1;//PRM=1，表示此帧报文来自启动站；PRM=0，表示此帧报文来自从动站。
      bool      DIR:             1;//0 主站下行  1 终端上行 
    }Str_GW3761_Ctrl_Up;
  }Union_GW3761_Ctrl_Up;
  
  //控制域_下行
  typedef union 
  {
    uint8_t Byte;
    struct          
    {
      uint8_t   FunctionCode:   4;
      bool      FCV:            1;//FCV=1，表示FCB位有效；FCV=0，表示FCB位无效。
      bool      FCB:            1;
      bool      PRM:            1;
      bool      DIR :           1;
    }Str_GW3761_Ctrl_Down;
  }Union_GW3761_Ctrl_Down;
  
  /*本地通讯模块运行模式信息数据单元格式*/
  
  
  //帧序列域SEQ定义
  //帧序列域SEQ为1字节，用于描述帧之间的传输序
  //列的变化规则，由于受报文长度限制，数据无
  //法在一帧内传输，需要分成多帧传输
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
 
//信息域
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
