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
#include <stdlib.h>
#ifdef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU 
#endif
//#ifndef __x86_64__
//#define __x86_64__ 
//#endif

//#include <features.h>
#include <ucontext.h>

#include <signal.h>
#include <bits/sigcontext.h>		
#include <string.h>    
#include <unistd.h>
#include <setjmp.h>
#include <sys/select.h>    
//#include <stdio.h>



#define OS_TASK_DEF_STK_SIZE 0x2000


/*
 * �豸������߳�ģʽ�����ĳ�ʼ������Ĺ��Ӻ�����Ϊ�˱�֤tick
 * �жϵ��ȶ���һ������ϵͳtick�жϻ�ŵ�ϵͳ�����߳�״̬ʱ��ʼ
 * ִ�У��ڽ����߳�֮ǰ������tick�����жϼ��߳��л���
 */
void wind_enter_thread_hook(void)
{
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
    //ExitProcess(0);
}



#if WIND_HEAP_SUPPORT
/*
 * ��ϵͳ��Ҫ֧���ڴ�ѹ���ʱ����Ҫ�ڴ�ѵ���ʼ��ַ�ͶԵĴ�С��
 * wind-os����֧�ִ���������������ڴ�ѣ����ҿ������ڴ����
 * ����һ��ռ䴴��һ��Ƕ�׵��ڴ�ѣ�����ĳЩ�ض���Ŀ��
 */ 
#include "wind_heap.h"
static w_uint8_t buff1[0x10000];
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
    //wind_vfs_mount("fs0","listfs","memblk","/");
    wind_vfs_mount("fs1","treefs","null0","/var");
    wind_vfs_mount("fs2","treefs","null1","/mnt/");
    //wind_vfs_mount("fs3","listfs","disk0","/disk/");
}
#endif


/*
 * ��ʼ���߳�ջ�������̳߳����л�ʱ����ջ����ȡ����ʼ������
 * ���������˳����Բο���Ӧ��CPU�߳̽���ջ��˳��
 */ 

w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt)
{    
    w_stack_t*    stk;   
    ucontext_t ctx;   
    w_int32_t sigsize = 20 + sizeof(ctx);   
    getcontext(&ctx);   
    stk = (w_stack_t*)((w_uint32_t)pstkbt - sigsize);   
    ctx.uc_link = NULL;
    ctx.uc_mcontext.gregs[REG_EBP] = (greg_t)stk;
    ctx.uc_stack.ss_sp = (void*)(((w_uint32_t)stk) - (OS_TASK_DEF_STK_SIZE) + sigsize); /// base address    
    ctx.uc_stack.ss_size = OS_TASK_DEF_STK_SIZE - sigsize;   
   
    makecontext(&ctx, (void*)pfunc, 1, pdata);   
    memcpy(stk, &ctx, sizeof(ctx));   
       
    return ((w_stack_t *)stk);   
}

