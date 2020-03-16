/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_led.c
**创   建   人: Jason Zhou
**最后修改日期: 2013.10.19
**描        述: LED灯操作命令
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_chdev.h"
#if (CMD_LED_SUPPORT)

COMMAND_DISC(led)
{
    wind_printf("to control a led device.\r\n");
}

COMMAND_USAGE(led)
{
    wind_printf("led on <ledname>:--to open a led device.\r\n");
    wind_printf("led off <ledname>:--to close a led device.\r\n");
}

COMMAND_MAIN(led,argc,argv)
{
    w_err_t err;
    w_chdev_s *led;
    w_uint8_t stat;
    WIND_ASSERT_RETURN(argc == 3,W_ERR_INVALID);
    led = wind_chdev_get(argv[2]);
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

