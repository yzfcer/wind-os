/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : cmd_watchdog.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : 看门狗模块管理命令
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
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_WATCHDOG_SUPPORT)
#include "wind_watchdog.h"
#include "wind_timer.h"

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/
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



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(wdog)
{
    wind_printf("to manage watchdog module status.\r\n");
}

COMMAND_USAGE(wdog)
{
    wind_printf("wdog list:--to enable a watchdog function.\r\n");
    wind_printf("wdog enable:--to enable all watchdog function.\r\n");
    wind_printf("wdog disable:--to disable all watchdog function.\r\n");
    wind_printf("wdog enable <wdogname>:--to enable a watchdog function.\r\n");
    wind_printf("wdog disable <wdogname>:--to disable a watchdog function.\r\n");
}

COMMAND_MAIN(wdog,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"list") == 0)
        return wind_watchdog_print_detail();
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
