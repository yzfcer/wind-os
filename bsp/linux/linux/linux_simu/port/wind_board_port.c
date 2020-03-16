/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_board_port.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的CPU体系相关的代码
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
#include "wind_string.h"
#include <stdlib.h>

#include <ucontext.h>
#include <signal.h>
#include <stdio.h>


/*
 * 设备进入多线程模式后函数的初始化处理的钩子函数，为了保证tick
 * 中断的稳定，一般配置系统tick中断会放到系统进入线程状态时开始
 * 执行，在进入线程之前，不让tick触发中断级线程切换。
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
 * 触发CPU设备重启,一般的MCU都有软件触发重启的功能，在这个函数实现即可，
 * 这个功能不是必须的,如无必要可以不用实现，直接空置即可。
 */

void wind_system_reset(void)
{
    extern char *argv_bak;
    if(argv_bak != W_NULL)
        system(argv_bak);
}



#if WIND_HEAP_SUPPORT
/*
 * 在系统需要支持内存堆功能时，需要内存堆的起始地址和对的大小，
 * wind-os可以支持创建多个不连续的内存堆，并且可以在内存对中
 * 申请一块空间创建一个嵌套的内存堆，用于某些特定的目的
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
 * 在系统需要支持文件系统功能时，需要在这里初始化mount的规则，
 * 在不需要文件系统时，可以不实现
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
 * 初始化线程栈，用于线程初次切换时，从栈里面取出初始化参数
 * 填入参数的顺序可以参考相应的CPU线程进出栈的顺序
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

