/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	adxl345.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		ADXL345初始化，读取三轴加速值
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "adxl345.h"
#include "i2c.h"
#include "delay.h"


ADXL345_INFO adxlInfo;


short ofz_X = 0, ofz_Y = 0, ofz_Z = 0;


/*
************************************************************
*	函数名称：	ADXL345_Init
*
*	函数功能：	ADXL345初始化配置
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void ADXL345_Init(void)
{
	
    unsigned char devid = 0, val = 0;

    DelayUs(300);

    I2C_ReadByte(ADXL345_ADDRESS, 0x00, &devid);			//读ID	且每次读写之前都需要读ID
	DelayUs(300);

	val = 0x2B;
    I2C_WriteByte(ADXL345_ADDRESS, DATA_FORMAT_REG, &val);	//低电平中断输出,13位全分辨率,输出数据右对齐,16g量程
	DelayUs(50);
    
	val = 0x0A;
    I2C_WriteByte(ADXL345_ADDRESS, BW_RATE, &val);			//数据输出速度为100Hz
	DelayUs(50);
    
	val = 0x28;
    I2C_WriteByte(ADXL345_ADDRESS, POWER_CTL, &val);		//链接使能,测量模式
	DelayUs(50);
    
	val = 0;
    I2C_WriteByte(ADXL345_ADDRESS, INT_ENABLE, &val);		//不使用中断
	DelayUs(50);
	
    DelayXms(20);
	
	//软件偏移校准，视上电时为平放状态，x=0，y=0，z=1=====================
	ADXL345_GetValue();
	
	ofz_X = 0 - adxlInfo.incidence_X;
	ofz_Y = 0 - adxlInfo.incidence_Y;
	ofz_Z = 256 - adxlInfo.incidence_Z;

}

/*
************************************************************
*	函数名称：	ADXL345_GetValue
*
*	函数功能：	读取ADXL345的三轴加速值
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		结果存放于adxl345结构体中
************************************************************
*/
void ADXL345_GetValue(void)
{

	unsigned char devid = 0;
	
	unsigned char dataTemp[6];
	
	IIC_SpeedCtl(5);													//控制IIC速度

    DelayUs(200);
    I2C_ReadByte(ADXL345_ADDRESS, 0x00, &devid);						//读ID	且每次读写之前都需要读ID
	DelayUs(200);
	
	I2C_ReadBytes(ADXL345_ADDRESS, 0x32, dataTemp, 6);					//读取原始加速值(4mg/LSB)
	
	adxlInfo.incidence_X = (short)(dataTemp[0] + ((unsigned short)dataTemp[1] << 8));
    adxlInfo.incidence_Y = (short)(dataTemp[2] + ((unsigned short)dataTemp[3] << 8));
    adxlInfo.incidence_Z = (short)(dataTemp[4] + ((unsigned short)dataTemp[5] << 8));
	
	adxlInfo.incidence_Xf = (float)(adxlInfo.incidence_X + ofz_X) * 0.0039;		//换算为g
	adxlInfo.incidence_Yf = (float)(adxlInfo.incidence_Y + ofz_Y) * 0.0039;		//每一个LSB代表3.9mg
	adxlInfo.incidence_Zf = (float)(adxlInfo.incidence_Z + ofz_Z) * 0.0039;		//有多少个LSB，就乘以0.0039g就得到了以g为单位的加速值

}
