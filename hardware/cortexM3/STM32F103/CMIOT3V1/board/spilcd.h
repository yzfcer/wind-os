#ifndef _SPILCD_H_
#define _SPILCD_H_





typedef struct
{

	unsigned char blSta;

} SPILCD_INFO;

extern SPILCD_INFO spilcd_info;


#define USE_GUI		1


#define SPILCD_IO	SPI2


#define HW_SPI_EN	1


#define LCD_CS_H		GPIOB->BSRR = 1UL << 12
#define LCD_CS_L		GPIOB->BSRR = 1UL << 28

#if(HW_SPI_EN == 0)

#define LCD_SCK_H		GPIOB->BSRR = 1UL << 13
#define LCD_SCK_L		GPIOB->BSRR = 1UL << 29

#define LCD_SDA_H		GPIOB->BSRR = 1UL << 15
#define LCD_SDA_L		GPIOB->BSRR = 1UL << 31

#endif

#define LCD_AO_H		GPIOB->BSRR = 1UL << 14
#define LCD_AO_L		GPIOB->BSRR = 1UL << 30

#define LCD_RST_OFF		GPIOC->BSRR = 1UL << 9
#define LCD_RST_ON		GPIOC->BSRR = 1UL << 25

#define LCD_BL_SET(x)	TIM_SetCompare1(TIM3, x)		



void SPILCD_Init(void);

void SPILCD_BL_Ctl(unsigned char value);

void SPILCD_DrawPoint(unsigned short x, unsigned short y, unsigned short color);

void SPILCD_FillRect(unsigned short xStart, unsigned short yStart, unsigned short xEnd, unsigned short yEnd, unsigned short color);

void SPILCD_Draw16BPP(unsigned short x, unsigned short y, const unsigned short *color, const unsigned short *pTrans,
						int xySize, unsigned char mode);


#if(USE_GUI == 0)

/***********************************************************************************************************
************************************************************************************************************


									以下为不使用图形系统时专用的函数


************************************************************************************************************
************************************************************************************************************/

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define BGC WHITE				//背景色


void SPILCD_DrawPic(const unsigned char *pic);

void SPILCD_Clear(unsigned short color);

void SPILCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color);

void SPILCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color);

void SPILCD_DrawCircle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color);

void SPILCD_DisString(unsigned short x, unsigned short y, unsigned char size, unsigned short color, unsigned char mode, char *fmt, ...);

void SPILCD_DisZW(unsigned short x, unsigned short y, unsigned short color, const unsigned char *zw);

#endif

#endif
