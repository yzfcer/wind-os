/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_echo.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令show处理函数，输出一些系统内部信息
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
#include "wind_types.h"
#include "console_framework.h"
#include "wind_err.h"
#include "wind_debug.h"
#include "wind_mem.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_sem.h"
#include "wind_list.h"
#include "wind_var.h"

#include "console_framework.h"
cmd_s g_cmd_coreobj[];
w_err_t wind_output_cmdlist(void)
{
    pcmd_s clist = wind_get_cmdlist();
    while(clist)
    {
        wind_printf("cmd list:%s\r\n",clist->cmd);
        clist = clist->next;
    }
    return 0;
}


w_err_t cmd_coreobj_main(w_int32_t argc,char **argv)
{
    pnode_s pnode;
    cmd_s *cmd = g_cmd_coreobj;
    
    if(argc < 2)
    {
        wind_printf(cmd->helpdetails);
        return ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"thread"))
    {
        //pnode = g_core.threadlist.head;
        wind_thread_print(&g_core.threadlist);
        return ERR_OK;
    }
    else if(0 == wind_strcmp(argv[1],"sem"))
    {
        //pnode = g_core.semlist.head;
        wind_sem_print(&g_core.semlist);
        return ERR_COMMAN;
    }
    return ERR_COMMAN;
}


cmd_s g_cmd_coreobj[] = 
{
    {
        NULL,
        "coreobj",
        "show core objects infomation.",
        "coreobj thread:to show thread infomation",
        cmd_coreobj_main
    }
};

void register_cmd_show(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_coreobj,sizeof(g_cmd_coreobj)/sizeof(cmd_s));
}

