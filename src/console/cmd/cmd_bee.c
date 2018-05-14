/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_bee.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令stati处理函数，统计资源使用情况
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
#include "wind_cmd.h"
#include "beep.h"
#if WIND_CONSOLE_SUPPORT
#if WIND_DRVFRAME_SUPPORT


COMMAND_DISC(bee)
{
    console_printf("control the bee device.\r\n");
}

COMMAND_USAGE(bee)
{
    console_printf("bee on:to open the bee device.\r\n");
    console_printf("bee off:to close the bee device.\r\n");
}

COMMAND_MAIN(bee,argc,argv)
{
    if(0 == wind_strcmp(argv[1],"on"))
    {
        BEEP = 1;
        return ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"off"))
    {
        BEEP = 0;
        return ERR_OK;
    }
    return ERR_FAIL;
}

COMMAND_DEF(bee);

#endif
#endif

