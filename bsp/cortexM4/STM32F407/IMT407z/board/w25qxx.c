#include "spi.h"
#include "w25qxx.h"  

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: w25qxx.c                                                             *
* �ļ�������W25Q128ʹ��                                                          *
* �������ڣ�2015.03.10                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����25Q128����Ϊ16M,����128��Block,4096��Sector                          *
255byteΪһҳ                                                        *
4KbytesΪһ������                                                    *
16������Ϊһ����                                                     *
*********************************************************************************/	

w_uint16_t W25QXX_ID;	 

//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); //ʹ��GPIOGʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;             //PG8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         //���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);                //��ʼ��

    W25QXX_CS=1;			                                    //SPI FLASH��ѡ��
    SPI1_Init();		   			                              //��ʼ��SPI
    SPI1_Setclock(SPI_BaudRatePrescaler_2);		            //����Ϊ42Mʱ��,����ģʽ 
    W25QXX_ID=W25QXX_ReadID();	                          //��ȡFLASH ID.
}  
/****************************************************************************
* ��    ��: w_uint8_t W25QXX_ReadSR(void) 
* ��    �ܣ���ȡW25QXX��״̬�Ĵ���
* ��ڲ�������
* ���ز�����״̬�Ĵ�����ֵ
* ˵    ���� 		     
****************************************************************************/
w_uint8_t W25QXX_ReadSR(void)   
{  
    w_uint8_t byte=0;   
    W25QXX_CS=0;                                  //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_ReadStatusReg);   //���Ͷ�ȡ״̬�Ĵ�������    
    byte=SPI1_ReadWriteByte(0Xff);                //��ȡһ���ֽ�  
    W25QXX_CS=1;                                  //ȡ��Ƭѡ     
    return byte;   
} 
/****************************************************************************
* ��    ��: void W25QXX_Write_SR(w_uint8_t sr)
* ��    �ܣ�дW25QXX״̬�Ĵ���
* ��ڲ�����д���ֵ
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void W25QXX_Write_SR(w_uint8_t sr)   
{   
    W25QXX_CS=0;                                 //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_WriteStatusReg); //����дȡ״̬�Ĵ�������    
    SPI1_ReadWriteByte(sr);                      //д��һ���ֽ�  
    W25QXX_CS=1;                                 //ȡ��Ƭѡ     	      
} 

//W25QXXдʹ��	 
void W25QXX_Write_Enable(void)   
{
    W25QXX_CS=0;                               //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_WriteEnable);  //����дʹ��  
    W25QXX_CS=1;                               //ȡ��Ƭѡ     	      
} 
//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
    while((W25QXX_ReadSR()&0x01)==0x01);      //�ȴ�BUSYλ���
} 
//W25QXXд��ֹ	 
void W25QXX_Write_Disable(void)   
{  
    W25QXX_CS=0;                                //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_WriteDisable);  //����д��ָֹ��    
    W25QXX_CS=1;                                //ȡ��Ƭѡ     	      
} 
/****************************************************************************
* ��    ��: w_uint16_t W25QXX_ReadID(void)
* ��    �ܣ���ȡоƬID
* ��ڲ�������
* ���ز�����оƬID
* ˵    ����0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
0XEF17,��ʾоƬ�ͺ�ΪW25Q128      
****************************************************************************/  
w_uint16_t W25QXX_ReadID(void)
{
    w_uint16_t IDnum = 0;	  
    W25QXX_CS=0;				    
    SPI1_ReadWriteByte(0x90); //���Ͷ�ȡID����	    
    SPI1_ReadWriteByte(0x00); 	    
    SPI1_ReadWriteByte(0x00); 	    
    SPI1_ReadWriteByte(0x00); 	 			   
    IDnum|=SPI1_ReadWriteByte(0xFF)<<8;  
    IDnum|=SPI1_ReadWriteByte(0xFF);	 
    W25QXX_CS=1;				    
    return IDnum;
} 
/****************************************************************************
* ��    ��: void W25QXX_Erase_Chip(void) 
* ��    �ܣ���������оƬ		  
* ��ڲ�������
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                   
    W25QXX_Wait_Busy();   
    W25QXX_CS=0;                             //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_ChipErase);  //����Ƭ��������  
    W25QXX_CS=1;                             //ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				           //�ȴ�оƬ��������
}  
/****************************************************************************
* ��    ��: void W25QXX_Erase_Sector(w_uint32_t First_Addr)
* ��    �ܣ�����ĳ���������׵�ַ	  
* ��ڲ�����First_Addr:������ַ
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void W25QXX_Erase_Sector(w_uint32_t First_Addr)   
{    
    First_Addr*=4096;
    W25QXX_Write_Enable();                  
    W25QXX_Wait_Busy();   
    W25QXX_CS=0;                                  //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_SectorErase);     //������������ָ�� 
    SPI1_ReadWriteByte((w_uint8_t)((First_Addr)>>16));   //���͵�ַ    
    SPI1_ReadWriteByte((w_uint8_t)((First_Addr)>>8));   
    SPI1_ReadWriteByte((w_uint8_t)First_Addr);  
    W25QXX_CS=1;                                  //ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				                //�ȴ��������
}  
//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
    W25QXX_CS=0;                             //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_PowerDown);  //���͵�������  
    W25QXX_CS=1;                             //ȡ��Ƭѡ     	      
    delay_us(3);                             
}   
//����
void W25QXX_WAKEUP(void)   
{  
    W25QXX_CS=0;                                      //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_ReleasePowerDown);    //���ͻ�������
    W25QXX_CS=1;                                      //ȡ��Ƭѡ     	      
    delay_us(3);                                     
}   

/****************************************************************************
* ��    ��: void W25QXX_Read(w_uint8_t* pBuffer,w_uint32_t ReadAddr,w_uint16_t NumByteToRead)
* ��    �ܣ�ָ����ַ��ʼ��ȡָ�����ȵ�����
* ��ڲ�����DataBuffer:���ݴ洢��
StartAddress:��ʼ��ȡ�ĵ�ַ(24bit)
ByteCount:Ҫ��ȡ���ֽ���(���65535)
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void W25QXX_Read(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)   
{  										    
    W25QXX_CS=0;                                 //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_ReadData);     //���Ͷ�ȡ����   
    SPI1_ReadWriteByte((w_uint8_t)((StartAddress)>>16));  //����24bit��ַ    
    SPI1_ReadWriteByte((w_uint8_t)((StartAddress)>>8));   
    SPI1_ReadWriteByte((w_uint8_t)StartAddress);   

    while (ByteCount--) 
    {
        *DataBuffer = SPI1_ReadWriteByte(0XFF);
        DataBuffer++;
    }		
    W25QXX_CS=1;  				    	      
}  
/****************************************************************************
* ��    ��: void W25QXX_Write_Page(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)
* ��    �ܣ���һҳ��д������256���ֽڵ�����  
* ��ڲ�����DataBuffer:���ݴ洢��
StartAddress:��ʼд��ĵ�ַ(24bit)
ByteCount:Ҫд����ֽ���(���256)
* ���ز�������
* ˵    ���� 		     
****************************************************************************/
void W25QXX_Write_Page(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)
{  
    W25QXX_Write_Enable();                   
    W25QXX_CS=0;                                   //ʹ������   
    SPI1_ReadWriteByte(W25X_CMD_PageProgram);      //����дҳ����   
    SPI1_ReadWriteByte((w_uint8_t)((StartAddress)>>16));  //���͵�ַ    
    SPI1_ReadWriteByte((w_uint8_t)((StartAddress)>>8));   
    SPI1_ReadWriteByte((w_uint8_t)StartAddress);   
    while (ByteCount--)
    {
        SPI1_ReadWriteByte(*DataBuffer);
        DataBuffer++;
    }	
    W25QXX_CS=1;                   //ȡ��Ƭѡ 
    W25QXX_Wait_Busy();					   //�ȴ�д�����
}
/****************************************************************************
* ��    ��: void W25QXX_PageWrite(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount) 
* ��    �ܣ���ҳд������
* ��ڲ�����DataBuffer:���ݴ洢��
StartAddress:��ʼд��ĵ�ַ(24bit)
ByteCount:Ҫд����ֽ���(���256)
* ���ز�������
* ˵    �������Զ���ҳ���ܣ���Ϊ�ú���û����д֮ǰ�ж�Ҫд��ĵ�ַ�ϵ������Ƿ�Ϊ
�գ�������д��֮ǰ����ȷ���õ�ַ�ϵ�����Ϊ��0xFF������д��ʧ�� 		     
****************************************************************************/
void W25QXX_PageWrite(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)   
{
    w_uint8_t NumOfPage = 0, NumOfSingle = 0, Surplus_Addr = 0, Surplus_count = 0, midtemp = 0;

    Surplus_Addr = StartAddress % 256;
    Surplus_count = 256 - Surplus_Addr;
    NumOfPage =  ByteCount / 256;
    NumOfSingle = ByteCount % 256;

    if (Surplus_Addr == 0) //��ʼд�ĵ�ַ�պ���ҳ��ʼ�ĵ�ַ
    {
        if (NumOfPage == 0)  // ByteCount < 256��һҳ�ܵ��ֽ�����  
        {
            W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
        }
        else                 // ByteCount > 256��һҳ�ܵ��ֽ�����  
        {
        while (NumOfPage--)
        {
            W25QXX_Write_Page(DataBuffer, StartAddress, 256);
            StartAddress +=  256;
            DataBuffer += 256;
        }
        W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
        }
    }
    else ///��ʼд�ĵ�ַ����ҳ���׵�ַ��
    {
        if (NumOfPage == 0) // ByteCount < 256��һҳ�ܵ��ֽ����� 
        {
            if (NumOfSingle > Surplus_count)  
            {
                midtemp = NumOfSingle - Surplus_count;
                W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
                StartAddress +=  Surplus_count;
                DataBuffer += Surplus_count;
                W25QXX_Write_Page(DataBuffer, StartAddress, midtemp);
            }
            else
            {
                W25QXX_Write_Page(DataBuffer, StartAddress, ByteCount);
            }
        }
        else //ByteCount > 256��һҳ�ܵ��ֽ�����  
        {
            ByteCount -= Surplus_count;
            NumOfPage =  ByteCount / 256;
            NumOfSingle = ByteCount % 256;

            W25QXX_Write_Page(DataBuffer, StartAddress, Surplus_count);
            StartAddress +=  Surplus_count;
            DataBuffer += Surplus_count;
            while (NumOfPage--)
            {
                W25QXX_Write_Page(DataBuffer, StartAddress, 256);
                StartAddress +=  256;
                DataBuffer += 256;
            }
            if (NumOfSingle != 0)
            {
                W25QXX_Write_Page(DataBuffer, StartAddress, NumOfSingle);
            }
        }
    }
} 
/****************************************************************************
* ��    ��: void W25QXX_SectorWrite(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)
* ��    �ܣ�������д������
* ��ڲ�����DataBuffer:���ݴ洢��
StartAddress:��ʼд��ĵ�ַ(24bit)
ByteCount:Ҫд����ֽ���(���65536)
* ���ز�������
* ˵    ����д��֮ǰ�жϸĵ�ַ�ϵ������Ƿ�Ϊ�գ������Ȳ�����д�룬�ú����ο����ϴ���http://www.openedv.com      
****************************************************************************/
w_uint8_t TS_BUFFER[4096];	
void W25QXX_SectorWrite(w_uint8_t* DataBuffer,w_uint32_t StartAddress,w_uint16_t ByteCount)   
{ 
    w_uint32_t secaddr;
    w_uint16_t secused;
    w_uint16_t Surplus_count;	   
    w_uint16_t i=0; 

    w_uint8_t * Cache_BUF;	   
    Cache_BUF=TS_BUFFER;	  

    secaddr=StartAddress/4096;//������ַ  
    secused=StartAddress%4096;//д��ĵ�ַ�������ڵ�ƫ�ƣ��Ӹ��������׵�ַ��ʼ��ʹ�õ��ֽ�����
    Surplus_count=4096-secused;//������ʣ��ռ��С   

    if(ByteCount<=Surplus_count)
        Surplus_count=ByteCount;//������4096���ֽ�
    while(1) 
    {	
        W25QXX_Read(Cache_BUF,secaddr*4096,4096);//������������������
        i=0;
        while(Cache_BUF[secused+i]==0XFF)
        {
            i++;
            if(i==Surplus_count)break;
        }
        if(i<Surplus_count)//��Ҫ����
        {
            W25QXX_Erase_Sector(secaddr);//�����������
            for(i=0;i<Surplus_count;i++)	   //����
            {
                Cache_BUF[i+secused]=DataBuffer[i];	  
            }
            W25QXX_PageWrite(Cache_BUF,secaddr*4096,4096);//д����������  
        }
        else
        W25QXX_PageWrite(DataBuffer,StartAddress,Surplus_count);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
        if(ByteCount==Surplus_count)
            break;//д�������
        else//д��δ����
        {
            secaddr++;//������ַ��1
            secused=0;//ƫ��λ��Ϊ0 	 

            DataBuffer+=Surplus_count;  //ָ��ƫ��
            StartAddress+=Surplus_count;//д��ַƫ��	   
            ByteCount-=Surplus_count;				//�ֽ����ݼ�
            if(ByteCount>4096)
                Surplus_count=4096; //��һ����������д����
            else 
                Surplus_count=ByteCount;            //��һ����������д����
        }	 
    };
}

