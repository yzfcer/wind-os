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

w_err_t wind_output_cmdlist(void)
{
    pcmd_s clist = wind_get_cmdlist();
    while(clist)
    {
        CONSOLE_PRINTF("cmd list:%s\r\n",clist->cmd);
        clist = clist->next;
    }
    return 0;
}


w_err_t cmd_show_main(w_int32_t argc,char **argv)
{
    pnode_s pnode;
    pthread_s pthread;

    if(0 == wind_strcmp(argv[0],"-c"))
    {
        CONSOLE_PRINTF("cpu usage persage:%%%d\r\n",WIND_CPU_USAGE);
        return ERR_OK;
    }
    else if(0 == wind_strcmp(argv[0],"proc"))
    {
        pnode = g_core.pcblist.head;
        if(0 == wind_strcmp(argv[0],"all"))
        {
            CONSOLE_PRINTF("proc list as following:\r\n");
            while(pnode)
            {
                pthread = (pthread_s)pnode->obj;
                CONSOLE_PRINTF("%s--prio:%d\r\n",pthread->name,pthread->prio);
                pnode = pnode->next;
            }
            
        }
        else
        {
            while(pnode)
            {
                pthread = (pthread_s)pnode->obj;
                if(0 == wind_strcmp(argv[1],pthread->name))
                {
                    CONSOLE_PRINTF("proc:%s\r\n",pthread->name);
                    CONSOLE_PRINTF("stacksize:%d\r\n",pthread->stksize);
                    CONSOLE_PRINTF("prio:%d\r\n",pthread->prio);
                    switch(pthread->runstat)
                    {
                    case THREAD_STATUS_READY:
                        CONSOLE_PRINTF("status:ready\r\n",pthread->stksize);
                        break;
                    case THREAD_STATUS_SUSPEND:
                        CONSOLE_PRINTF("status:suspend\r\n",pthread->stksize);
                        break;
                    case THREAD_STATUS_SLEEP:
                        CONSOLE_PRINTF("status:sleep\r\n",pthread->stksize);
                        break;
                    case THREAD_STATUS_DEAD:
                        CONSOLE_PRINTF("status:dead\r\n",pthread->stksize);
                        break;
                    default:
                        CONSOLE_PRINTF("status:unknown\r\n",pthread->stksize);
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

