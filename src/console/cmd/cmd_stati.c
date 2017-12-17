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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_mem.h"
#include "wind_string.h"
#include "wind_stati.h"
#include "wind_var.h"
#include "wind_cmd.h"
#if WIND_CONSOLE_SUPPORT

static void core_stati_print(w_uint16_t opt)
{
    dnode_s *dnode;
    pool_s *pool;
    dlist_s *list = &g_core.poollist;
    wind_printf("-------------------------------------------------\r\n");
    wind_printf("%-16s%-8s%-8s%-8s%-8s\r\n","pool","tot","used","maxused","err");
    wind_printf("-------------------------------------------------\r\n");
    foreach_node(dnode,list)
    {
        pool = (pool_s*)DLIST_OBJ(dnode,pool_s,poolnode);
        wind_printf("%-16s%-8d%-8d%-8d%-8d\r\n",pool->name,pool->stati.tot,
            pool->stati.used,pool->stati.max,pool->stati.err);
    }
    wind_printf("-------------------------------------------------\r\n");
}




w_err_t cmd_stat_show_mpool_main(w_int32_t argc,char **argv)
{
    core_stati_print(0xffff);
    return ERR_OK;
}

w_err_t cmd_stat_show_cpuusage_main(w_int32_t argc,char **argv)
{
    console_printf("cpu usage persage:%d%%\r\n",WIND_CPU_USAGE);
    return ERR_OK;    
}


static void cmd_showdisc(void)
{
    console_printf("show some statistics infomation.\r\n");
}

static void cmd_showusage(void)
{
    console_printf("stati showpool:to show system pools usage info.\r\n");
    console_printf("stati cpuusage:to show current cpu usage persent.\r\n");
}

static w_err_t cmd_main(w_int32_t argc,char **argv)
{
    if(argc < 2)
    {
        cmd_showusage();
        return ERR_OK;
    }
    if(0 == wind_strcmp(argv[1],"showpool"))
    {
        return cmd_stat_show_mpool_main(argc,argv);
    }
    else if(0 == wind_strcmp(argv[1],"cpuusage"))
    {
        return cmd_stat_show_cpuusage_main(argc,argv);
    }
    return ERR_COMMAN;
}

CMD_DEF(stati);

#endif

