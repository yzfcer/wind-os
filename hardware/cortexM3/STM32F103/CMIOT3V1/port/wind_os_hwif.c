/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_os_hwif.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的CPU体系相关的代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "misc.h"
#include "usart.h"
#include "core_cm3.h"

#ifndef SYSCLK
#define SYSCLK 72    //系统时钟
#endif

void _wind_target_init(void)
{

}

void wind_system_reset(void)
{
    NVIC_SystemReset();
}



#if WIND_HEAP_SUPPORT
#include "wind_heap.h"
#include "wind_var.h"
#define HEAP1_HEAD  0x10000000
#define HEAD1_LENTH (64*1024)

//堆可自由分配的内存空间进行初始化
void _wind_heaps_init(void)
{
    wind_heap_create("heap0",HEAP1_HEAD,HEAD1_LENTH,0);
    wind_heap_print(&g_core.heaplist);
    wind_heapitem_print(&g_core.heaplist);
}

#endif




w_pstack_t _wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_pstack_t pstkbt)
{
    w_pstack_t stk;
    stk = pstkbt;  
                                  
    stk       = pstkbt;                            /* Load stack pointer                                 */
                                                 /* Registers stacked as if auto-saved on exception    */
    *(stk)    = (w_uint32_t)0x01000000L;             /* xPSR                                               */
    *(--stk)  = (w_uint32_t)pfunc;                    /* Entry Point                                        */
    *(--stk)  = (w_uint32_t)0xFFFFFFFEL;             /* R14 (LR) (init value will cause fault if ever used)*/
    *(--stk)  = (w_uint32_t)0x12121212L;             /* R12                                                */
    *(--stk)  = (w_uint32_t)0x03030303L;             /* R3                                                 */
    *(--stk)  = (w_uint32_t)0x02020202L;             /* R2                                                 */
    *(--stk)  = (w_uint32_t)0x01010101L;             /* R1                                                 */
    *(--stk)  = (w_uint32_t)pdata;                   /* R0 : argument                                      */

                                                 /* Remaining registers saved on process stack         */
    *(--stk)  = (w_uint32_t)0x11111111L;             /* R11                                                */
    *(--stk)  = (w_uint32_t)0x10101010L;             /* R10                                                */
    *(--stk)  = (w_uint32_t)0x09090909L;             /* R9                                                 */
    *(--stk)  = (w_uint32_t)0x08080808L;             /* R8                                                 */
    *(--stk)  = (w_uint32_t)0x07070707L;             /* R7                                                 */
    *(--stk)  = (w_uint32_t)0x06060606L;             /* R6                                                 */
    *(--stk)  = (w_uint32_t)0x05050505L;             /* R5                                                 */
    *(--stk)  = (w_uint32_t)0x04040404L;             /* R4                                                 */

    return (stk);
}



void wind_tick_init(void)
{

	w_uint32_t reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//fac_us=SYSCLK/8;		//不论是否使用ucos,fac_us都需要使用

	reload=SYSCLK/8;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/WIND_TICK_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间
	//reload为24位寄存器,最大值:16777216,在168M下,约合0.7989s左右	
	//fac_ms=1000/WIND_TICK_PER_SEC;//代表ucos可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK

}

