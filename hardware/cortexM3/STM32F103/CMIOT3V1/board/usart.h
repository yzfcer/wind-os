#ifndef _USART_H_
#define _USART_H_


#include "stm32f10x.h"


#define USART_DEBUG		USART1		//调试打印所使用的串口组


#define USART_DMA_TX_EN		1		//1-使能DMA发送		0-失能DMA发送

#define USART_DMA_RX_EN		1		//1-使能DMA接收		0-失能DMA接收


#define USART_TX_TYPE		0

#define USART_RX_TYPE		1


typedef struct
{

	char alterBuf[150];
	unsigned char alterCount;
	_Bool rev_idle;

} ALTER_INFO;

extern ALTER_INFO alterInfo;


void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

void USARTx_ResetMemoryBaseAddr(USART_TypeDef *USARTx, unsigned int mAddr, unsigned short num, _Bool type);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);


#endif
