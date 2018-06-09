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
#include "usart1.h"
#include "core_cm4.h"
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
    //wind_heap_print(&g_core.heaplist);
    //wind_heapitem_print(&g_core.heaplist);
}
#endif




w_pstack_t wind_stk_init(thread_run_f pfunc,void *pdata, w_pstack_t pstkbt)
{
    w_pstack_t stk;
    stk = pstkbt;                            /* Load stack pointer                                 */

#if (__FPU_PRESENT==1)&&(__FPU_USED==1)	
    *(--stk) = (w_uint32_t)0x00000000L; //No Name Register  
    *(--stk) = (w_uint32_t)0x00001000L; //FPSCR
    *(--stk) = (w_uint32_t)0x00000015L; //s15
    *(--stk) = (w_uint32_t)0x00000014L; //s14
    *(--stk) = (w_uint32_t)0x00000013L; //s13
    *(--stk) = (w_uint32_t)0x00000012L; //s12
    *(--stk) = (w_uint32_t)0x00000011L; //s11
    *(--stk) = (w_uint32_t)0x00000010L; //s10
    *(--stk) = (w_uint32_t)0x00000009L; //s9
    *(--stk) = (w_uint32_t)0x00000008L; //s8
    *(--stk) = (w_uint32_t)0x00000007L; //s7
    *(--stk) = (w_uint32_t)0x00000006L; //s6
    *(--stk) = (w_uint32_t)0x00000005L; //s5
    *(--stk) = (w_uint32_t)0x00000004L; //s4
    *(--stk) = (w_uint32_t)0x00000003L; //s3
    *(--stk) = (w_uint32_t)0x00000002L; //s2
    *(--stk) = (w_uint32_t)0x00000001L; //s1
    *(--stk) = (w_uint32_t)0x00000000L; //s0
#endif
                                         /* Registers stacked as if auto-saved on exception    */
    *(stk)    = (w_uint32_t)0x01000000L;             /* xPSR                                               */
    *(--stk)  = (w_uint32_t)pfunc;                    /* Entry Point                                        */
    *(--stk)  = (w_uint32_t)NULL;//OS_TaskReturn;           /* R14 (LR) (init value will cause fault if ever used)*/
    *(--stk)  = (w_uint32_t)0x12121212L;             /* R12                                                */
    *(--stk)  = (w_uint32_t)0x03030303L;             /* R3                                                 */
    *(--stk)  = (w_uint32_t)0x02020202L;             /* R2                                                 */
    *(--stk)  = (w_uint32_t)0x01010101L;             /* R1                                                 */
    *(--stk)  = (w_uint32_t)pdata;                   /* R0 : argument                                      */

#if (__FPU_PRESENT==1)&&(__FPU_USED==1)	
    *(--stk) = (w_uint32_t)0x00000031L; //s31
    *(--stk) = (w_uint32_t)0x00000030L; //s30
    *(--stk) = (w_uint32_t)0x00000029L; //s29
    *(--stk) = (w_uint32_t)0x00000028L; //s28
    *(--stk) = (w_uint32_t)0x00000027L; //s27
    *(--stk) = (w_uint32_t)0x00000026L; //s26	
    *(--stk) = (w_uint32_t)0x00000025L; //s25
    *(--stk) = (w_uint32_t)0x00000024L; //s24
    *(--stk) = (w_uint32_t)0x00000023L; //s23
    *(--stk) = (w_uint32_t)0x00000022L; //s22
    *(--stk) = (w_uint32_t)0x00000021L; //s21
    *(--stk) = (w_uint32_t)0x00000020L; //s20
    *(--stk) = (w_uint32_t)0x00000019L; //s19
    *(--stk) = (w_uint32_t)0x00000018L; //s18
    *(--stk) = (w_uint32_t)0x00000017L; //s17
    *(--stk) = (w_uint32_t)0x00000016L; //s16
#endif

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

