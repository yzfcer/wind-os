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
#include "wind_string.h"
#include <stdlib.h>

#include <ucontext.h>
#include <signal.h>
#include <stdio.h>


/*
 Hook function of function initialization after the device enters multithreading mode. 
 In order to ensure the stability of tick interrupt, generally configure the system tick 
 interrupt to be executed when the system enters the thread state. Before entering the 
 thread, do not let tick trigger the interrupt level thread switch.
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
 To trigger the restart of CPU device, general MCU has the function of software triggering 
 the restart, which can be realized in this function. This function is not necessary. 
 If it is unnecessary, it can be directly vacant. 
 */

void wind_system_reset(void)
{
    extern char *argv_bak;
    if(argv_bak != W_NULL)
        system(argv_bak);
}



#if WIND_HEAP_SUPPORT
/*
 When the system needs to support the memory heap function, it needs the starting 
 address of the memory heap and the size of the pair. wind-os can support the creation 
 of multiple discontinuous memory heaps, and can apply for a piece of space in the 
 memory pair to create a nested memory heap for some specific purposes
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
		wind_notice("file:%p",file);
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
        file = wind_fopen("/mem/",FMODE_R | FMODE_CRT);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"create dir %s fail","/mem/");
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
    wind_vfs_mount("fs1","listfs","memblk","/mem");
    wind_vfs_mount("fs2","treefs","null0","/var");
    wind_vfs_mount("fs3","treefs","null1","/mnt/");
    wind_vfs_mount("fs4","hostfs","/home/jason/wfs/","/host/");
}
#endif

#if WIND_MODULE_NET_SUPPORT
/*
 When the system needs to support the net stack function, it needs to register 
 net devices into the ssytem here
 */
#include "wind_netdev.h"
void wind_netdevs_register(void)
{
    //wind_vfs_mount("fs0","treefs","null","/");
}
#endif



/*
 Initialization thread stack is used to take initialization parameters from the 
 stack when the thread is switched for the first time. The order of filling in the 
 parameters can refer to the order of CPU threads entering and leaving the stack 
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

