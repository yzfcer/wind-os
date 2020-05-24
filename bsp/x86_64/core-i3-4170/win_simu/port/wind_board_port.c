/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_board_port.c
**Author: Jason Zhou
**Last Date: 2012.09.26
**Description: wind os的CPU体系相关的代码
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2012.09.26
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2012.10.20
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_config.h"
#include "wind_type.h"
#include "wind_board_port.h"
#include "wind_debug.h"
#include <Windows.h>                //需要包含该头文件

void wind_init_hook(void)
{
}

/*
 * 设备进入多线程模式后函数的初始化处理的钩子函数，为了保证tick
 * 中断的稳定，一般配置系统tick中断会放到系统进入线程状态时开始
 * 执行，在进入线程之前，不让tick触发中断级线程切换。
 */
void wind_enter_thread_hook(void)
{
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
    ExitProcess(0);
}



#if WIND_HEAP_SUPPORT
/*
 * 在系统需要支持内存堆功能时，需要内存堆的起始地址和对的大小，
 * wind-os可以支持创建多个不连续的内存堆，并且可以在内存对中
 * 申请一块空间创建一个嵌套的内存堆，用于某些特定的目的
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
 * 在系统需要支持文件系统功能时，需要在这里初始化mount的规则，
 * 在不需要文件系统时，可以不实现
 */ 
#include "wind_fs.h"
void _wind_fs_mount_init(void)
{
    wind_vfs_mount("fs0","listfs","disk0","/");
    wind_vfs_mount("fs1","treefs","null0","/var");
    wind_vfs_mount("fs2","treefs","null1","/mnt/");
    wind_vfs_mount("fs3","hostfs","C:/hostfs/","/host");
}
#endif

//向栈中压入一个uint值
static void push_stack(w_stack_t ** Stackpp, w_stack_t v)
{
    *Stackpp -= 1;//esp - 4
    **Stackpp = v;//
    return;
}

/*
 * 初始化线程栈，用于线程初次切换时，从栈里面取出初始化参数
 * 填入参数的顺序可以参考相应的CPU线程进出栈的顺序
 */ 

w_stack_t *_wind_thread_stack_init(thread_run_f pfunc,void *pdata, w_stack_t *pstkbt,w_int32_t stk_depth)
{
	w_stack_t *stk_bottom = pstkbt + stk_depth - 1;
    push_stack(&stk_bottom, (w_stack_t)pdata);        //通过这个指针来找到线程函数，线程参数
    push_stack(&stk_bottom, (w_stack_t)0);            //平衡堆栈的(不用管)
    push_stack(&stk_bottom, (w_stack_t)pfunc);        //线程入口函数 这个函数负责调用线程函数
    push_stack(&stk_bottom, (w_stack_t)5);                //push ebp
    push_stack(&stk_bottom, (w_stack_t)7);                //push edi
    push_stack(&stk_bottom, (w_stack_t)6);                //push esi
    push_stack(&stk_bottom, (w_stack_t)3);                //push ebx
    push_stack(&stk_bottom, (w_stack_t)2);                //push ecx
    push_stack(&stk_bottom, (w_stack_t)1);                //push edx
    push_stack(&stk_bottom, (w_stack_t)0);                //push eax
    return stk_bottom;
}




