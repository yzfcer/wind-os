#include "24c02.h"  				 

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: 24C02.c                                                              *
* �ļ�������24C02��������                                                        *
* �������ڣ�2015.03.09                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����IO��ģ��IICЭ����24c02��д����                                       * 
**********************************************************************************
*********************************************************************************/	

//IIC��ӦIO�ڵĳ�ʼ��
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;     //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 //��ʼ��IO
	IIC_SCL=1;
	IIC_SDAOUT=1;
}
/*******************************************************************************
*************************����ΪIO��ģ��IICͨ��**********************************
*******************************************************************************/
//IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //����SDA��Ϊ���
	IIC_SDAOUT=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDAOUT=0;
	delay_us(4);
	IIC_SCL=0;     //׼�����ͻ�������� 
}	  
//IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();    //����SDA��Ϊ���
	IIC_SCL=0;
	IIC_SDAOUT=0; 
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDAOUT=1; //����I2C���߽����ź�
	delay_us(4);							   	
}
/****************************************************************************
* ��    ��: w_uint8_t MCU_Wait_Ack(void)
* ��    �ܣ�MCU�ȴ����豸Ӧ���źŵ���
* ��ڲ�������
* ���ز�����1:����Ӧ��ʧ��  0:����Ӧ��ɹ�
* ˵    ����  �B
****************************************************************************/
w_uint8_t MCU_Wait_Ack(void)
{
	w_uint8_t WaitTime=0;
	SDA_IN();      //����SDA��Ϊ����  
	IIC_SDAOUT=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(IIC_SDAIN)
	{
		WaitTime++;
		if(WaitTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0; 
	return 0;  
}
/****************************************************************************
* ��    ��: w_uint8_t void MCU_Send_Ack(void)
* ��    �ܣ�MCU����ACKӦ��,��֪24cxx
* ��ڲ�������
* ���ز�����
* ˵    ����  �B
****************************************************************************/
void MCU_Send_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
/****************************************************************************
* ��    ��: w_uint8_t void MCU_Send_Ack(void)
* ��    �ܣ�MCU������ACKӦ��	
* ��ڲ�������
* ���ز�����
* ˵    ����  �B
****************************************************************************/  
void MCU_NOAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}	
/****************************************************************************
* ��    ��: void IIC_write_OneByte(w_uint8_t Senddata)
* ��    �ܣ�IICдһ���ֽڵ�EEPROM	
* ��ڲ�����Senddata:д���8λ����
* ���ز�����
* ˵    ����  �B
****************************************************************************/	  
void IIC_write_OneByte(w_uint8_t Senddata)
{                        
    w_uint8_t t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;    //����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDAOUT=(Senddata&0x80)>>7;
        Senddata<<=1; 	  
		delay_us(2);   
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 
/****************************************************************************
* ��    ��: void IIC_Read_OneByte(w_uint8_t Senddata)
* ��    �ܣ�IIC��ȡһ���ֽ�
* ��ڲ�����ack=1������ACK��ack=0������nACK 
* ���ز�����������8λ����
* ˵    ����  �B
****************************************************************************/	  
w_uint8_t IIC_Read_OneByte(w_uint8_t ack)
{
	w_uint8_t i,receivedata=0;
	SDA_IN();       //����SDA��Ϊ����  
    for(i=0;i<8;i++ )
	  {
        IIC_SCL=0; 
        delay_us(2);
		    IIC_SCL=1;
        receivedata<<=1;
        if(IIC_SDAIN)receivedata++;   
		delay_us(1); 
    }					 
    if (!ack)
        MCU_NOAck();//����nACK
    else
        MCU_Send_Ack(); //����ACK   
    return receivedata;
}
/*******************************IO��ģ��IIC*************************************
*******************************************************************************/


/*******************************************************************************
*************************����ΪEEPROM24C02��д����******************************
*******************************************************************************/
//��ʼ��24c02��IIC�ӿ�
void AT24C02_Init(void)
{
	IIC_Init();  //IIC��ʼ��
}
/****************************************************************************
* ��    ��: w_uint8_t AT24C02_ReadByte(w_uint8_t ReadAddr)
* ��    �ܣ���AT24C02ָ����ַ����һ������
* ��ڲ�����ReadAddr��Ҫ��ȡ�������ڵĵ�ַ
* ���ز�����������8λ����
* ˵    ����  �B
****************************************************************************/
w_uint8_t AT24C02_ReadByte(w_uint8_t ReadAddr)
{				  
	w_uint8_t receivedata=0;		  	    																 
  
	IIC_Start();  
	IIC_write_OneByte(0XA0);           //����������ַ0XA0
	MCU_Wait_Ack();
  IIC_write_OneByte(ReadAddr);       //���͵͵�ַ
	MCU_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_write_OneByte(0XA1);           //�������ģʽ			   
	MCU_Wait_Ack();	 
  receivedata=IIC_Read_OneByte(0);		   
  IIC_Stop();                    //����һ��ֹͣ����	    
	
	return receivedata;
}
/****************************************************************************
* ��    ��: w_uint8_t AT24C02_WriteByte(w_uint8_t WriteAddr,w_uint8_t WriteData)
* ��    �ܣ���AT24C02ָ����ַд��һ������
* ��ڲ�����WriteAddr��Ҫд���������ڵĵ�ַ
            WriteData: Ҫд�������
* ���ز����� 
* ˵    ����  �B
****************************************************************************/
void AT24C02_WriteByte(w_uint8_t WriteAddr,w_uint8_t WriteData)
{				   	  	    																 
  IIC_Start();  
	IIC_write_OneByte(0XA0);       //����0XA0,д���� 	 
	MCU_Wait_Ack();	   
  IIC_write_OneByte(WriteAddr);  //���͵͵�ַ
	MCU_Wait_Ack(); 	 										  		   
	IIC_write_OneByte(WriteData);  //�����ֽ�							   
	MCU_Wait_Ack();  		    	   
  IIC_Stop();                    //����һ��ֹͣ���� 
	delay_ms(10);	 
}
/****************************************************************************
* ��    ��: w_uint8_t AT24C02_Test(void)
* ��    �ܣ�����AT24C02�Ƿ�����
* ��ڲ�������
* ���ز���������1:���ʧ��
            ����0:���ɹ� 
* ˵    ����  �B
****************************************************************************/
w_uint8_t AT24C02_Test(void)
{
	w_uint8_t Testdata;
	Testdata=AT24C02_ReadByte(255); //����������ԣ�����ֵ�����ٴ�д��	   
	if(Testdata==0XAB)return 0;		   
	else                             
	{
		AT24C02_WriteByte(255,0XAB);
	  Testdata=AT24C02_ReadByte(255);	  
		if(Testdata==0XAB)return 0;
	}
	return 1;											  
}
/****************************************************************************
* ��    ��: w_uint32_t Buf_4Byte(w_uint8_t *pBuffer,w_uint32_t Date_4Byte,w_uint8_t Byte_num,w_uint8_t mode)
* ��    �ܣ���λ�����ֽڻ�ת
* ��ڲ�����mode��1:��λ��ת�ֳ��ֽ�   0:�ֽںϲ���һ����λ��
            Byte_num����Ҫת�����ֽ���
            *pBuffer���ֽڽ���������ֽ���������
            Date_4Byte����λ����
* ���ز�����modeΪ0ʱ�����ض�λ��
* ˵    ����Byte_num���Ϊ4���ֽڣ��ú����ں���Ĵ�����У��ʱ��ȡУ��ֵ���õ�
****************************************************************************/
w_uint32_t Buf_4Byte(w_uint8_t *pBuffer,w_uint32_t Date_4Byte,w_uint8_t Byte_num,w_uint8_t mode)
{
   w_uint8_t i; w_uint32_t middata=0;
	if(mode)    //��λ��ת�ֳ��ֽ�
	 {
	   for(i=0;i<Byte_num;i++)
	     {
	       *pBuffer++ =(Date_4Byte>>(8*i))&0xff;
	     }
			return 0; 
	 } 
	 else       //�ֽںϲ���һ����λ��
	 {
	    Date_4Byte=0;
		  pBuffer+=(Byte_num-1);
		  for(i=0;i<Byte_num;i++)
	      { 		
		      middata<<=8;
		      middata+= *pBuffer--;			   
	      }
			return middata;	
	 }
}
/****************************************************************************
* ��    ��: void AT24C02_Read(w_uint8_t ReadAddr,w_uint8_t *pBuffer,w_uint8_t ReadNum)
* ��    �ܣ���AT24C02�����ָ����ַ��ʼ����ָ������������
* ��ڲ�����ReadAddr :��ʼ�����ĵ�ַ  0~255
            pBuffer  :���������׵�ַ
            ReadNum:Ҫ�������ݵĸ���
* ���ز�����
* ˵    ����  �B
****************************************************************************/
void AT24C02_Read(w_uint8_t ReadAddr,w_uint8_t *pBuffer,w_uint8_t ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 
/****************************************************************************
* ��    ��: void AT24C02_Write(w_uint8_t WriteAddr,w_uint8_t *pBuffer,w_uint8_t WriteNum)
* ��    �ܣ���AT24C02�����ָ����ַ��ʼд��ָ������������
* ��ڲ�����WriteAddr :��ʼд��ĵ�ַ  0~255
            pBuffer  :���������׵�ַ
            WriteNum:Ҫд�����ݵĸ���
* ���ز�����
* ˵    ����  �B
****************************************************************************/
void AT24C02_Write(w_uint8_t WriteAddr,w_uint8_t *pBuffer,w_uint8_t WriteNum)
{
	while(WriteNum--)
	{
		AT24C02_WriteByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








