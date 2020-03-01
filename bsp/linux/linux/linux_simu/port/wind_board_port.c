/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_board_port.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os��CPU��ϵ��صĴ���
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
#include "wind_string.h"
#include <stdlib.h>

#include <ucontext.h>
#include <signal.h>
#include <stdio.h>


/*
 * �豸������߳�ģʽ�����ĳ�ʼ������Ĺ��Ӻ�����Ϊ�˱�֤tick
 * �жϵ��ȶ���һ������ϵͳtick�жϻ�ŵ�ϵͳ�����߳�״̬ʱ��ʼ
 * ִ�У��ڽ����߳�֮ǰ������tick�����жϼ��߳��л���
 */
extern void wind_tick_isr(void);


void wind_init_hook(void)
{

}


void wind_enter_thread_hook(void)
{
	//tick_timer_run();
}


/*
 * ����CPU�豸����,һ���MCU����������������Ĺ��ܣ����������ʵ�ּ��ɣ�
 * ������ܲ��Ǳ����,���ޱ�Ҫ���Բ���ʵ�֣�ֱ�ӿ��ü��ɡ�
 */

void wind_system_reset(void)
{
    extern char *argv_bak;
    if(argv_bak != W_NULL)
        system(argv_bak);
}



#if WIND_HEAP_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ڴ�ѹ���ʱ����Ҫ�ڴ�ѵ���ʼ��ַ�ͶԵĴ�С��
 * wind-os����֧�ִ���������������ڴ�ѣ����ҿ������ڴ����
 * ����һ��ռ䴴��һ��Ƕ�׵��ڴ�ѣ�����ĳЩ�ض���Ŀ��
 */ 
#include "wind_heap.h"
static w_uint8_t buff1[0x100000];
static w_uint8_t buff2[0x100000];
void _wind_heaps_create(void)
{
    wind_heap_create("heap1",(w_addr_t)(buff1),sizeof(buff1),0);
    wind_heap_create("heap2",(w_addr_t)(buff2),sizeof(buff2),0);
}
#endif

#if WIND_MODULE_VFS_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ļ�ϵͳ����ʱ����Ҫ�������ʼ��mount�Ĺ���
 * �ڲ���Ҫ�ļ�ϵͳʱ�����Բ�ʵ��
 */ 
#include "wind_fs.h"
void _wind_fs_mount_init(void)
{
    wind_vfs_mount("fs0","listfs","disk0","/");
    wind_vfs_mount("fs1","listfs","memblk","/mem");
    wind_vfs_mount("fs2","treefs","null0","/var");
    wind_vfs_mount("fs3","treefs","null1","/mnt/");
    wind_vfs_mount("fs4","hostfs","/home/jason/wfs/","/host/");
}
#endif


/*
 * ��ʼ���߳�ջ�������̳߳����л�ʱ����ջ����ȡ����ʼ������
 * ���������˳����Բο���Ӧ��CPU�߳̽���ջ��˳��
 */ 

extern ucontext_t mainctx;
w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth)
{    

	ucontext_t *ctx;   
    w_int32_t ctx_size;
    w_stack_t* stk;
	
	ctx_size = 4 + sizeof(ucontext_t)/sizeof(w_stack_t);
    stk = (w_stack_t*)(pstkbt + stk_depth - 1 - ctx_size);
	
	ctx = (ucontext_t *)stk;
	getcontext(ctx);
    ctx->uc_link = &mainctx; /// next context   
    ctx->uc_stack.ss_sp = (void*)(pstkbt); /// base address    
    ctx->uc_stack.ss_size = (stk - pstkbt - 1) * sizeof(w_stack_t);
    ctx->uc_stack.ss_flags = 0;
	WIND_ASSERT_RETURN(ctx->uc_stack.ss_size > 0x1000,W_NULL);
    makecontext(ctx, (void*)pfunc, 1, pdata);
	return ((w_stack_t *)stk);
}

