#ifndef __BSP_CLKCONFIG_FIRE_H
#define	__BSP_CLKCONFIG_FIRE_H


#include "stm32f10x.h"

void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);

#endif  
