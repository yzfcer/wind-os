/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_board_port.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĵ�����صĽӿڴ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
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
#define SYSCLK 72    //ϵͳʱ��
#endif

/*
 * �豸������߳�ģʽ�����ĳ�ʼ������Ĺ��Ӻ�����Ϊ�˱�֤tick
 * �жϵ��ȶ���һ������ϵͳtick�жϻ�ŵ�ϵͳ�����߳�״̬ʱ��ʼ
 * ִ�У��ڽ����߳�֮ǰ������tick�����жϼ��߳��л���
 */
void wind_enter_thread_hook(void)
{

	w_uint32_t reload;
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	reload=SYSCLK/8;		//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/WIND_TICK_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ��
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK

}

/*
 * ����CPU�豸����,һ���MCU����������������Ĺ��ܣ����������ʵ�ּ��ɣ�
 * ������ܲ��Ǳ����,���ޱ�Ҫ���Բ���ʵ�֣�ֱ�ӿ��ü��ɡ�
 */
void wind_system_reset(void)
{
    NVIC_SystemReset();
}

#if WIND_FS_SUPPORT


/*
 * ��ϵͳ��Ҫ֧���ļ�ϵͳ����ʱ����Ҫ�������ʼ��mount�Ĺ���
 * �ڲ���Ҫ�ļ�ϵͳʱ�����Բ�ʵ��
 */ 

#include "wind_file.h"
void _wind_fs_mount_init(void)
{
    wind_vfs_mount("fs0","treefs","null","/");
}
#endif


#if WIND_HEAP_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ڴ�ѹ���ʱ����Ҫ�ڴ�ѵ���ʼ��ַ�ͶԵĴ�С��
 * wind-os����֧�ִ���������������ڴ�ѣ����ҿ������ڴ����
 * ����һ��ռ䴴��һ��Ƕ�׵��ڴ�ѣ�����ĳЩ�ض���Ŀ��
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
 * ��ʼ���߳�ջ�������̳߳����л�ʱ����ջ����ȡ����ʼ������
 * ���������˳����Բο���Ӧ��CPU�߳̽���ջ��˳��
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




