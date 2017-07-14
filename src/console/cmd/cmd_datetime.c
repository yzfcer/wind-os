/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_datetime.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令date和time处理函数，处理系统的日期和时间命令
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
    extern "C" {
#endif // #ifdef __cplusplus

#include "wind_config.h"
#include "wind_types.h"
#include "console_framework.h"
#include "wind_err.h"
#include "wind_string.h"
#include "wind_mem.h"
#include "wind_debug.h"
#include "wind_rtc.h"
#include "wind_comman.h"
#if WIND_RTC_SUPPORT > 0

extern datetime_s G_DATETIME;//当前日期和时间

w_err_t cmd_setdate_main(w_int32_t argc,char **argv)
{
    
    //pdate_s pd;
    datetime_s *dt;

    dt = &G_DATETIME;
    if(wind_strlen(argv[0]) >= 10)
    {
        wind_set_date(wind_convert_str2u32_t(&argv[0][0]),
                        wind_convert_str2u32_t(&argv[0][4]),
                        wind_convert_str2u32_t(&argv[0][7]));
        CONSOLE_OUT("system date:%d/%d/%d\r\n",dt->date.year,
        dt->date.month,dt->date.day);
    }    
    return ERR_OK;
}

w_err_t cmd_settime_main(w_int32_t argc,char **argv)
{
    //ptime_s pt;
    datetime_s *dt;
    w_int8_t *cmdstr;
    
    dt = &G_DATETIME;
    if(wind_strlen(cmdstr) >= 15)
    {
        wind_set_time(wind_convert_str2u32_t(&(argv[0][0])),
                        wind_convert_str2u32_t(&(argv[0][3])),
                        wind_convert_str2u32_t(&(argv[0][6])),0);
        CONSOLE_OUT("system time:%d:%d:%d  %d\r\n",dt->time.hour,
        dt->time.minute,dt->time.second,dt->time.msecond);
    }
    
    return ERR_OK;
}

w_err_t cmd_showdatetime_main(w_int32_t argc,char **argv)
{
    
    pdate_s pd;
    //datetime_s *dt;

    //dt = &G_DATETIME;
    pd = wind_get_date();
    CONSOLE_OUT("system datetime:%d/%d/%d \r\n",pd->year,
    pd->month,pd->day);
  
    return ERR_OK;
}



cmd_s g_cmd_datetime[] =
{
    {
        NULL,"datetime set date","to set the system date info.",
        "system date format:YYYY/MM/DD",
        cmd_setdate_main,
    },

    {
        NULL,"datetime set time","to set the system time info.",
        "system time format:HH:mm:SS",
        cmd_settime_main,
    }, 
    
    {
        NULL,"datetime show detail","to show the system date info.",
        "",cmd_showdatetime_main,
    },
};

#endif

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

