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
#include "wind_list.h"
#include "wind_var.h"

#include "console_framework.h"

err_t wind_output_cmdlist(void)
{
    pcmd_s clist = wind_get_cmdlist();
    while(clist)
    {
        CONSOLE_OUT("cmd list:%s\r\n",clist->cmd);
        clist = clist->next;
    }
    return 0;
}


err_t cmd_show_main(s32_t argc,char **argv)
{
    pnode_s pnode;
    pthread_s pthread;

    if(is_string_equal(argv[0],"-c"))
    {
        CONSOLE_OUT("cpu usage persage:%%%d\r\n",WIND_CPU_USAGE);
        return ERR_OK;
    }
    else if(is_string_equal(argv[0],"proc"))
    {
        pnode = g_core.pcblist.head;
        if(is_string_equal(argv[0],"all"))
        {
            CONSOLE_OUT("proc list as following:\r\n");
            while(pnode)
            {
                pthread = (pthread_s)pnode->obj;
                CONSOLE_OUT("%s--prio:%d\r\n",pthread->name,pthread->prio);
                pnode = pnode->next;
            }
            
        }
        else
        {
            while(pnode)
            {
                pthread = (pthread_s)pnode->obj;
                if(is_string_equal(argv[1],pthread->name))
                {
                    CONSOLE_OUT("proc:%s\r\n",pthread->name);
                    CONSOLE_OUT("stacksize:%d\r\n",pthread->stksize);
                    CONSOLE_OUT("prio:%d\r\n",pthread->prio);
                    switch(pthread->proc_status)
                    {
                    case PROC_STATUS_READY:
                        CONSOLE_OUT("status:ready\r\n",pthread->stksize);
                        break;
                    case PROC_STATUS_SUSPEND:
                        CONSOLE_OUT("status:suspend\r\n",pthread->stksize);
                        break;
                    case PROC_STATUS_SLEEP:
                        CONSOLE_OUT("status:sleep\r\n",pthread->stksize);
                        break;
                    case PROC_STATUS_DEAD:
                        CONSOLE_OUT("status:dead\r\n",pthread->stksize);
                        break;
                    default:
                        CONSOLE_OUT("status:unknown\r\n",pthread->stksize);
                        break;
                    }
                }
                pnode = pnode->next;
            }
        }
        return ERR_OK;
    }
    else
    {
        return ERR_COMMAN;
    }
}


cmd_s g_cmd_show[] = 
{
    {
        NULL,
        "show",
        "show some info you want to known.",
        "",
        cmd_show_main
    }
};

void register_cmd_show(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_show,sizeof(g_cmd_show)/sizeof(cmd_s));
}

