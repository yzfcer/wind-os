#ifndef __SPI_H
#define __SPI_H
#include "common.h"

//////////////////////////////////////////////////////////////////////////////////	 
 	    													  
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI1_Setclock(w_uint8_t SPI_Prescaler); //����SPI1ʱ���ٶ�   
w_uint8_t SPI1_ReadWriteByte(w_uint8_t writeData);  //SPI1���߶�дһ���ֽ�
		 
#endif

