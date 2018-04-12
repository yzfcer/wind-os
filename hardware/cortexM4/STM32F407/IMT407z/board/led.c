#include "led.h" 

/*********************************************************************************
*************************MCU启明 STM32F407核心开发板******************************
**********************************************************************************
* 文件名称: led.c                                                                *
* 文件简述：LED初始化                                                            *
* 创建日期：2015.03.03                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：LED对应IO口初始化                                                    * 
**********************************************************************************
*********************************************************************************/

//LED对应IO初始化
void LED_Init(int mask)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟

    //PG13、PG14和PG15初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//LED0、LED1和LED2对应IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                  //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;             //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                   //上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);                         //初始化GPIO
    //GPIOG13,G14,G15设置高，灯灭
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





