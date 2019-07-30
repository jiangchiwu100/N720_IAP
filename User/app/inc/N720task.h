#ifndef __N720task_H
#define __N720task_H

#ifdef __cplusplus
extern "C" {
#endif 

#define Init_Success        0xfff1
#define Init_Fail           0xfff2
#define Ftp_Success         0xfff3
#define Ftp_Fail            0xfff4
#define TCP_Success         0xfff5
#define TCP_Fail            0xfff6
#define SMS_Success         0xfffA
#define SMS_Fail            0xfffB
#define Msg_Finish          0xfffC
#define Msg_Fail            0xfffD



#define SMS_Msg         1
#define TCP_Msg         2
  
typedef struct 
{
    uint8_t len;
    uint8_t buf[256];
}Str_TCP;



/**********************SMS信息处理相关状态************************************/
typedef enum 
{
    SMS_Info_Set             = 1,//配置信息
    SMS_Software_Reset       = 2,//清除表库
    SMS_Hardware_Reset       = 3,//硬件复位
    SMS_IAP_Ctrl             = 4,//进行IAP
    Ctrl_Error               = 5,//操作码错误
    PWD_Error                = 6,//短信密码错误
}Enum_SMS_Ctrl;
/****************************************************************************/

/************************Init相关状态***************************************/

/************************最顶层状态机**************************************/
typedef enum 
{ 
    Enum_N720_Check_AT          = 0,//检查AT通讯
    Enum_N720_Check_Internet    = 1,//检查网络连接
    Enum_N720_Init_State        = 2,//初始化N720
    Enum_N720_Init_SMS          = 3,//初始化SMS配置
    Enum_N720_Init_TCP          = 4,//初始化TCP配置
    Enum_N720_JudgeAndGet       = 5,//等待接收信息
}Enum_N720_Init;


/************************次一层状态机**************************************/

typedef enum 
{ 
    Enum_N720_PowerOn_Disable   = 1,//拉低poweron引脚 开机成功 然后延时10s 
    Enum_N720_Check_Report      = 2,//发送“AT”检查串口是否已通
    Enum_N720_FCDisplay         = 3,//关闭回显
    Enum_N720_Check_SIMCard     = 4,//检查SIM卡状态   
    Enum_N720_Query_Net_Reg     = 5,//查询网络注册情况
    Enum_N720_Query_Net_Attach  = 6,//查询网络附着情况
    Enum_N720_Enable_Reporting  = 7,//开启主动上报
    Enum_N720_Set_APN           = 8,//设置数据业务号码(APN参数) 
    Enum_N720_Set_USERPWD       = 9,//设置用户名和密码
    Enum_N720_Net_Connection    = 10,//激活网络连接
    Enum_N720_Check_Connection  = 11,//查询网络连接情况
    Enum_N720_Init_Fail         = 12,//初始化失败    
}Enum_N720_State;

typedef enum 
{
    Enum_N720_Set_SMS_Mode      = 1,//设置短信模式
    Enum_N720_Set_Char          = 2,//设置字符集
    Enum_N720_Reporting_Mode    = 3,//设置短信到来主动上报模式
    Enum_N720_SMS_Fail          = 4,//SMS初始化失败
}Enum_N720_SMS;

typedef enum 
{ 
    Enum_N720_Close_TCP         = 1,//关闭TCP连接
    Enum_N720_Set_TCP           = 2,//设置TCP连接参数
    Enum_N720_Open_TCP          = 3,//打开TCP连接
    Enum_N720_TCP_Fail          = 4,//TCP初始化失败
}Enum_N720_TCP;

typedef enum
{
    Null_Type                   = 0,//无消息
    SMS_Type                    = 1,//SMS信息
    TCP_Type                    = 2,//TCP信息
    Get_SMS_Success             = 3,//获取短信成功
    SMS_Error                   = 4,//错误短信
    Get_SMS_Fail                = 5,//获取短信失败
    Get_TCP_Success             = 6,//获取TCP通讯内容成功
    TCP_Error                   = 7,//错误TCP消息
    Get_TCP_Fail                = 8,//获取TCP通讯内容失败
}Enum_Message_JudgeAndGet;

/**************************************************************************/

extern OS_STK AppTaskN720InitStk[];
void AppTaskN720Init(void *p_arg);
uint32_t N720_Init(void);

uint32_t N720_State_Init(void);
uint32_t N720_SMS_Init(void);
uint32_t N720_TCP_Init(void);
uint32_t N720_JudgeAndGet(void);

extern OS_STK AppTaskSMSCommStk[];
void AppTaskSMSComm(void *p_arg);

void SMS_Comm(void);

#ifdef __cplusplus
}
#endif
#endif