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
#include <Windows.h>                //需要包含该头文件

void wind_init_hook(void)
{
}

/*
 Hook function of function initialization after the device enters multithreading mode. 
 In order to ensure the stability of tick interrupt, generally configure the system tick 
 interrupt to be executed when the system enters the thread state. Before entering the 
 thread, do not let tick trigger the interrupt level thread switch.
 */
void wind_enter_thread_hook(void)
{
}


/*
 To trigger the restart of CPU device, general MCU has the function of software triggering 
 the restart, which can be realized in this function. This function is not necessary. 
 If it is unnecessary, it can be directly vacant. 
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
 When the system needs to support the memory heap function, it needs the starting 
 address of the memory heap and the size of the pair. wind-os can support the creation 
 of multiple discontinuous memory heaps, and can apply for a piece of space in the 
 memory pair to create a nested memory heap for some specific purposes
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
#include "wind_fs.h"
#include "wind_file.h"
static w_err_t create_dirs(void)
{
    w_err_t err;
    w_file_s *file;
    do
    {
        err = W_ERR_OK;
        file = wind_fopen("/usr/",FMODE_R | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"create dir %s fail","/usr/");
        wind_fclose(file);
        file = wind_fopen("/mnt/",FMODE_R | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"create dir %s fail","/mnt/");
        wind_fclose(file);
        file = wind_fopen("/var/",FMODE_R | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"create dir %s fail","/var/");
        wind_fclose(file);
        file = wind_fopen("/host/",FMODE_R | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"create dir %s fail","/host/");
        wind_fclose(file);
    }while(0);
    return err;
}
/*
 When the system needs to support the file system function, it needs to initialize 
 the rule of Mount here. When the file system is not needed, it need not be implemented
 */ 
void _wind_fs_mount_init(void)
{
    w_err_t err;
    wind_vfs_mount("fs0","listfs","disk0","/");
    err = create_dirs();
    WIND_ASSERT_RETURN_VOID(err == W_ERR_OK);
    wind_vfs_mount("fs1","treefs","null0","/var");
    wind_vfs_mount("fs2","treefs","null1","/mnt/");
    wind_vfs_mount("fs3","hostfs","C:/hostfs/","/host");
}
#endif

//push into stack
static void push_stack(w_stack_t ** Stackpp, w_stack_t v)
{
    *Stackpp -= 1;//esp - 4
    **Stackpp = v;//
    return;
}

/*
 Initialization thread stack is used to take initialization parameters from the 
 stack when the thread is switched for the first time. The order of filling in the 
 parameters can refer to the order of CPU threads entering and leaving the stack 
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




