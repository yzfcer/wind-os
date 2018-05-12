/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_echo.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令stat处理函数，统计资源使用情况
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
#include "wind_string.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_pool.h"
#include "wind_heap.h"

#if WIND_CONSOLE_SUPPORT

static void core_stati_print(w_uint16_t opt)
{
    dnode_s *dnode;
    pool_s *pool;
    heap_s *heap;
    dlist_s *list = &g_core.poollist;
    wind_printf("-------------------------------------------------\r\n");
    wind_printf("%-16s%-8s%-8s%-8s%-8s\r\n","pool","tot","used","maxused","err");
    wind_printf("-------------------------------------------------\r\n");
    if(opt == 1)
    {
        foreach_node(dnode,list)
        {
            pool = (pool_s*)DLIST_OBJ(dnode,pool_s,poolnode);
            wind_printf("%-16s%-8d%-8d%-8d%-8d\r\n",pool->name,pool->stati.tot,
                pool->stati.used,pool->stati.max,pool->stati.err);
        }
    }
    else if(opt == 2)
    {
        foreach_node(dnode,list)
        {
            heap = (heap_s*)DLIST_OBJ(dnode,heap_s,heapnode);
            wind_printf("%-16s%-8d%-8d%-8d%-8d\r\n",heap->name,heap->stati.tot,
                heap->stati.used,heap->stati.max,heap->stati.err);
        }
    }
    wind_printf("-------------------------------------------------\r\n");
}




w_err_t cmd_stat_show_pool_main(w_int32_t argc,char **argv)
{
    core_stati_print(1);
    return ERR_OK;
}

w_err_t cmd_stat_show_heap_main(w_int32_t argc,char **argv)
{
    core_stati_print(2);
    return ERR_OK;
}


w_err_t cmd_stat_show_cpuusage_main(w_int32_t argc,char **argv)
{
    console_printf("cpu usage persage:%d%%\r\n",WIND_CPU_USAGE);
    return ERR_OK;    
}


COMMAND_DISC(stati)
{
    console_printf("show some statistics infomation.\r\n");
}

COMMAND_USAGE(stati)
{
    console_printf("stati pool:to show system pools statistics info.\r\n");
    console_printf("stati heap:to show system heaps statistics info.\r\n");
    console_printf("stati cpuusage:to show current cpu usage persent.\r\n");
}

COMMAND_MAIN(stati,argc,argv)
{
    if(0 == wind_strcmp(argv[1],"pool"))
    {
        return cmd_stat_show_pool_main(argc,argv);
    }
    if(0 == wind_strcmp(argv[1],"heap"))
    {
        return cmd_stat_show_pool_main(argc,argv);
    }
    else if(0 == wind_strcmp(argv[1],"cpuusage"))
    {
        return cmd_stat_show_cpuusage_main(argc,argv);
    }
    return ERR_FAIL;
}

COMMAND_DEF(stati);

#endif

