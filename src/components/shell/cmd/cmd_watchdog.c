/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_watchdog.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:24:37
** 描        述: 看门狗模块管理命令
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2015/1/24 20:24:37
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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/

#if (WIND_CONSOLE_SUPPORT && CMD_WATCHDOG_SUPPORT)
#include "wind_watchdog.h"
#include "wind_timer.h"

/********************************************内部变量定义**********************************************/




/********************************************内部函数定义*********************************************/
static w_err_t enable_watchdog(w_int32_t argc,char **argv)
{
    w_watchdog_s *watchdog;
    w_timer_s *timer;
    if(argc == 2)
    {
        timer = wind_timer_get("watchdog");
        return wind_timer_stop(timer);
    }
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    watchdog = wind_watchdog_get(argv[3]);
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_INVALID);
    SET_F_WATCHDOG_ENABLE(watchdog);
    return W_ERR_OK;
}

static w_err_t disable_watchdog(w_int32_t argc,char **argv)
{
    w_watchdog_s *watchdog;
    w_timer_s *timer;
    if(argc == 2)
    {
        timer = wind_timer_get("watchdog");
        return wind_timer_start(timer);
    }
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    watchdog = wind_watchdog_get(argv[3]);
    WIND_ASSERT_RETURN(watchdog != W_NULL,W_ERR_INVALID);
    CLR_F_WATCHDOG_ENABLE(watchdog);
    return W_ERR_OK;
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
COMMAND_DISC(wdog)
{
    wind_printf("to manage watchdog module status.\r\n");
}

COMMAND_USAGE(wdog)
{
    wind_printf("wdog list:to enable a watchdog function.\r\n");
    wind_printf("wdog enable:to enable all watchdog function.\r\n");
    wind_printf("wdog disable:to disable all watchdog function.\r\n");
    wind_printf("wdog enable <wdogname>:to enable a watchdog function.\r\n");
    wind_printf("wdog disable <wdogname>:to disable a watchdog function.\r\n");
}

COMMAND_MAIN(wdog,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
        return wind_watchdog_print();
    else if(wind_strcmp(argv[1],"enable") == 0)
        return enable_watchdog(argc,argv);
    else if(wind_strcmp(argv[1],"disable") == 0)
        return disable_watchdog(argc,argv);
    return W_ERR_FAIL;
}

COMMAND_DEF(wdog);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
