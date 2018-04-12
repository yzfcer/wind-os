#include "led.h" 

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: led.c                                                                *
* �ļ�������LED��ʼ��                                                            *
* �������ڣ�2015.03.03                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����LED��ӦIO�ڳ�ʼ��                                                    * 
**********************************************************************************
*********************************************************************************/

//LED��ӦIO��ʼ��
void LED_Init(int mask)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��

    //PG13��PG14��PG15��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//LED0��LED1��LED2��ӦIO��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);                         //��ʼ��GPIO
    //GPIOG13,G14,G15���øߣ�����
    if(mask & 0x01)
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
    if(mask & 0x02)
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
    if(mask & 0x04)
        GPIO_SetBits(GPIOG, GPIO_Pin_15);
}

void LED_On(unsigned int ledidx)
{
    switch(ledidx)
    {
    case 0:LED0 = 0;break;
    case 1:LED1 = 0;break;
    case 2:LED2 = 0;break;
    default:break;
    }
}

void LED_Off(unsigned int ledidx)
{
    switch(ledidx)
    {
    case 0:LED0 = 1;break;
    case 1:LED1 = 1;break;
    case 2:LED2 = 1;break;
    default:break;
    }
}





