#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"
#include "common.h" 
#include "wind_config.h"
#if WIND_DEBUG_SUPPORT
void uart1_init(w_uint32_t baud);
void uart1_send_chars(w_uint8_t *str, w_uint16_t strlen);
#else 
#define uart1_init(b)
#endif
#endif


