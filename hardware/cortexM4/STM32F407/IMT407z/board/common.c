#include "common.h"
#include "wind_thread.h"

/*********************************************************************************
*************************MCU启明 STM32F407核心开发板******************************
**********************************************************************************
* 文件名称: common.c                                                             *
* 文件简述：各个工程所需调用的公共文件                                           *
* 创建日期：2015.03.03                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：包涵数据类型定义、IO口位定义、位段定义与延时函数定义                 * 
**********************************************************************************
*********************************************************************************/	  

/****************************************************************************
* 名    称: void GPIO_group_OUT(_gpio_group *group,w_uint16_t outdata)
* 功    能：使用随意16个IO口组成一个16位并行输出口
* 入口参数：*group： 任意16个IO口为元素的结构体指针
            outdata: 16位输出数值
* 返回参数：无
* 说    明：outdata从高位开始赋值
****************************************************************************/
void GPIO_group_OUT(_gpio_group *group,w_uint16_t outdata)
{
  w_uint8_t t;
	for(t=0;t<16;t++)
    {               
        if((outdata&0x8000)>>15)  
				{
						switch(t)
						{
								case 0:	   group->data15=1; break;
								case 1:	   group->data14=1; break;
								case 2:	   group->data13=1; break;
								case 3:	   group->data12=1; break;
								case 4:	   group->data11=1; break;
								case 5:	   group->data10=1; break;
								case 6:	   group->data9=1;  break;
								case 7:	   group->data8=1;  break;
								case 8:	   group->data7=1;  break;
								case 9:	   group->data6=1;  break;
								case 10:	 group->data5=1;  break;
								case 11:	 group->data4=1;  break;
								case 12:	 group->data3=1;  break;
								case 13:	 group->data2=1;  break;
								case 14:	 group->data1=1;  break;
								case 15:	 group->data0=1;  break;
						}
				}
				else
				{
				  switch(t)
						{
								case 0:	   group->data15=0; break;
								case 1:	   group->data14=0; break;
								case 2:	   group->data13=0; break;
								case 3:	   group->data12=0; break;
								case 4:	   group->data11=0; break;
								case 5:	   group->data10=0; break;
								case 6:	   group->data9=0;  break;
								case 7:	   group->data8=0;  break;
								case 8:	   group->data7=0;  break;
								case 9:	   group->data6=0;  break;
								case 10:	 group->data5=0;  break;
								case 11:	 group->data4=0;  break;
								case 12:	 group->data3=0;  break;
								case 13:	 group->data2=0;  break;
								case 14:	 group->data1=0;  break;
								case 15:	 group->data0=0;  break;
						}
				}
     outdata<<=1; 	
	  }
}
/****************************************************************************
* 名    称: void GPIO_bits_OUT(GPIO_TypeDef* GPIOx, w_uint8_t start_bit, w_uint8_t bit_size,w_uint16_t outdata)
* 功    能：位段操作实现，同一IO口的几位并行输出操作
* 入口参数：* GPIOx：  对应的IO口
*           start_bit: 并行输出的起始位
*           bit_size:  要并行输出的位数
* 返回参数：无
* 说    明：start_bit: 0~14
            bit_size:  1~16 
            bit_size<=16-start_bit
****************************************************************************/
void GPIO_bits_OUT(GPIO_TypeDef* GPIOx, w_uint8_t start_bit, w_uint8_t bit_size,w_uint16_t outdata)
{
  w_uint8_t i=0;
	w_uint16_t bu1=0;w_uint16_t middata=1;

	if( bit_size>(16-start_bit) ) 
     bit_size=16-start_bit;
	
	i=start_bit;
	if(i>0)
		 {
			 while(i--)
         { bu1+=middata; middata*=2;}
		 }
	
   assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
   
	 GPIOx->ODR&=(  ( (0xffff<<bit_size) <<start_bit ) |bu1   ); 
	 GPIOx->ODR|=(outdata<<start_bit);		 
}

/*****************************************************************************
*****************************以下代码参考网上*********************************
*****************************************************************************/
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(w_uint32_t addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

//利用系统滴答定时，编写的延时函数

static w_uint8_t  fac_us=0; //us延时倍乘数			   
static w_uint16_t fac_ms=0; //ms延时倍乘数,在ucos下,代表每个节拍的ms数


			   
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init(void)
{
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	w_uint32_t reload;
#endif
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SYSCLK/8;		//不论是否使用ucos,fac_us都需要使用
	    
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	reload=SYSCLK/8;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/OS_TICKS_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间
							//reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右	
	fac_ms=1000/OS_TICKS_PER_SEC;//代表ucos可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK
#else
	fac_ms=(w_uint16_t)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数   
#endif
}


//延时nus
//nus:要延时的us数.		    								   
void delay_us(w_uint32_t nus)
{		
    wind_thread_sleep((nus+999)/1000);
}
//延时nms
//nms:要延时的ms数
void delay_ms(w_uint16_t nms)
{	
    wind_thread_sleep(nms);
}
	 
 
