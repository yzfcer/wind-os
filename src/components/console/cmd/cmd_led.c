/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_led.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: LED等操作命令
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
#include <stdio.h>
#include "wind_cmd.h"
#include "wind_chdev.h"
#if (WIND_CONSOLE_SUPPORT && CMD_LED_SUPPORT)

COMMAND_DISC(led)
{
    console_printf("control a led device.\r\n");
}

COMMAND_USAGE(led)
{
    console_printf("led on <ledindex>:to open a led device.\r\n");
    console_printf("led off <ledindex>:to close a led device.\r\n");
}

COMMAND_MAIN(led,argc,argv)
{
    w_err_t err;
    w_chdev_s *led;
    w_uint8_t stat;
    char devname[8];
    WIND_ASSERT_RETURN(argc == 3,W_ERR_INVALID);
    wind_memset(devname,0,sizeof(devname));
    sprintf(devname,"led%s",argv[2]);
    led = wind_chdev_get(devname);
    WIND_ASSERT_RETURN(led != W_NULL,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"on"))
    {
        err = wind_chdev_open(led);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 1;
        wind_chdev_write(led,&stat,1);
        return W_ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"off"))
    {
        err = wind_chdev_open(led);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
        stat = 0;
        wind_chdev_write(led,&stat,1);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(led);

#endif

