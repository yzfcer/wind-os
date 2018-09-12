/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_echo.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的内存池，内存堆和CPU占用率等统计信息查看命令
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

static void core_stati_pool_print(void)
{
    w_dnode_s *dnode;
    w_pool_s *pool;
    w_dlist_s *list;
    wind_print_space(7);
    wind_printf("%-16s %-8s %-8s %-8s %-8s\r\n","pool","tot","used","maxused","err");
    wind_print_space(7);
    list = &g_core.poollist;
    foreach_node(dnode,list)
    {
        pool = (w_pool_s*)DLIST_OBJ(dnode,w_pool_s,poolnode);
        wind_printf("%-16s %-8d %-8d %-8d %-8d\r\n",pool->name,pool->stati.tot,
            pool->stati.used,pool->stati.max,pool->stati.err);
    }
    wind_print_space(7);
}

#if WIND_HEAP_SUPPORT
static void core_stati_heap_print(void)
{
    w_dnode_s *dnode;
    w_heap_s *heap;
    w_dlist_s *list;
    wind_print_space(7);
    wind_printf("%-16s %-8s %-8s %-8s %-8s\r\n","heap","tot","used","maxused","err");
    wind_print_space(7);

    list = &g_core.heaplist;
    foreach_node(dnode,list)
    {
        heap = (w_heap_s*)DLIST_OBJ(dnode,w_heap_s,heapnode);
        wind_printf("%-16s %-8d %-8d %-8d %-8d\r\n",heap->name,heap->stati.tot,
            heap->stati.used,heap->stati.max,heap->stati.err);
    }
   
    wind_print_space(7);
}
#endif


w_err_t cmd_stat_show_pool_main(w_int32_t argc,char **argv)
{
    core_stati_pool_print();
    return W_ERR_OK;
}

#if WIND_HEAP_SUPPORT
w_err_t cmd_stat_show_heap_main(w_int32_t argc,char **argv)
{
    core_stati_heap_print();
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

