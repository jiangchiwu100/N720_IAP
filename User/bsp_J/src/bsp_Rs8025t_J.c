#include "includes.h"






/**
* @brief  ��8025Tָ���Ĵ�����д��һ���ֽ�����
* @param  addr �Ĵ�����ַ
*         dat  Ҫд�������
* @retval None
*/

static void RX8025T_ByteWrite(uint8_t addr,uint8_t dat)
{		 
  ISendStr(R8025T_ADDR,addr,&dat,1);
}


/**
* @brief  ��ȡ8025Tָ���Ĵ�������ֵ
* @param  addr �Ĵ�����ַ
* @retval Returndat ������ֵ
*/
static uint8_t RX8025T_ByteRead(uint8_t addr)
{
  uint8_t Returndat;
  IRcvStr(R8025T_ADDR,addr,&Returndat,1);
  return (Returndat);
}

/**
* @brief  ��ʼ��8025T 
* @param  None
* @retval None
*/

void bsp_InitRS8025T_J(void)
{	
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  
  IIC_GPIO_Init();
  
  
  RX8025T_ByteWrite(cR8025TControl,0x10);       // 2S����һ�� ������ʱ����ж��¼� 
  RX8025T_ByteWrite(cR8025TExtension,0x10);     // bit4 TE �� BIT0,1 TSEL ���ڸ������ж�ѡ��ʱ��Դ 00 Ϊ4096 01 Ϊ64 
  RX8025T_ByteWrite(cR8025TTimeCounter0,0x00);  // ���1/16��֮һ���ж� ʱ��Դ4096 ����Ҫ��Ƶ256 
  RX8025T_ByteWrite(cR8025TTimeCounter1,0x01);  //  BIT0 Ϊ256��Ƶ 
  RX8025T_ByteWrite(cR8025TFlag,0x00);          // ������б�־

  
  /* ��1������GPIOʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  /* ʹ�� AFIO ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  /* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	/* ���븡��ģʽ */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   /* Connect EXTI0 Line to pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource15);
  
  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  	/* �����ش��� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




/**
* @brief  ����
* @param  None
* @retval None
*/
uint8_t ReadSecond(void)
{
  return (RX8025T_ByteRead(cR8025TSeconds) & 0x7F);	
}

/**
* @brief  ������
* @param  None
* @retval None
*/
uint8_t ReadMinute(void)
{
  return (RX8025T_ByteRead(cR8025TMinutes) & 0x7F);
}


/**
* @brief  ��Сʱ
* @param  None
* @retval None
*/
uint8_t ReadHour(void)
{	
  return (RX8025T_ByteRead(cR8025THours) & 0x3F);	
}

/**
* @brief  ������
* @param  None
* @retval None
*/
uint8_t ReadWeek(void)
{
  uint8_t TempChar=0;	
  
  TempChar = RX8025T_ByteRead(cR8025TWeekDays) & 0x7F ;
  switch(TempChar)
  {
  case 0x01:			   		/// bit0 :����
    TempChar = 0x00;
    break;
  case 0x02:				  	/// bit1 :��һ
    TempChar = 0x01;
    break;
  case 0x04:
    TempChar = 0x02;
    break;
  case 0x08:
    TempChar = 0x03;
    break;
  case 0x10:
    TempChar = 0x04;
    break;
  case 0x20:					
    TempChar = 0x05;
    break;
  default: 
    TempChar = 0x06;		  	/// bit6��o?������
    break;
  }
  return TempChar;
}


/**
* @brief  ������
* @param  None
* @retval None
*/
uint8_t ReadDay(void)
{
  return (RX8025T_ByteRead(cR8025TDays) & 0x3F);
}


/**
* @brief  ����
* @param  None
* @retval None
*/
uint8_t ReadMonth(void)
{
  return (RX8025T_ByteRead(cR8025TMonths) & 0x1F);	
}


/**
* @brief  ����
* @param  None
* @retval None
*/
uint8_t ReadYear(void)
{
  return (RX8025T_ByteRead(cR8025TYears));	
}

/*******************************************************************************
** Function name:           RS8025T_Init_Calendar_Time
** Descriptions:            д8025t����ʱ�� 
** input parameters:        ����8035t���� 
** output parameters:       void
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-7   
  Str_Calendar CalTem;
  CalTem.Year = 0x16;
  CalTem.Month= 0x06;
  CalTem.DayOfMonth=0x12;
  CalTem.DayOfWeek=1;
  CalTem.Hours=0x11;
  CalTem.Minutes=0x47;
  CalTem.Seconds=0x00;
  RS8025T_Init_Calendar_Time(&CalTem);
*******************************************************************************/
void RS8025T_Init_Calendar_Time(Str_Calendar *CalendarTime)
{
  switch(CalendarTime->DayOfWeek)
  {
  case 0x00:					  	/// bit0��o?����? 
    CalendarTime->DayOfWeek = 0x01;
    break;
  case 0x01:
    CalendarTime->DayOfWeek = 0x02;
    break;
  case 0x02:
    CalendarTime->DayOfWeek = 0x04;
    break;
  case 0x03:
    CalendarTime->DayOfWeek = 0x08;
    break;
  case 0x04:
    CalendarTime->DayOfWeek = 0x10;
    break;
  case 0x05:
    CalendarTime->DayOfWeek = 0x20;
    break;
  default: //0x06
    CalendarTime->DayOfWeek = 0x40;
    break;
  }
  RX8025T_ByteWrite(cR8025TSeconds,CalendarTime->Seconds);
  RX8025T_ByteWrite(cR8025TMinutes,CalendarTime->Minutes);
  RX8025T_ByteWrite(cR8025THours,CalendarTime->Hours);
  RX8025T_ByteWrite(cR8025TWeekDays,CalendarTime->DayOfWeek);
  RX8025T_ByteWrite(cR8025TDays,CalendarTime->DayOfMonth);
  RX8025T_ByteWrite(cR8025TMonths,CalendarTime->Month);
  RX8025T_ByteWrite(cR8025TYears,CalendarTime->Year);
}

/*******************************************************************************
** Function name:           RS8025T_Get_Calendar_Time
** Descriptions:            ��8025t����ʱ�� 
** input parameters:        void 
** output parameters:       ����8035t����
** Returned value:          void 
** Created by:              ������
** Created Date:            2016-6-7   
*******************************************************************************/
 
void RS8025T_Get_Calendar_Time( Str_Calendar *rtc )
{
  rtc->Seconds        = ReadSecond();
  rtc->Minutes        = ReadMinute();
  rtc->Hours          = ReadHour();
  rtc->DayOfWeek      = ReadWeek();
  rtc->DayOfMonth     = ReadDay();
  rtc->Month          = ReadMonth();
  rtc->Year           = ReadYear();
}