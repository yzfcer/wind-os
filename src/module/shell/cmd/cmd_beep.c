/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_bee.c
** Author      : Jason Zhou
** Last Date   : 2013.10.19
** Description : 蜂鸣器开关命令
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.10.19
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.10.19
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_chdev.h"
#if (CMD_BEEP_SUPPORT)


COMMAND_DISC(beep)
{
    wind_printf("control the beep device.\r\n");
}

COMMAND_USAGE(beep)
{
    wind_printf("beep on:--to open the beep device.\r\n");
    wind_printf("beep off:--to close the beep device.\r\n");
}

COMMAND_MAIN(beep,argc,argv)
{
    w_chdev_s *dev;
    w_err_t err;
    w_uint8_t stat;
    if(argc < 2)
        return W_ERR_FAIL;
    dev = wind_chdev_get("beep");
    WIND_ASSERT_RETURN(dev != W_NULL,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"on"))
    {
        err = wind_chdev_open(dev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 1;
        wind_chdev_write(dev,&stat,1);
        return W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"off"))
    {
        err = wind_chdev_open(dev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 0;
        wind_chdev_write(dev,&stat,1);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(beep);

#endif

