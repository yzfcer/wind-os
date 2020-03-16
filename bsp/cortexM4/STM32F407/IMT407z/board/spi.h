#ifndef __SPI_H
#define __SPI_H
#include "common.h"

//////////////////////////////////////////////////////////////////////////////////	 
 	    													  
void SPI1_Init(void);			 //初始化SPI1口
void SPI1_Setclock(w_uint8_t SPI_Prescaler); //设置SPI1时钟速度   
w_uint8_t SPI1_ReadWriteByte(w_uint8_t writeData);  //SPI1总线读写一个字节
		 
#endif

