#ifndef __bsp_Rs8025t_J_H
#define __bsp_Rs8025t_J_H

#ifdef __cplusplus
extern "C" {
#endif 


  
#define RCC_ALL_KEY 	(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG)

#define GPIO_PORT_K1    GPIOC
#define GPIO_PIN_K1	GPIO_Pin_13
  
#define R8025T_ADDR             0x64  
//8025 内部寄存器地址定义
#define cR8025TSeconds          0x00
#define cR8025TMinutes          0x01
#define cR8025THours            0x02
#define cR8025TWeekDays         0x03
#define cR8025TDays             0x04
#define cR8025TMonths           0x05
#define cR8025TYears            0x06
#define cR8025TRAM              0x07
#define cR8025TAlarmMin         0x08
#define cR8025TAlarmHour        0x09
#define cR8025TAlarmWeekDay     0x0A
#define cR8025TTimeCounter0     0x0B
#define cR8025TTimeCounter1     0x0C
#define cR8025TExtension        0x0D
#define cR8025TFlag             0x0E
#define cR8025TControl          0x0F

  typedef union 
  {
    uint8_t Value;
    struct          
    {
      bool RESET:       1;
      uint8_t Space:    2;
      bool AIE:         1;
      bool TIE:         1;
      bool UIE:         1;
      uint8_t CSEL:     2;
    }Str_RS8025TControl;
  }Union_RS8025TControl;

  typedef union 
  {
    uint8_t Value;
    struct          
    {
      bool VDET:        1;
      bool VLF:         1;
      bool SpaceBit2:   1;
      bool AF:          1;
      bool TF:          1;
      bool UF:          1;
      bool SpaceBit6:   1;
      bool SpaceBit7:   1;
    }Str_RS8025TFlag;
  }Union_RS8025TFlag;
  
  typedef union 
  {
    uint8_t Value;
    struct          
    {
      uint8_t TSEL:     2;
      uint8_t FSEL:     2;
      bool TE:          1;
      bool USEL:        1;
      bool WADA:          1;
      bool TEST:          1;
    }Str_RS8025TExtension;
  }Union_RS8025TExtension;

  typedef struct Calendar
  {
    //! Seconds of minute between 0-59
    uint8_t Seconds;
    //! Minutes of hour between 0-59
    uint8_t Minutes;
    //! Hour of day between 0-23
    uint8_t Hours;
    //! Day of week between 0-6
    uint8_t DayOfWeek;
    //! Day of month between 1-31
    uint8_t DayOfMonth;
    //! Month between 0-11
    uint8_t Month;
    //! Year between 0-4095
    uint8_t Year;
  } Str_Calendar;
  
  
  

  
  
void    bsp_InitRS8025T_J(void);
uint8_t ReadYear(void);
uint8_t ReadMonth(void);
uint8_t ReadDay(void);
uint8_t ReadWeek(void);
uint8_t ReadHour(void);
uint8_t ReadMinute(void);
uint8_t ReadSecond(void);
/*******************************************************************************
** Function name:           RS8025T_Init_Calendar_Time
** Descriptions:            写8025t日历时钟 
** input parameters:        返回8035t日历 
** output parameters:       void
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-7   
*******************************************************************************/
void RS8025T_Init_Calendar_Time(Str_Calendar *CalendarTime);
/*******************************************************************************
** Function name:           RS8025T_Get_Calendar_Time
** Descriptions:            读8025t日历时钟 
** input parameters:        void 
** output parameters:       返回8035t日历
** Returned value:          void 
** Created by:              程卫玺
** Created Date:            2016-6-7   
*******************************************************************************/
void RS8025T_Get_Calendar_Time( Str_Calendar *rtc );

  #ifdef __cplusplus
}
#endif
#endif
