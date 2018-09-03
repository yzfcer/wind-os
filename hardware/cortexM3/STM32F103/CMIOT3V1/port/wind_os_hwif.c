/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_os_hwif.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os��CPU��ϵ��صĴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
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
#define SYSCLK 72    //ϵͳʱ��
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

//�ѿ����ɷ�����ڴ�ռ���г�ʼ��
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
	//fac_us=SYSCLK/8;		//�����Ƿ�ʹ��ucos,fac_us����Ҫʹ��

	reload=SYSCLK/8;		//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/WIND_TICK_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ��
	//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����	
	//fac_ms=1000/WIND_TICK_PER_SEC;//����ucos������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK

}

