/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	spilcd.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-04-15
	*
	*	版本： 		V1.0
	*
	*	说明： 		SPI TFT-LCD驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "spilcd.h"
#include "delay.h"
#include "hwtimer.h"
#if(HW_SPI_EN == 1)
#include "spi.h"
#endif

//C库
#include <stdarg.h>
#include <stdio.h>


SPILCD_INFO spilcd_info;


/*
************************************************************
*	函数名称：	LCD_SendByte
*
*	函数功能：	发送一个字节
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void LCD_SendByte(unsigned char byte)
{

#if(HW_SPI_EN == 0)
	unsigned char i = 0;
	
	for(; i < 8; i++)
	{
		if(byte & 0x80)
		{
			LCD_SDA_H;
		}
		else
		{
			LCD_SDA_L;
		}
		
		LCD_SCK_L;
		LCD_SCK_H;
		
		byte <<= 1;
	}
#else
	SPIx_WriteByte(SPILCD_IO, byte);
#endif

}

void LCD_WriteData(unsigned char data)
{

	LCD_CS_L;
	LCD_AO_H;
	LCD_SendByte(data);
	LCD_CS_H;

}

void LCD_WriteData16(unsigned short data)
{

	LCD_CS_L;
	LCD_AO_H;
	LCD_SendByte(data >> 8);
	LCD_SendByte(data & 0xff);
	LCD_CS_H;

}

void LCD_WriteReg(unsigned char reg)
{

	LCD_CS_L;
	LCD_AO_L;
	LCD_SendByte(reg);
	LCD_CS_H;

}

void LCD_WriteRegData(unsigned char reg, unsigned short data)
{

	LCD_WriteReg(reg);
	LCD_WriteData16(data);

}

void LCD_Rst(void)
{

	LCD_RST_ON;
	DelayXms(100);
	LCD_RST_OFF;
	DelayXms(50);

}

/*
************************************************************
*	函数名称：	LCD_Init
*
*	函数功能：	LCD初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		PA8-背光	PB12-CS		PB14-MISO	PB15-MOSI
*				PC9-RST		PB13-CLK	
************************************************************
*/
void SPILCD_Init(void)
{

	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
	//CS
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
#if(HW_SPI_EN == 1)
	SPIx_TX_Init(SPILCD_IO);
	
	gpioInitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14;
#else
	gpioInitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
#endif
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInitStruct);
	
	//复位
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &gpioInitStruct);
	
	//背光
	TIM3_PWM_Init(250, 1799);
	
	LCD_Rst();
	
	LCD_WriteReg(0x11);		//sleep out
	DelayXms(120);

	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_WriteReg(0xB1); 
	LCD_WriteData(0x05); 
	LCD_WriteData(0x3C); 
	LCD_WriteData(0x3C); 
	LCD_WriteReg(0xB2); 
	LCD_WriteData(0x05); 
	LCD_WriteData(0x3C); 
	LCD_WriteData(0x3C); 
	LCD_WriteReg(0xB3); 
	LCD_WriteData(0x05); 
	LCD_WriteData(0x3C); 
	LCD_WriteData(0x3C); 
	LCD_WriteData(0x05); 
	LCD_WriteData(0x3C); 
	LCD_WriteData(0x3C); 
	//------------------------------------End ST7735S Frame Rate-----------------------------------------// 
	LCD_WriteReg(0xB4); //Dot inversion 
	LCD_WriteData(0x03); 

	//------------------------------------ST7735S Power Sequence-----------------------------------------// 
	LCD_WriteReg(0xC0); 
	LCD_WriteData(0x28); 
	LCD_WriteData(0x08); 
	LCD_WriteData(0x04); 
	LCD_WriteReg(0xC1); 
	LCD_WriteData(0XC0); 
	LCD_WriteReg(0xC2); 
	LCD_WriteData(0x0D); 
	LCD_WriteData(0x00); 
	LCD_WriteReg(0xC3); 
	LCD_WriteData(0x8D); 
	LCD_WriteData(0x2A); 
	LCD_WriteReg(0xC4); 
	LCD_WriteData(0x8D); 
	LCD_WriteData(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	 
	LCD_WriteReg(0xC5); //VCOM 
	LCD_WriteData(0x1A); 
	 
	LCD_WriteReg(0x36); //MX, MY, RGB mode 
	LCD_WriteData(0x08); 
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------// 
	 
	LCD_WriteReg(0xE0); 
	LCD_WriteData(0x04); 
	LCD_WriteData(0x22); 
	LCD_WriteData(0x07); 
	LCD_WriteData(0x0A); 
	LCD_WriteData(0x2E); 
	LCD_WriteData(0x30); 
	LCD_WriteData(0x25); 
	LCD_WriteData(0x2A); 
	LCD_WriteData(0x28); 
	LCD_WriteData(0x26); 
	LCD_WriteData(0x2E); 
	LCD_WriteData(0x3A); 
	LCD_WriteData(0x00); 

	LCD_WriteData(0x01); 
	LCD_WriteData(0x03); 
	LCD_WriteData(0x13); 
	LCD_WriteReg(0xE1); 
	LCD_WriteData(0x04); 
	LCD_WriteData(0x16); 
	LCD_WriteData(0x06); 
	LCD_WriteData(0x0D); 
	LCD_WriteData(0x2D); 
	LCD_WriteData(0x26); 
	LCD_WriteData(0x23); 
	LCD_WriteData(0x27); 
	LCD_WriteData(0x27); 
	LCD_WriteData(0x25); 
	LCD_WriteData(0x2D); 
	LCD_WriteData(0x3B); 
	LCD_WriteData(0x00); 
	LCD_WriteData(0x01); 
	LCD_WriteData(0x04); 
	LCD_WriteData(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------// 

	LCD_WriteReg(0x3A); //65k mode 
	LCD_WriteData(0x05); 
	LCD_WriteReg(0x29); //Display on 
	LCD_WriteReg(0x2c);
	
	SPILCD_BL_Ctl(250);

}

void SPILCD_BL_Ctl(unsigned char value)
{

	if(value > 250)
		value = 250;
	
	LCD_BL_SET(value);
	
	spilcd_info.blSta = value;

}

/*
************************************************************
*	函数名称：	LCD_SetAddress
*
*	函数功能：	设置显示地址区域
*
*	入口参数：	xStart：X轴起始地址
*				yStart：Y轴起始地址
*				xEnd：X轴结束地址
*				yEnd：Y轴结束地址
*
*	返回参数：	无
*
*	说明：		在此区域内写数据时，地址自动自增
************************************************************
*/
void LCD_SetAddress(unsigned short xStart, unsigned short yStart, unsigned short xEnd, unsigned short yEnd)
{

	LCD_WriteReg(0x2a);
	LCD_WriteData(xStart >> 8);
	LCD_WriteData(0x00FF & xStart);
	LCD_WriteData(xEnd >> 8);
	LCD_WriteData(0x00FF & xEnd);
  
	LCD_WriteReg(0x2b);
	LCD_WriteData(yStart >> 8);
	LCD_WriteData(0x00FF & yStart);
	LCD_WriteData(yEnd >> 8);
	LCD_WriteData(0x00FF & yEnd);

	LCD_WriteReg(0x2C);

}

/*
************************************************************
*	函数名称：	LCD_DrawPoint
*
*	函数功能：	画点
*
*	入口参数：	x：X轴位置
*				y：Y轴位置
*				color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DrawPoint(unsigned short x, unsigned short y, unsigned short color)
{
	
	RTOS_ENTER_CRITICAL();

	LCD_SetAddress(x, y, x, y);
	LCD_WriteData16(color);
	
	RTOS_EXIT_CRITICAL();

}

/*
************************************************************
*	函数名称：	SPILCD_FillRect
*
*	函数功能：	画Rect
*
*	入口参数：	xStart：X轴起始地址
*				yStart：Y轴起始地址
*				xEnd：X轴结束地址
*				yEnd：Y轴结束地址
*				color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_FillRect(unsigned short xStart, unsigned short yStart, unsigned short xEnd, unsigned short yEnd, unsigned short color)
{

	unsigned short i = 0, j = 0;
	
	RTOS_ENTER_CRITICAL();
	
	LCD_SetAddress(xStart, yStart, xEnd, yEnd);
	
	for(; i <= yEnd; i++)
	{
		for(j = 0; j <= xEnd; j++)
		{
			LCD_WriteData16(color);
		}
	}
	
	RTOS_EXIT_CRITICAL();

}

/*
************************************************************
*	函数名称：	SPILCD_Draw16BPP
*
*	函数功能：	画图
*
*	入口参数：	x：X轴位置
*				y：Y轴位置
*				color：颜色值
*				xySize：画点次数
*
*	返回参数：	无
*
*	说明：		专供ucGUI使用
************************************************************
*/
void SPILCD_Draw16BPP(unsigned short x, unsigned short y, const unsigned short *color, const unsigned short *pTrans,
						int xySize, unsigned char mode)
{
	
	RTOS_ENTER_CRITICAL();

	LCD_SetAddress(x, y, x + xySize, y + xySize);
	
	switch(mode)
	{
		case 0:
			
			for(; xySize > 0; xySize--)
			{
				LCD_WriteData16(*(*color++ + pTrans));
			}
		
		break;
		
		case 1:
			
			for(; xySize > 0; xySize--)
				LCD_WriteData16(*color++);
		
		break;
		
		case 2:
			
			for(; xySize > 0; xySize--)
			{
				if(*color)
					LCD_WriteData16(*(*color + pTrans));
				
				color++;
			}
		
		break;
			
		case 3:
			
			for(; xySize > 0; xySize--)
			{
				if(*color)
					LCD_WriteData16(*color);
				
				color++;
			}
		
		break;
	}
	
	RTOS_EXIT_CRITICAL();

}

#if(USE_GUI == 0)

/***********************************************************************************************************
************************************************************************************************************


									以下为不使用图形系统时专用的函数


************************************************************************************************************
************************************************************************************************************/


/*
************************************************************
*	函数名称：	LCD_Clear
*
*	函数功能：	填充屏幕
*
*	入口参数：	color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_Clear(unsigned short color)
{

	unsigned short i = 0, j = 0;
	
	RTOS_ENTER_CRITICAL();
	
	LCD_SetAddress(0, 0, 127, 127);
	
	for(; i < 128; i++)
	{
		for(j = 0; j < 128; j++)
		{
			LCD_WriteData16(color);
		}
	}
	
	RTOS_EXIT_CRITICAL();

}

/*
************************************************************
*	函数名称：	SPILCD_DrawLine
*
*	函数功能：	在屏幕上画一条线
*
*	入口参数：	x1：X轴起始点
*				y1：Y轴起始点
*				x2：X轴结束点
*				y2：Y轴结束点
*				color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	
	unsigned short t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	
	delta_x = x2 - x1;										//计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	if(delta_x > 0)
		incx = 1;											//设置单步方向
	else if(delta_x == 0)
		incx = 0;											//垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}

	if(delta_y > 0)
		incy = 1;
	else if(delta_y == 0)
		incy = 0;											//水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}

	if(delta_x > delta_y)
		distance = delta_x;									//选取基本增量坐标轴
	else
		distance = delta_y;

	for(t = 0; t <= distance + 1; t++)						//画线输出
	{
		SPILCD_DrawPoint(uRow, uCol, color);				//画点
		xerr += delta_x ;
		yerr += delta_y ;

		if(xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}

		if(yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
	
}

/*
************************************************************
*	函数名称：	SPILCD_DrawRectangle
*
*	函数功能：	在屏幕上画一个空心矩形
*
*	入口参数：	x1：X轴起始点
*				y1：Y轴起始点
*				x2：X轴结束点
*				y2：Y轴结束点
*				color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	
	SPILCD_DrawLine(x1, y1, x2, y1, color);
	SPILCD_DrawLine(x1, y1, x1, y2, color);
	SPILCD_DrawLine(x1, y2, x2, y2, color);
	SPILCD_DrawLine(x2, y1, x2, y2, color);
	
}

/*
************************************************************
*	函数名称：	SPILCD_DrawCircle
*
*	函数功能：	在屏幕上画一个圆形
*
*	入口参数：	x0：圆心X轴坐标
*				y0：圆心Y轴坐标
*				r：半径
*				color：颜色值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DrawCircle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color)
{
	
	int a, b;
	int di;
	
	a = 0;
	b = r;
	di = 3 - (r << 1);       //判断下个点位置的标志

	while(a <= b)
	{
		SPILCD_DrawPoint(x0 + a, y0 - b, color);        //5
		SPILCD_DrawPoint(x0 + b, y0 - a, color);        //0
		SPILCD_DrawPoint(x0 + b, y0 + a, color);        //4
		SPILCD_DrawPoint(x0 + a, y0 + b, color);        //6
		SPILCD_DrawPoint(x0 - a, y0 + b, color);        //1
		SPILCD_DrawPoint(x0 - b, y0 + a, color);
		SPILCD_DrawPoint(x0 - a, y0 - b, color);        //2
		SPILCD_DrawPoint(x0 - b, y0 - a, color);        //7
		a++;

		//使用Bresenham算法画圆
		if(di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
}

/*
************************************************************
*	函数名称：	SPILCD_DrawPic
*
*	函数功能：	画图
*
*	入口参数：	pic：根据图片流指针画图
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DrawPic(const unsigned char *pic)
{

	unsigned short i = 0, j = 0;
	
	RTOS_ENTER_CRITICAL();
	
	LCD_SetAddress(0, 0, 127, 127);
	
	for(; i < 128; i++)
	{
		for(j = 0; j < 128; j++)
		{
			LCD_WriteData16(*pic | (unsigned short)*(++pic) << 8);
			
			pic++;
		}
	}
	
	RTOS_EXIT_CRITICAL();

}

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
/*
************************************************************
*	函数名称：	SPILCD_DisChar
*
*	函数功能：	显示一个ASCII字符
*
*	入口参数：	x：X坐标
*				y：Y坐标
*				size：字符尺寸
*				color：显示的颜色
*				mode：0-叠加显示		1-不叠加显示
*				Char：字符索引
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DisChar(unsigned short x, unsigned short y, unsigned char size, unsigned short color, _Bool mode, unsigned char Char)    //能显示动态数据  一个字符占用12像素宽  24像素高
{

	unsigned char temp, t1, t;
	unsigned short y0 = y;
	unsigned char csize = ((size >> 3) + ((size % 8) ? 1 : 0)) * (size >> 1);	//得到字体一个字符对应点阵集所占的字节数  12  16都没变  24变36

	Char -= 32;//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

	for(t = 0; t < csize; t++)
	{
		if(size == 12)
			temp = asc2_1206[Char][t]; 	 //调用1206字体
		else if(size == 16)
			temp = asc2_1608[Char][t];	//调用1608字体
		else if(size == 24)
			temp = asc2_2412[Char][t];	//调用2412字体
		else
			return;						//没有的字库

		for(t1 = 0; t1 < 8; t1++)
		{
			if(mode)
				SPILCD_DrawPoint(x, y, BGC);    //写背景色擦除

			if(temp & 0x80)
				SPILCD_DrawPoint(x, y, color);

			temp <<= 1;
			y++;

			if((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}

}

/*
************************************************************
*	函数名称：	SPILCD_DisString
*
*	函数功能：	显示一个字符串
*
*	入口参数：	x：X坐标
*				y：Y坐标
*				size：字符尺寸
*				color：显示的颜色
*				mode：0-叠加显示		1-不叠加显示
*				...
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPILCD_DisString(unsigned short x, unsigned short y, unsigned char size, unsigned short color, unsigned char mode, char *fmt, ...)
{

	char LCD_Printf_Buf[256];
	va_list ap;
	char *pStr = LCD_Printf_Buf;

	va_start(ap, fmt);
	vsprintf(LCD_Printf_Buf, fmt, ap);
	va_end(ap);

	while(*pStr != 0)
	{
		SPILCD_DisChar(x, y, size, color, mode, *pStr++);

		x += size >> 1;
	}

}

/*
************************************************************
*	函数名称：	SPILCD_DisZW
*
*	函数功能：	显示一个字汉字
*
*	入口参数：	x：X坐标
*				y：Y坐标
*				color：颜色值
*				zw：字库数据指针
*
*	返回参数：	无
*
*	说明：		此函数只显示16x16的纵向取摸的汉字(为了和OLED使用相同的字库数据)
*				显示方式：16x16分2排，每排16列，每排每列为一个字节
*
*				B0								B0
*				B1								B1
*				B2 B							B2 B
*				B3 Y							B3 Y
*				B4 T	....................	B4 T
*				B5 E							B5 E
*				B6 0							B6 15
*				B7								B7
*				
*				B0								B0
*				B1								B1
*				B2 B							B2 B
*				B3 Y							B3 Y
*				B4 T	....................	B4 T
*				B5 E							B5 E
*				B6 16							B6 31
*				B7								B7
*
************************************************************
*/
void SPILCD_DisZW(unsigned short x, unsigned short y, unsigned short color, const unsigned char *zw)
{
	
	unsigned char i = 0, j = 0, k = 0;
	unsigned char byte = 0;
	
	for(; i < 2; i++)													//排数
	{
		for(j = 0; j < 16; j++)											//列数
		{
			byte = zw[j + (i << 4)];									//取一个字节显示到LCD上；16x16汉字，共32字节
			for(k = 0; k < 8; k++)										//一个汉字字节里有8位，每位代表一个点
			{
				SPILCD_DrawPoint(x + j, y + k + (i << 3), BGC);			//写背景色擦除
				
				if(byte & 0x01)											//此点有数据，则显示
					SPILCD_DrawPoint(x + j, y + k + (i << 3), color);
				
				byte >>= 1;												//低位在前
			}
		}
	}

}

#endif
