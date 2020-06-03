/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : cmd_log.c
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2020/05/23
** Description : wind-s log command function
** Others      : 
** Modify      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#include "wind_cmd.h"
#include "wind_log.h"
#include "wind_conv.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (CMD_LOG_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/
COMMAND_DISC(log)
{
    wind_printf("to set or show log infomation.\r\n");
}

COMMAND_USAGE(log)
{
    wind_printf("log setlevel <level>:--to set log level.\r\n");
    wind_printf("log showlevel :--to show log level.\r\n");
    wind_printf("log info:--to show log level infomation.\r\n");
}

COMMAND_MAIN(log,argc,argv)
{
    w_int32_t i;
    w_int32_t level,oldlevel;
    
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"setlevel") == 0)
    {
        WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
        oldlevel = wind_log_get_level();
        if(wind_str_to_int(argv[2],(w_int32_t*)&level))
        {
            wind_log_set_level(level);
            level = wind_log_get_level();
            wind_printf("set log leve:%d ==> %d",oldlevel,level);
        }
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"showlevel") == 0)
    {
        level = wind_log_get_level();
        wind_log_print_level(level);
        return W_ERR_OK;
    }
    else if(wind_strcmp(argv[1],"info") == 0)
    {
        wind_printf("log levels:\r\n");
        for(i = WIND_LOG_CRIT;i <= WIND_LOG_DEBUG;i ++)
            wind_log_print_level(i);
        return W_ERR_OK;
    }
    return W_ERR_OK;
}

COMMAND_DEF(log);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
