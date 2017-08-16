/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_proc.c
** 创   建   人: 周江村
** 最后修改日期: 2015/9/29 22:28:32
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/9/29 22:28:32
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 与proc相关的命令处理函数
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "wind_var.h"
#include "console_framework.h"
#include "wind_thread.h"

/********************************************内部变量定义**********************************************/



/********************************************内部函数定义*********************************************/
static w_err_t wind_thread_output(pthread_s pthread)
{
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    wind_printf("proc name:%s\r\n",pthread->name);
    wind_printf("proc stack:%d\r\n",pthread->pstk);
    wind_printf("proc prio:0x%x\r\n\r\n",pthread->prio);
    wind_printf("proc state:%d\r\n",pthread->runstat);
    wind_printf("proc stack size:%d\r\n",pthread->stksize);
    return ERR_OK;
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/


w_err_t cmd_proc_show_list_main(w_int32_t argc,char **argv)
{
    pnode_s node = g_core.threadlist.head;
    pthread_s pthread;
    
    WIND_DEBUG("\r\n\r\nprocess list as following:\r\n");
    while(node != NULL)
    {
        pthread = (pthread_s)node->obj;
        wind_thread_output(pthread);
        node = node->next;
    }
    return ERR_OK;
}

w_err_t cmd_proc_show_obj_main(w_int32_t argc,char **argv)
{
    pthread_s pthread = wind_thread_get_byname(argv[0]);
    WIND_ASSERT_RETURN(pthread != NULL,ERR_NULL_POINTER);
    return wind_thread_output(pthread);
}


cmd_s g_cmd_proc[] = 
{
    {
        NULL,"proc showlist","To show all the proc struct.",
        "",cmd_proc_show_list_main
    },
    {
        NULL,"proc showobj","To show a proc struct by name.",
        "proc show obj <name>,name is a proc name provided by creator.",
        cmd_proc_show_obj_main
    }
};

void register_cmd_proc(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_proc,sizeof(g_cmd_proc)/sizeof(cmd_s));
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
