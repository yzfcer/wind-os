/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	hwtimer.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		单片机定时器初始化
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//OS头文件
//#include "includes.h"

//硬件驱动
#include "wind_config.h"
#include "hwtimer.h"

#define OS_TICKS_PER_SEC WIND_TICK_PER_SEC

/*
************************************************************
*	函数名称：	Timer1_8_Init
*
*	函数功能：	Timer1或8的PWM配置
*
*	入口参数：	TIMx：TIM1 或者 TIM8
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	
	GPIO_InitTypeDef gpioInitStruct;
	TIM_TimeBaseInitTypeDef timerInitStruct;
	TIM_OCInitTypeDef timerOCInitStruct;

	if(TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	}
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStruct);	
	
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;				//选择定时器模式:TIM脉冲宽度调制模式2
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	timerOCInitStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIMx, &timerOCInitStruct);
	TIM_OC3Init(TIMx, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE 主输出使能	
	
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPE使能
	
	TIM_Cmd(TIMx, ENABLE);										//使能TIMx

}

/*
************************************************************
*	函数名称：	TIM3_PWM_Init
*
*	函数功能：	Timer3_PWM配置
*
*	入口参数：	arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM3_PWM_Init(unsigned short arr, unsigned short psc)
{

	GPIO_InitTypeDef gpioInitStruct;
	TIM_TimeBaseInitTypeDef timerInitStruct;
	TIM_OCInitTypeDef timerOCInitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_6;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIM3, &timerInitStruct);
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;				//选择定时器模式:TIM脉冲宽度调制模式1
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	timerOCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM3, ENABLE);							//MOE 主输出使能	
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIM3, ENABLE);							//ARPE使能
	
	TIM_Cmd(TIM3, ENABLE);										//使能TIMx

}

/*
************************************************************
*	函数名称：	Timer6_7_Init
*
*	函数功能：	Timer6或7的定时配置
*
*	入口参数：	TIMx：TIM6 或者 TIM7
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer6和timer7只具有更新中断功能
************************************************************
*/
void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{

	TIM_TimeBaseInitTypeDef timerInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	if(TIMx == TIM6)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM6_IRQn;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM7_IRQn;
	}
	
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//使能更新中断
	
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvicInitStruct);
	
	TIM_Cmd(TIMx, ENABLE); //使能定时器

}

/*
************************************************************
*	函数名称：	RTOS_TimerInit
*
*	函数功能：	RTOS的心跳定时初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		APB1--36MHz，APB1总线分频不为1，则定时器时钟要乘以2
*				定时5ms
************************************************************
*/
void RTOS_TimerInit(void)
{

	Timer6_7_Init(TIM6, 10000 / OS_TICKS_PER_SEC, 7199);	//72MHz，7200分频-100us，50重载值。则中断周期为100us * 50 = 5ms

}

/*
************************************************************
*	函数名称：	TIM6_IRQHandler
*
*	函数功能：	RTOS的心跳定时中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
#if 0
void TIM6_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
	{
		//do something...
		OSIntEnter();
		OSTimeTick();
		OSIntExit();
		
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}

}
#endif
/*
************************************************************
*	函数名称：	TIM7_IRQHandler
*
*	函数功能：	Timer7更新中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM7_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}

}
