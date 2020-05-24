/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: module_shell.c
** ** Author      : Jason Zhou
** Last Date: 2015/1/24 20:24:37
** Description : shell模块入口
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#include "wind_daemon.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_SHELL_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/

static w_stack_t ctrlstk[THREAD_SHELL_STKSIZE];//主任务堆栈
extern w_err_t thread_shell(w_int32_t argc,char **argv);

w_err_t _create_thread_shell(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("shell",thread_shell,
               0,W_NULL,ctrlstk,THREAD_SHELL_STKSIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_setflag(thread, F_THREAD_DAEMON | F_THREAD_SYSTEM);
    wind_thread_set_priority(thread,32760);
#if WIND_DAEMON_SUPPORT
    if(wind_daemon_get("shell") == W_NULL)
        wind_daemon_create("shell",_create_thread_shell);
#endif
    return W_ERR_OK;
}


MODULE_INIT(shell)
{
    wind_cmd_init();
    return _create_thread_shell();
}

MODULE_EXIT(shell)
{
    return W_ERR_OK;
}

MODULE_DEF(shell, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
