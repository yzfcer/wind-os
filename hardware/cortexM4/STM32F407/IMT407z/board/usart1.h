#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

#define USART1_REC_NUM  			100  	//定义最大接收字节数 200
extern u8_t uart_byte_count;          //uart_byte_count要小于USART_REC_LEN
extern u8_t receive_str[USART1_REC_NUM];  

void uart1_init(u32_t baud);
void uart1_send_chars(u8_t *str, u16_t strlen);

#endif


