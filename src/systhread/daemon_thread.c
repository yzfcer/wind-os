/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: daemon_thread.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统监视线程，监控一些可能出现异常的系统状态
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "wind_thread.h"
#include "wind_debug.h"
//#include "wind_watchdog.h"

#if WIND_DAEMON_THREAD_SUPPORT 
#define DAEMON_STK_SIZE 256
static w_stack_t daemonstk[DAEMON_STK_SIZE];


static w_err_t daemon_thread(w_int32_t argc,w_int8_t **argv)
{
    (void)argc;
    //wind_watchdog_create("daemon",0,10);
	
    while(1)
    {
        wind_thread_sleep(1000);
    }
}

w_err_t _create_daemon_thread(void)
{
    thread_s *thread;
    thread = wind_thread_create("daemon",daemon_thread,
                     0,NULL,PRIO_HIGH,daemonstk,DAEMON_STK_SIZE);
    WIND_ASSERT_RETURN(thread != NULL,ERR_FAIL);
    return ERR_OK;
}

#endif

