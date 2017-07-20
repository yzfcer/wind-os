/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: console_framework.h / console_framework.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 20:08:03
** 描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/1/24 20:08:03
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 能够支持多个标准输入输出的控制台的框架
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "console_framework.h"
#include "wind_config.h"
#include "wind_debug.h"
#include "wind_time.h"
#include "wind_var.h"

/********************************************内部变量定义**********************************************/

console_s g_ctrl[WIND_CONSOLE_COUNT];
cmd_list_s *g_cmd_global;
//临时使用的数据读取函数


/********************************************内部函数定义*********************************************/
static w_err_t core_get_cmd_ch(w_int8_t *ch)
{
    *ch = 0;
    return ERR_COMMAN;
}

static void show_cmd_list(void)
{
    cmd_s *cmd = g_cmd_global->head;
    while(cmd)
    {
        WIND_INFO("%s : %s\r\n",cmd->cmd,cmd->helpinfo);
        cmd = cmd->next;
    }
}

static void wind_console_prehandle(console_s *ctrl,char ch)
{
    if(ctrl->stat == CSLSTAT_PWD)
        return;
    else if((ch == WVK_BACKSPACE) && (ctrl->index == 0))
        return;
    else if((ch == WVK_BACKSPACE) && (ctrl->index > 0))
    {
        CONSOLE_OUT("%c",WVK_DEL);
        return;
    }
    if(ch == 0x0d)
    {
        CONSOLE_OUT("\r\n");
    }
    else
        CONSOLE_OUT("%c",ch);
}


static w_int32_t console_read_line(console_s *ctrl,w_int32_t len)
{
    w_err_t err;
    w_int32_t idx = 0;
    char ch;
    while(1)
    {
        err = core_get_cmd_ch(&ch);
        if(err != ERR_OK)
            wind_thread_sleep(20);
        else
        {
            wind_console_prehandle(ctrl,ch);
            if((ctrl->stat != CSLSTAT_PWD) || (ch == '\n'))
                ctrl->ops.printf(&ch,1);
            ctrl->buf[idx] = ch;
            if(ch == '\n')
            {
                ctrl->buf[idx] = 0;
                return idx;
            }
            else
            {
                idx ++;
                if(idx >= len)
                {
                    ctrl->buf[idx- 1] = 0;
                    return idx -1;
                }
            }
        }
    }
}





static void init_console_stat(console_s *ctrl)
{
    ctrl->stat = CSLSTAT_USER;
    ctrl->index = 0;
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    wind_memset(ctrl->user,0,WIND_CTL_USRNAME_LEN);
    wind_memset(ctrl->pwd,0,WIND_CTL_PWD_LEN);
    cmd_history_init(&ctrl->his);
    console_framework_init(ctrl);
    ctrl->ops.printf = wind_printf;
}

int find_param_end(char *str,int offset,int len)
{
    int i;
    for(i = offset;i < len;i ++)
    {
        if(str[i] == ' ' || str[i] == 0)
            return i;
    }
    return i;
}


/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

void console_framework_init(console_s *ctrl)
{
    cmd_list_s *cgl = &ctrl->cmd_list;
    cgl->head = NULL;
    cgl->tail = NULL;
    cgl->cnt = 0;
    register_cmd_echo(ctrl);
    register_cmd_help(ctrl);
    register_cmd_proc(ctrl);
    register_cmd_show(ctrl);
    register_cmd_stat(ctrl);
    register_cmd_test(ctrl);
    g_cmd_global = cgl;
    show_cmd_list();
}

w_err_t wind_cmd_register(cmd_list_s *cgl,cmd_s *cmd,int cnt)
{
    int i;
    cmd_list_s *cg = cgl;

    WIND_ASSERT_RETURN(cmd != NULL,ERR_NULL_POINTER);
    for(i = 0;i < cnt;i ++)
    {
        if(cg->tail == NULL)
        {
            cmd[i].next = NULL;
            cg->tail = &cmd[i];
            cg->head = &cmd[i];
        }
        else
        {
            cmd[i].next = NULL;
            cg->tail->next = &cmd[i];
            cg->tail = &cmd[i];
        }
        cg->cnt ++;
    }
    return ERR_OK;
}

//获取当前系统支持的命令列表
cmd_s *wind_get_cmdlist(void)
{
    return g_cmd_global->head;
}


static w_err_t check_user_name(console_s *ctrl)
{
    if(wind_strcmp(ctrl->buf,"root") != 0)
        return ERR_COMMAN;
    wind_strcpy(ctrl->user,ctrl->buf);
    return ERR_OK;
}


static w_err_t check_user_pwd(console_s *ctrl)
{
    if(wind_strcmp(ctrl->user,"root") != 0)
        return ERR_COMMAN;
    wind_strcpy(ctrl->user,ctrl->buf);
    if(wind_strcmp(ctrl->buf,"wind") != 0)
        return ERR_COMMAN;
    wind_strcpy(ctrl->pwd,ctrl->buf);
    return ERR_OK;
}

