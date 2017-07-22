/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: cmd_help.h / cmd_help.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 20:52:30
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/1/24 20:52:30
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 帮助菜单命令定义函数
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
//#include "wind_config.h"
//#include "wind_types.h"
#include "console_framework.h"



/********************************************内部变量定义**********************************************/
	#if 0
static const char * const help_str = "help [opt]:to show some helping infomation\r\n";
static const char * const help_details[] = 
{
    "help:to show list of commands\r\n",
    "help [cmd]:to get more info of the command\r\n"
};
#endif

/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

static w_err_t cmd_help_main(w_int32_t argc,char **argv)
{
    cmd_s *cmdlist = wind_get_cmdlist();
    if(argc < 1)
    {
        wind_printf("\r\ncommand list as following:\r\n");
        while(cmdlist)
        {
            wind_printf("%s : %s\r\n",cmdlist->cmd,cmdlist->helpdetails);
            cmdlist = cmdlist->next;
        }
        return ERR_OK;
    }
    while(cmdlist)
    {
        if(wind_strcmp(argv[0],cmdlist->cmd) == 0)
        {
            wind_printf("%s : %s\r\n",cmdlist->cmd,cmdlist->helpdetails);
            break;
        }
        cmdlist = cmdlist->next;
    }
    return ERR_OK;
}


cmd_s g_cmd_help[] = 
{
    {
        NULL,
        "help",
        "get help infomation.",
        "",
        cmd_help_main
    }
};

void register_cmd_help(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_help,sizeof(g_cmd_help)/sizeof(cmd_s));
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
