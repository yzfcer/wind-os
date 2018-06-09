/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	delay.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-01-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����Timer4������ʽ��ʱ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//delayͷ�ļ�
#include "delay.h"


/*
************************************************************
*	�������ƣ�	Delay_Init
*
*	�������ܣ�	systick��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Delay_Init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	//STM32F10x��Ƭ����ʱ����BUG����Ҫ������һ�Σ�Ȼ�������־�����������BUG
	TIM4->CR1 = 1;
	while(!(TIM4->SR & 1));							//�ȴ������ж��¼�
	TIM4->SR &= ~(1UL << 0);						//�����־
	TIM4->CR1 = 0;
	
	TIM4->CR1 |= 1UL << 3;							//������ģʽ
	
	TIM4->EGR |= 1;									//��������
	
	TIM4->DIER = 0;									//��ֹ�����ж�
	
	TIM4->CR1 &= (unsigned short)~TIM_CR1_CEN;		//ֹͣ��ʱ
	
	TIM4->PSC = 71;

}

/*
************************************************************
*	�������ƣ�	DelayUs
*
*	�������ܣ�	΢�뼶��ʱ
*
*	��ڲ�����	us����ʱ��ʱ��
*
*	���ز�����	��
*
*	˵����		��ʱ��(21MHz)�����ʱ798915us
************************************************************
*/
void DelayUs(unsigned short us)
{

	TIM4->ARR = us;
	
	TIM4->PSC = 71;									//timer4Ϊ72MHz������Ϊ71+1��Ƶ��1MHz��1us����һ��
	
	TIM4->CR1 |= (unsigned short)TIM_CR1_CEN;		//��ʼ��ʱ
	
	while(!(TIM4->SR & 1));							//�ȴ������ж��¼�
	
	TIM4->SR &= ~(1UL << 0);						//�����־

}

/*
************************************************************
*	�������ƣ�	DelayXms
*
*	�������ܣ�	���뼶��ʱ
*
*	��ڲ�����	ms����ʱ��ʱ��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void DelayXms(unsigned short ms)
{

	while(ms--)
		DelayUs(1000);

}