static w_int32_t get_string(char *str,w_int32_t idx,char ** arg)
{
    int i;
    while(str[i] && (str[i] == ' '))
        i ++;
    if(str[i] == 0)
        return -1;
    *arg = &str[i];
    while(str[i] && (str[i] == ' '))
        i ++;
    str[i] = 0;
    return 0;
}

static w_err_t spit_cmd(console_s * ctrl)
{
    w_err_t idx = 0,i;
    cmd_param_s *prm = &ctrl->param;
    prm->argc = 0;
    for(i = 0;i < CMD_PARAM_CNT;i ++)
    {
        prm->argv[i] = NULL;
        idx = get_string(ctrl->buf,idx,&prm->argv[i]);
        if(idx > 0)
            prm->argc ++;
        else
            break;
    }
    if(i >= CMD_PARAM_CNT)
        return ERR_COMMAN;
    return ERR_OK;
}

static cmd_s *get_matched_cmd(console_s *ctrl)
{
    cmd_s *cmd = ctrl->cmd_list.head;
    while(cmd)
    {
        if(wind_strcmp(ctrl->param.argv[0],cmd->cmd) == 0)
        {
            return cmd;
        }
        cmd = cmd->next;
    }
    return NULL;
}

static w_err_t execute_cmd(console_s * ctrl)
{
    w_err_t err;
    cmd_s *cmd;
    err = spit_cmd(ctrl);
    if(err < 0)
        return err;
    cmd = get_matched_cmd(ctrl);
    if(cmd == NULL)
        return ERR_COMMAN;
    if(ctrl->param.argc == 1)
        ctrl->ops.printf(cmd->helpdetails);
    cmd->execute(ctrl->param.argc,ctrl->param.argv);
    return ERR_OK;
}


w_err_t console_proc(w_int32_t argc,char **argv)
{
    w_int32_t len;
    console_s *ctrl;
    w_err_t err;

    if(argc >= WIND_CONSOLE_COUNT)
        return ERR_COMMAN;
    ctrl = &g_ctrl[argc];
    cmd_history_init(&ctrl->his);
    init_console_stat(ctrl);
    CONSOLE_OUT("\r\nlogin:");
    while(1)
    {
        len = console_read_line(ctrl,WIND_CMD_MAX_LEN);
        if(len > 0)
        {
            switch(ctrl->stat)
            {
                case CSLSTAT_USER:
                    err = check_user_name(ctrl);
                    if(err != ERR_OK)
                        CONSOLE_OUT("\r\nlogin:");
                    else
                        ctrl->stat = CSLSTAT_PWD;
                    break;
                case CSLSTAT_PWD:
                    err = check_user_pwd(ctrl);
                    if(err != ERR_OK)
                        CONSOLE_OUT("\r\nlogin:");
                    else
                        ctrl->stat = CSLSTAT_APP;
                    break;
                case CSLSTAT_APP:
                    CONSOLE_OUT("%s@wind-os>");
                    cmd_history_append(&ctrl->his,ctrl->buf);
                    execute_cmd(ctrl);
                    if(wind_strcmp(ctrl->buf,"exit") == 0)
                    {
                        CONSOLE_OUT("\r\nlogin:");
                        ctrl->stat = CSLSTAT_APP;
                    }
                    break;
                default:
                    CONSOLE_OUT("\r\nlogin:");
                    ctrl->stat = CSLSTAT_APP;
                    break;
            }
        }
    }
    
}

#if 0
for(i = 0;i < len;i ++)
{
    do
    {
        ch = ctrl->buf[i];
        if(!is_valid_ch(ch))
        {
            memoryset(ctrl->cmdstr,0,WIND_CMD_MAX_LEN);
            ctrl->index = 0;
            break;
        }
        wind_console_prehandle(ctrl,ch);
        if(ch == WVK_BACKSPACE)
        {
            ctrl->cmdstr[ctrl->index] = 0;
            if(ctrl->index > 0)
            {
                ctrl->index --;
            }
        }
        else
        {
            ctrl->cmdstr[ctrl->index ++] = ch;
        }

        if((ch == '\n') || (ch == '\r'))
        {
            ctrl->cmdstr[ctrl->index - 1] = 0;
            ctrl->index --;

                wind_decode_cmd(ctrl);
                memoryset(ctrl->cmdstr,0,WIND_CMD_MAX_LEN);
                if(ctrl->stat == CSLSTAT_CMD)
                    CONSOLE_OUT("%s@wind_os/GT2440>#:",ctrl->user);
                ctrl->index = 0;
        }
        if(ctrl->index>= WIND_CMD_MAX_LEN)
        {
            CONSOLE_OUT("\r\nERROR:command is too long\r\n");
            memoryset(ctrl->cmdstr,0,WIND_CMD_MAX_LEN);
            ctrl->index = 0;
        }
    }while(0);
}

#endif

#define CTRL_STK_SIZE 1024
static w_stack_t ctrlstk[CTRL_STK_SIZE];//主任务堆栈
void create_console_thread(void)
{
    g_core.pctrl = wind_thread_create("console",PRIO_LOW,console_proc,
               0,NULL,ctrlstk,CTRL_STK_SIZE);
    wind_thread_changeprio(g_core.pctrl,32760);
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
