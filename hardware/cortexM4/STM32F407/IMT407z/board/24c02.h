#ifndef __24C02_H
#define __24C02_H
#include "common.h"  
#include "wind_type.h"
////////////////////////////////////////////////////////////////////////////////// 	

//IIC_SDA��IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=0<<18;}	//PB9����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=1<<18;} //PB9���ģʽ
//IO��������	 
#define IIC_SCL      PBout(8) //SCL
#define IIC_SDAOUT   PBout(9) //���SDA	 
#define IIC_SDAIN    PBin(9)  //����SDA 

//IIC��غ���
void IIC_Init(void);          //��ʼ��IIC��IO��				 
void IIC_Start(void);				  //����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
w_uint8_t MCU_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MCU_Send_Ack(void);					  //IIC����ACK�ź�
void MCU_NOAck(void);				  //IIC������ACK�ź�
void IIC_write_OneByte(w_uint8_t Senddata);
w_uint8_t IIC_Read_OneByte(w_uint8_t ack);
	
//EEPROM24c02��غ���
w_uint8_t AT24C02_ReadByte(w_uint8_t ReadAddr);							     //ָ����ַ��ȡһ���ֽ�
void AT24C02_WriteByte(w_uint8_t WriteAddr,w_uint8_t DataToWrite);		//ָ����ַд��һ���ֽ�

w_uint32_t Buf_4Byte(w_uint8_t *pBuffer,w_uint32_t Date_4Byte,w_uint8_t Byte_num,w_uint8_t mode);

void AT24C02_Write(w_uint8_t WriteAddr,w_uint8_t *pBuffer,w_uint8_t WriteNum);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24C02_Read(w_uint8_t ReadAddr,w_uint8_t *pBuffer,w_uint8_t ReadNum);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

w_uint8_t AT24C02_Test(void);  //�������
void AT24C02_Init(void); //��ʼ��IIC
#endif
















