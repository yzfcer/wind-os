/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: cmd_datetime.c
**Author: Jason Zhou
**Last Date: 2013.10.19
**Description: 处理系统的日期和时间命令
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2013.10.19
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2013.10.19
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_string.h"
#include "wind_cmd.h"
#include "wind_time.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if (CMD_DATETIME_SUPPORT)

static w_err_t cmd_set_datetime(w_int32_t argc,char **argv)
{
    w_int32_t cnt;
    datetime_s dt;
    if(argc < 4)
    {
        wind_printf("error:parameter is NOT enough.\r\n");
        return W_ERR_INVALID;
    }
    if(wind_strlen(argv[2]) < 10)
    {
        wind_printf("date format error.\r\n");
        return W_ERR_INVALID;
    }
    if(wind_strlen(argv[3]) < 8)
    {
        wind_printf("time format error.\r\n");
        return W_ERR_INVALID;
    }
    
    cnt = wind_sscanf(argv[2],"%4d/%02d/%02d",&dt.date.year,
                (w_uint32_t*)&dt.date.month,(w_uint32_t*)&dt.date.day);
    WIND_ASSERT_RETURN(cnt >= 3,W_ERR_INVALID);
    cnt = wind_sscanf(argv[3],"%2d:%2d:%2d",(w_uint32_t*)&dt.time.hour,
                (w_uint32_t*)&dt.time.minute,(w_uint32_t*)&dt.time.second);
    WIND_ASSERT_RETURN(cnt >= 3,W_ERR_INVALID);
    dt.time.msecond = 0;
    wind_datetime_set(&dt);
    wind_printf("system date:%4d/%02d/%02d %02d:%02d:%02d %4d\r\n",dt.date.year,
                dt.date.month,dt.date.day,dt.time.hour,
                dt.time.minute,dt.time.second,dt.time.msecond);
    return W_ERR_OK;
}


static w_err_t cmd_showdatetime(w_int32_t argc,char **argv)
{
    w_err_t err;
    datetime_s dt;
    err = wind_datetime_get(&dt);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    wind_printf("system date:%4d/%02d/%02d %02d:%02d:%02d  %4d\r\n",dt.date.year,
                dt.date.month,dt.date.day,dt.time.hour,
                dt.time.minute,dt.time.second,dt.time.msecond);
    return W_ERR_OK;
}



COMMAND_DISC(datetime)
{
    wind_printf("to set or show system date and time.\r\n");
}

COMMAND_USAGE(datetime)
{
    wind_printf("datetime:--to show the system date and time infomation.\r\n");
    wind_printf("datetime set <datetime>:--to set the system date infomation.format:YYYY/MM/DD HH:mm:SS\r\n");
}

COMMAND_MAIN(datetime,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(argc == 1)
    {
        return cmd_showdatetime(argc,argv);
    }
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"set") == 0)
    {
        return cmd_set_datetime(argc,argv);
    }
    else
    {
        wind_printf("datetime:format error.\r\n");
        return W_ERR_FAIL;
    }
}

COMMAND_DEF(datetime);


#endif

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

