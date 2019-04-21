/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_thread.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/9/14 21:24:37
** 描        述: 线程基本操作命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/9/14 21:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_thread.h"
#include "wind_conv.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (CMD_THREAD_SUPPORT)

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(thread)
{
    wind_printf("to control thread status.\r\n");
}

COMMAND_USAGE(thread)
{
    wind_printf("thread list:--to show all thread infomation.\r\n");
    wind_printf("thread start <threadname>:--to start a thread.\r\n");
    wind_printf("thread suspend <threadname>:--to suspend a thread.\r\n");
    wind_printf("thread kill <threadname>:--to kill a thread.\r\n");
    wind_printf("thread setprio <threadname> <prio>:--to change a thread priority.\r\n");
}

COMMAND_MAIN(thread,argc,argv)
{
    w_bool_t res;
    w_int32_t prio;
    w_thread_s *thread;
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
    {
        wind_thread_print();
        return W_ERR_OK;
    }
	thread = wind_thread_get(argv[2]);
    if(thread == W_NULL)
    {
        wind_error("has no such a thread:%s",argv[2]);
        return W_ERR_INVALID;
    }
    if(wind_strcmp(argv[1],"start") == 0)
    {
        wind_thread_start(thread);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"suspend") == 0)
    {
        wind_thread_suspend(thread);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"kill") == 0)
    {
        wind_thread_destroy(thread);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"setprio") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 4,W_ERR_INVALID);
        res = wind_str_to_int(argv[3],&prio);
        if(res != W_TRUE)
            return W_ERR_INVALID;
        WIND_ASSERT_RETURN(prio > 0,W_ERR_INVALID);
        WIND_ASSERT_RETURN(prio <= 0x7fff,W_ERR_INVALID);
        wind_thread_set_priority(thread,(w_int16_t)prio);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(thread);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
