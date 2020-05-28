/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_stati.c
** Author      : Jason Zhou
** Last Date   : 2013.10.19
** Description : System memory pool, memory heap, CPU usage and other statistics viewing command
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
#include "wind_string.h"
#include "wind_stati.h"
#include "wind_core.h"
#include "wind_pool.h"
#include "wind_heap.h"

#if (CMD_STATI_SUPPORT)

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
    w_core_var_s core_var;
    wind_get_core_var(&core_var);
    wind_printf("cpu usage persage:%d%%\r\n",core_var.cpu_usage);
    return W_ERR_OK;    
}


COMMAND_DISC(stati)
{
    wind_printf("to show system statistics infomation.\r\n");
}

COMMAND_USAGE(stati)
{
    wind_printf("stati pool:--to show system pools statistics info.\r\n");
#if WIND_HEAP_SUPPORT
    wind_printf("stati heap:--to show system heaps statistics info.\r\n");
#endif
    wind_printf("stati cpuusage:--to show current cpu usage persent.\r\n");
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

