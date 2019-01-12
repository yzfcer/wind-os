/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_echo.c
**创   建   人: Jason Zhou
**最后修改日期: 2013.10.19
**描        述: 系统的内存池，内存堆和CPU占用率等统计信息查看命令
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
#include "wind_string.h"
#include "wind_stati.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_heap.h"

#if (WIND_CONSOLE_SUPPORT && CMD_STATI_SUPPORT)






w_err_t cmd_stat_show_pool_main(w_int32_t argc,char **argv)
{
    wind_pool_stati_print();
    return W_ERR_OK;
}

#if WIND_HEAP_SUPPORT
w_err_t cmd_stat_show_heap_main(w_int32_t argc,char **argv)
{
    wind_heap_stati_print();
    return W_ERR_OK;
}
#endif

w_err_t cmd_stat_show_cpuusage_main(w_int32_t argc,char **argv)
{
    console_printf("cpu usage persage:%d%%\r\n",WIND_CPU_USAGE);
    return W_ERR_OK;    
}


COMMAND_DISC(stati)
{
    console_printf("show some statistics infomation.\r\n");
}

COMMAND_USAGE(stati)
{
    console_printf("stati pool:to show system pools statistics info.\r\n");
#if WIND_HEAP_SUPPORT
    console_printf("stati heap:to show system heaps statistics info.\r\n");
#endif
    console_printf("stati cpuusage:to show current cpu usage persent.\r\n");
}

COMMAND_MAIN(stati,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"pool"))
    {
        return cmd_stat_show_pool_main(argc,argv);
    }
#if WIND_HEAP_SUPPORT
    if(0 == wind_strcmp(argv[1],"heap"))
    {
        return cmd_stat_show_heap_main(argc,argv);
    }
#endif
    else if(0 == wind_strcmp(argv[1],"cpuusage"))
    {
        return cmd_stat_show_cpuusage_main(argc,argv);
    }
    return W_ERR_FAIL;
}

COMMAND_DEF(stati);

#endif

