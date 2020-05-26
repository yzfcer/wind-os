/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_board_port.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os board adapter interface
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26    
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
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
#define SYSCLK 72    //System tick by MHz
#endif



/*
 Hook function of function initialization after the device enters multithreading mode. 
 In order to ensure the stability of tick interrupt, generally configure the system tick 
 interrupt to be executed when the system enters the thread state. Before entering the 
 thread, do not let tick trigger the interrupt level thread switch.
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
 To trigger the restart of CPU device, general MCU has the function of software triggering 
 the restart, which can be realized in this function. This function is not necessary. 
 If it is unnecessary, it can be directly vacant. 
 */
void wind_system_reset(void)
{
    NVIC_SystemReset();
}



#if WIND_HEAP_SUPPORT
/*
 When the system needs to support the memory heap function, it needs the starting 
 address of the memory heap and the size of the pair. wind-os can support the creation 
 of multiple discontinuous memory heaps, and can apply for a piece of space in the 
 memory pair to create a nested memory heap for some specific purposes
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


#if WIND_MODULE_VFS_SUPPORT
/*
 When the system needs to support the file system function, it needs to initialize 
 the rule of Mount here. When the file system is not needed, it need not be implemented
 */ 
#include "wind_file.h"
void _wind_fs_mount_init(void)
{
    //wind_vfs_mount("treefs","treefs","null","/");
}
#endif

/*
 Initialization thread stack is used to take initialization parameters from the 
 stack when the thread is switched for the first time. The order of filling in the 
 parameters can refer to the order of CPU threads entering and leaving the stack 
 */ 
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth)
{
    w_stack_t *stk;
    stk = pstkbt + stk_depth - 1;  
                                  
    stk       = pstkbt;                              /* Load stack pointer                                 */
                                                     /* Registers stacked as if auto-saved on exception    */
    *(stk)    = (w_stack_t)0x01000000L;             /* xPSR                                               */
    *(--stk)  = (w_stack_t)pfunc;                   /* Entry Point                                        */
    *(--stk)  = (w_stack_t)0xFFFFFFFEL;             /* R14 (LR) (init value will cause fault if ever used)*/
    *(--stk)  = (w_stack_t)0x12121212L;             /* R12                                                */
    *(--stk)  = (w_stack_t)0x03030303L;             /* R3                                                 */
    *(--stk)  = (w_stack_t)0x02020202L;             /* R2                                                 */
    *(--stk)  = (w_stack_t)0x01010101L;             /* R1                                                 */
    *(--stk)  = (w_stack_t)pdata;                   /* R0 : argument                                      */

                                                     /* Remaining registers saved on process stack         */
    *(--stk)  = (w_stack_t)0x11111111L;             /* R11                                                */
    *(--stk)  = (w_stack_t)0x10101010L;             /* R10                                                */
    *(--stk)  = (w_stack_t)0x09090909L;             /* R9                                                 */
    *(--stk)  = (w_stack_t)0x08080808L;             /* R8                                                 */
    *(--stk)  = (w_stack_t)0x07070707L;             /* R7                                                 */
    *(--stk)  = (w_stack_t)0x06060606L;             /* R6                                                 */
    *(--stk)  = (w_stack_t)0x05050505L;             /* R5                                                 */
    *(--stk)  = (w_stack_t)0x04040404L;             /* R4                                                 */

    return (stk);
}




