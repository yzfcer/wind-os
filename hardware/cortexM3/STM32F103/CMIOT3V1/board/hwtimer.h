#ifndef _HWTIMER_H_
#define _HWTIMER_H_


#include "stm32f10x.h"


void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void TIM3_PWM_Init(unsigned short arr, unsigned short psc);

void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void RTOS_TimerInit(void);


#endif
