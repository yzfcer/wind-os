/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_board_port.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的单板相关的接口代码
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_board_port.h"
#include "wind_debug.h"
#include "misc.h"
#include "usart.h"
#include "core_cm3.h"

#ifndef SYSCLK
#define SYSCLK 72    //系统时钟
#endif

/*
 * 设备进入多线程模式后函数的初始化处理的钩子函数，为了保证tick
 * 中断的稳定，一般配置系统tick中断会放到系统进入线程状态时开始
 * 执行，在进入线程之前，不让tick触发中断级线程切换。
 */
void wind_enter_thread_hook(void)
{

	w_uint32_t reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	reload=SYSCLK/8;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/WIND_TICK_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK

}

/*
 * 触发CPU设备重启,一般的MCU都有软件触发重启的功能，在这个函数实现即可，
 * 这个功能不是必须的,如无必要可以不用实现，直接空置即可。
 */
void wind_system_reset(void)
{
    NVIC_SystemReset();
}

#if WIND_FS_SUPPORT


/*
 * 在系统需要支持文件系统功能时，需要在这里初始化mount的规则，
 * 在不需要文件系统时，可以不实现
 */ 

#include "wind_file.h"
void _wind_fs_mount_init(void)
{
    wind_vfs_mount("fs0","treefs","null","/");
}
#endif


#if WIND_HEAP_SUPPORT
/*
 * 在系统需要支持内存堆功能时，需要内存堆的起始地址和对的大小，
 * wind-os可以支持创建多个不连续的内存堆，并且可以在内存对中
 * 申请一块空间创建一个嵌套的内存堆，用于某些特定的目的
 */ 
#include "wind_heap.h"
extern unsigned char Image$$ER_HEAP$$ZI$$Base;
extern unsigned char Image$$ER_DATA$$Length;
extern unsigned char Image$$ER_DATA$$ZI$$Base;
#define HEAP1_HEAD ((w_addr_t)&Image$$ER_HEAP$$ZI$$Base+8)
#define HEAD1_LENTH ((w_addr_t)&Image$$ER_DATA$$ZI$$Base+0x10000-8-(w_addr_t)&Image$$ER_HEAP$$ZI$$Base)
void _wind_heaps_create(void)
{
    wind_heap_create("heap1",(w_addr_t)(HEAP1_HEAD),HEAD1_LENTH,0);
}


#endif



/*
 * 初始化线程栈，用于线程初次切换时，从栈里面取出初始化参数
 * 填入参数的顺序可以参考相应的CPU线程进出栈的顺序
 */ 
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt)
{
    w_stack_t *stk;
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




