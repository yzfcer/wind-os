#ifndef __LED_H
#define __LED_H
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////	

//LED端口定义
#define LED0 PGout(13)	 
#define LED1 PGout(14)	 
#define LED2 PGout(15)	  

//函数声明
void LED_Init(void);//初始化	

#endif
