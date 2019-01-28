#include "timer.h"
#include "led.h"
#include "beep.h"

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: timer.c                                                              *
* �ļ�������timer��ʱ                                                            *
* �������ڣ�2015.03.07                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ������ʱ��2��ʱ��LED1��ת�ͷ��������                                    * 
**********************************************************************************
*********************************************************************************/ 

/****************************************************************************
* ��    ��: TIM2_Init(u16_t auto_data,u16_t fractional)
* ��    �ܣ���ʱ��2��ʼ��
* ��ڲ�����auto_data: �Զ���װֵ
*           fractional: ʱ��Ԥ��Ƶ��
* ���ز�������
* ˵    ������ʱ�����ʱ����㷽��:Tout=((auto_data+1)*(fractional+1))/Ft(us)  Ft��ʱ��ʱ��
****************************************************************************/
void TIM2_Init(u16_t auto_data,u16_t fractional)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);      //ʹ��TIM2ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = auto_data; 	     //�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=fractional;      //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��2�����ж�
	TIM_Cmd(TIM2,ENABLE);                    //ʹ�ܶ�ʱ��2
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;  //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//��ʱ��3�жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
		LED1=!LED1;//LED1��ת
		BEEP=!BEEP;
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
}
