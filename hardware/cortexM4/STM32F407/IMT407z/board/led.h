#ifndef __LED_H
#define __LED_H
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////	

//LED�˿ڶ���
#define LED0 PGout(13)	 
#define LED1 PGout(14)	 
#define LED2 PGout(15)	  

//��������
void LED_Init(int mask);//��ʼ��	
void LED_On(unsigned int ledidx);
void LED_Off(unsigned int ledidx);
#endif
