/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	usart.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-06-25
	*
	*	版本： 		V1.2
	*
	*	说明： 		单片机串口外设初始化，格式化打印
	*
	*	修改记录：	V1.1：增加DMA发送功能
	*				V1.2：增加DMA接收功能、IDLE中断
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <stdarg.h>
#include <string.h>
#include "wind_type.h"


ALTER_INFO alterInfo;

/*
************************************************************
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA9		RX-PA10
*				发送：DMA1_Channel4
*				接收：DMA1_Channel5
*				未使用DMA_TC中断，发现在RTOS版本中有可能会关闭总中断而导致死循环
************************************************************
*/
void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART1, &usartInitStruct);
	
	USART_Cmd(USART1, ENABLE);														//使能串口
	
#if(USART_DMA_RX_EN == 0)
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//使能接收中断
#endif

	//USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);									//使能IDLE中断
	
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvicInitStruct);
}

/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA2		RX-PA3
*				发送：DMA1_Channel7
*				接收：DMA1_Channel6
*				未使用DMA_TC中断，发现在RTOS版本中有可能会关闭总中断而导致死循环
************************************************************
*/
void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usartInitStruct.USART_Parity = USART_Parity_No;									//无校验
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//使能串口

#if(USART_DMA_RX_EN == 0)
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断
#endif

	//USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);									//使能IDLE中断
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);

}

/*
************************************************************
*	函数名称：	USARTx_ResetMemoryBaseAddr
*
*	函数功能：	重设DMA内存地址并使能发送
*
*	入口参数：	USARTx：串口组
*				mAddr：内存地址值
*				num：本次发送的数据长度(字节)
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void USARTx_ResetMemoryBaseAddr(USART_TypeDef *USARTx, unsigned int mAddr, unsigned short num, _Bool type)
{

}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}


/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
extern void wind_stdin_irq(char data);
void USART1_IRQHandler(void)
{
	w_uint8_t rec_data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        rec_data =(w_uint8_t)USART_ReceiveData(USART1);
        wind_stdin_irq((char)rec_data);
    } 
}


void USART2_IRQHandler(void)
{
	w_uint8_t rec_data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        rec_data =(w_uint8_t)USART_ReceiveData(USART2);
        wind_stdin_irq((char)rec_data);
    } 
}
