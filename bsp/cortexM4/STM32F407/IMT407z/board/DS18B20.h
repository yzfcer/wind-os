#ifndef __DS18B20_H
#define __DS18B20_H 
#include "common.h"   

//////////////////////////////////////////////////////////////////////////////////	 

//IO��������
#define DS18B20_IO_IN()  {GPIOB->MODER&=0XCFFFFFFF;GPIOB->MODER|=0;}	//PB15����ģʽ
#define DS18B20_IO_OUT() {GPIOB->MODER&=0XCFFFFFFF;GPIOB->MODER|=0x40000000;} 	//PB15���ģʽ
 
////IO��������											   
#define	DS18B20_DQ_OUT PBout(15) //���ݶ˿�	PB15
#define	DS18B20_DQ_IN  PBin(15)  //���ݶ˿�	PB15
   	
u8_t DS18B20_Init(void);			   //��ʼ��DS18B20
short DS18B20_Get_Temp(void);	 //��ȡ�¶�
void DS18B20_Start(void);		   //��ʼ�¶�ת��
void DS18B20_Write_Byte(u8_t dat);//д��һ���ֽ�
u8_t DS18B20_Read_Byte(void);		  //����һ���ֽ�
u8_t DS18B20_Read_Bit(void);	  	//����һ��λ
u8_t DS18B20_Check(void);			    //����Ƿ����DS18B20
void DS18B20_Rst(void);			    //��λDS18B20    
#endif















