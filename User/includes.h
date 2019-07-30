/*
*********************************************************************************************************
*
*	ģ������ : ͷ�ļ�����
*	�ļ����� : includes.h
*	��    �� : V1.0
*	˵    �� : ��ǰʹ��ͷ�ļ�����
*
*	�޸ļ�¼ :
*		�汾��    ����        ����     ˵��
*		V1.0    2015-08-02  Eric2013   �״η���
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/*
*********************************************************************************************************
*                                         ��׼��
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
*                                         ������
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
*                                           �궨��
*********************************************************************************************************
*/

#pragma section = "_exram" 
#define exram @"_exram"

#define EXT_SRAM_ADDR  	((uint32_t)0x68000000)

#define Task_Suspend 0xffff
#define PrintfEnable
#define MeterLibMaxNum 464//����������
#define LoraSlotTimeBSNum 470// �㲥ʱ϶���

/****************�����������ò���*********************/ 

#define SuspendTask                     0xffff
#define Lora_Max_Channels               8//��Ӧ���������ŵ� �������ŵ�Ƶ��
#define UpLink_Frequency_Start          470300000//10����472.1
#define Comm_Frequency_Start            470500000//10����472.1
#define DownLink_Frequency_Start        470700000//10����474.1

#define FS_VOLUME_SD		"0:"
#define FS_VOLUME_FLASH		"1:"
#define FS_VOLUME_USB		"2:"


#define FLASH_APP1_ADDR	    0x08010800    //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
#define FLASH_IAP_info      0x08010000    //IAP��Ϣ

#define BeaconSlotTickOffset    16 //ע�� ǰ����12��  �������� 4/8 125 11
#define RegisterSlotTickOffset  17 //ע�� ǰ����12��  �������� 4/8 125 11
#define BroadcastSlotTickOffset  42 //ע�� ǰ����2��  �������� 4/8 125 11


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

/*********************����״̬***************************/
  typedef enum 
  {
    Uart0RcvOK       = 1,//����0�������
    Uart0SendOK      = 2,//����0�������
    N720RcvOK        = 3,//����0�������
    N720SendOK       = 4,//����0�������
    LoraComming      = 5,//loraping����ͨ��
    LoraRcvOK        = 7,//����0�������
    LoraSendOK       = 8,//����0�������
    N720Register     = 9,//4G����ע��ɹ�
    TCPRegister      = 10,//TCP���óɹ�
  } Enum_State;
/*********************����״̬***************************/

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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
