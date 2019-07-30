/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2015-08-02  Eric2013   首次发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include "stdbool.h"
#include "stdint.h"
#include "cJSON.h"
/*
*********************************************************************************************************
*                                         其它库
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>
#include  <app_cfg.h>


/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/

#include  <ucos_ii.h>

/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/

#pragma section = "_exram" 
#define exram @"_exram"

#define EXT_SRAM_ADDR  	((uint32_t)0x68000000)

#define Task_Suspend 0xffff
#define PrintfEnable
#define MeterLibMaxNum 464//表库最大数量
#define LoraSlotTimeBSNum 470// 广播时隙编号

/****************无线任务设置参数*********************/ 

#define SuspendTask                     0xffff
#define Lora_Max_Channels               8//对应下面上行信道 和下行信道频率
#define UpLink_Frequency_Start          470300000//10个到472.1
#define Comm_Frequency_Start            470500000//10个到472.1
#define DownLink_Frequency_Start        470700000//10个到474.1

#define FS_VOLUME_SD		"0:"
#define FS_VOLUME_FLASH		"1:"
#define FS_VOLUME_USB		"2:"


#define FLASH_APP1_ADDR	    0x08010800    //第一个应用程序起始地址(存放在FLASH)
#define FLASH_IAP_info      0x08010000    //IAP信息

#define BeaconSlotTickOffset    16 //注意 前导码12个  无线配置 4/8 125 11
#define RegisterSlotTickOffset  17 //注意 前导码12个  无线配置 4/8 125 11
#define BroadcastSlotTickOffset  42 //注意 前导码2秒  无线配置 4/8 125 11


typedef union 
{
uint32_t B32;
uint8_t B8[4];
}B32_B08;

typedef union 
{
uint16_t B16;
uint8_t  B8[2];
}B16_B08 ;

/*********************运行状态***************************/
  typedef enum 
  {
    Uart0RcvOK       = 1,//串口0接收完成
    Uart0SendOK      = 2,//串口0发送完成
    N720RcvOK        = 3,//串口0接收完成
    N720SendOK       = 4,//串口0发送完成
    LoraComming      = 5,//loraping正在通信
    LoraRcvOK        = 7,//串口0接收完成
    LoraSendOK       = 8,//串口0发送完成
    N720Register     = 9,//4G网络注册成功
    TCPRegister      = 10,//TCP配置成功
  } Enum_State;
/*********************运行状态***************************/

typedef struct 
  {
    uint8_t len;
    uint8_t rssi;
    uint8_t buf[128];
  }Str_Lora;

extern OS_EVENT *SempN720Rx;
extern OS_EVENT *SempDEBUGRx;
extern OS_EVENT *SempLoraRx;
extern OS_EVENT *SempLoraTx;

extern OS_EVENT *SempTCPRx;
extern OS_EVENT *SempSMSRx;

extern OS_EVENT *MboxLoraPing;
extern OS_EVENT *MboxGateWayTemp;

/*
*********************************************************************************************************
*                                        BSP-armfly
*********************************************************************************************************
*/
#include  <bsp.h>

/*
*********************************************************************************************************
*                                        BSP-fire
*********************************************************************************************************
*/
#include "bsp_sram_fire.h"
/*
*********************************************************************************************************
*                                        Middleware
*********************************************************************************************************
*/
#include "ZLG_Queue.h"
#include "State.h"
#include "Math_Lib.h"
#include "GateWayTaskQueue.h"

/*
*********************************************************************************************************
*                                        BSP-J
*********************************************************************************************************
*/
#include "bsp_IAP.h"
#include "bsp_N720_J.h"
#include "Sx1278_Spi.h"
#include "bsp_ZLG_IIC_J.h"
#include "bsp_Rs8025t_J.h"
#include "bsp_temperature_J.h"
/*
*********************************************************************************************************
*                                        FATFS
*********************************************************************************************************
*/

#include "ff.h"
#include "diskio.h"

/*
*********************************************************************************************************
*                                         APP
*********************************************************************************************************
*/

#include "MeterLib.h"
#include "LoraNetTask.h"
#include "GateWayVariable.h"
#include "GateWayTemp.h"
#include "N720task.h"
#include "N720MessageQueues.h"
#include "Sx1278_LoRaMisc.h"
#include "Sx1278_LoRa.h"
#include "GateWayInit.h"
#include "GateWayReset.h"
#include "LoRaProtocol.h"
#include "GW3761_2013.h"
#include "Tcp4GComm.h"



#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
