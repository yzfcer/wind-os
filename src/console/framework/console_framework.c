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

/********************************************内部变量定义**********************************************/

console_s g_ctrl[WIND_CONSOLE_COUNT];
cmd_global_s *g_cmd_global;
//临时使用的数据读取函数
static err_t core_get_cmd_ch(s8_t *ch)
{
    //*ch = uart_getchar();
    //wind_getchar(ch);
    *ch = 0;
    return ERR_OK;
}


/********************************************内部函数定义*********************************************/
static void show_cmd_list(void)
{
    cmd_s *cmd = g_cmd_global->head;
    while(cmd)
    {
        WIND_INFO("%s : %s\r\n",cmd->cmd,cmd->helpinfo);
        cmd = cmd->next;
    }
}

static s32_t stringlenth(char *str)
{
    s32_t i;
    for(i = 0;i < 65535;i ++)
    {
        if(str[i] == 0)
            break;
    }
    return i;
}

char* stringcopy(char *dest,const char *src)
{
    char *tmp = dest;
    
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

void * memoryset(void * s,char c,u32_t count)
{
    char *xs = (char *) s;

    while (count--)
        *xs++ = c;

    return s;
}

//把大写字母转换成小写
static void wind_cmd_tolower(char *cmdstr)
{
    u16_t i = 0;
    while((cmdstr[i] != ' ') && (cmdstr[i] != 0))
    {
        if((cmdstr[i] >= 'A') && (cmdstr[i] <= 'Z'))
            cmdstr[i] += 0x20;
        i ++;
    }
}

s32_t is_match_cmd(char *cmdstr,char*cmd)
{
    int i;
    int len = stringlenth(cmd);
    for(i = 0;i < len;i ++)
    {
        if(cmd[i] != cmdstr[i])
            return 0;
    }
    return 1;
}
s32_t is_string_equal(char *dest,char *src)
{
    s32_t i = 0;
    WIND_DEBUG("%S<->%S\r\n",dest,src);
    do
    {
        if(dest[i] != src[i])
            return 0;
        if(dest[i] == 0)
            break;
        i ++;
    }while(1);
    return 1;
}

#if 0
static s32_t console_read(char *buf,s32_t len)
{
    char buff[512];
    s32_t lenth,i;
    wind_memset(buff,0,512);
    lenth = stringlenth(buff);
    for(i = 0;i < lenth;i ++)
        buf[i] = buff[i];
    buf[i++] = '\n';
    return i;
}
#else
static s32_t console_read(char *buf,s32_t len)
{
    core_get_cmd_ch(buf);
    return 1;
}
#endif


static void wind_console_echoback(console_s *ctrl,char ch)
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

static s32_t is_valid_ch(char ch)
{
    if((ch >= 0x20) && (ch <= 0x7e))
        return 1;
    if((ch == WVK_BACKSPACE))
        return 1;
    if((ch == '\r') || (ch == '\n'))
        return 1;
    return 0;
}

static init_console_stat(console_s *ctlobj,s32_t flag)
{
    s32_t i;
    for(i = 0;i < WIND_CMD_MAX_LEN;i++)
        ctlobj->cmdstr[i] = 0;    
    for(i = 0;i < WIND_CMD_MAX_LEN;i++)
        ctlobj->buf[i] = 0;
    for(i = 0;i < WIND_CTL_PWD_LEN;i++)
        ctlobj->pwd[i] = 0;
    ctlobj->index = 0;
    for(i = 0;i < WIND_CTL_USRNAME_LEN;i++)
        ctlobj->pwd[i] = 0;
    if(flag)
    {
        ctlobj->stat = CSLSTAT_USER;
    }
    
    console_framework_init(ctlobj);
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
static void get_param(console_s *ctrl,char *cmd)
{
    s32_t len,strlen,i,offset;
    cmd_param_s *param = &ctrl->param;
    for(i = 0;i < CMD_PARAM_CNT;i ++)
    {
        param->param[i] = NULL;
    }
    param->cnt = 0;
    len =stringlenth(cmd);
    if(len <= 0)
    {
        return;
    }
    strlen = stringlenth(ctrl->cmdstr);
    if(len+1>= strlen)
        return;
    offset = len + 1;
    while(1)
    {   
        param->param[param->cnt++] = &ctrl->cmdstr[i];
        offset = find_param_end(ctrl->cmdstr,offset,strlen);
        if(offset >= strlen)
        break;
    }
    WIND_DEBUG("get param num:%d\r\n",param->cnt);
    for(i = 0;i < param->cnt;i ++)
    {
        WIND_DEBUG("param[%d] :%s\r\n",i+1,param->param);
    }
}
//检查用户名是否正确
static err_t wind_check_user(char *user)
{
    err_t err;
    err = is_string_equal(user,"root");
    if(err == 0)
    {
        err = is_string_equal(user,"wind");
    }
    if(err)
        return ERR_OK;
    else
        return ERR_COMMAN;
}

static err_t wind_check_pwd(char *pwd)
{
    err_t err;
    err = is_string_equal(pwd,"wind");
    if(err)
        return ERR_OK;
    else
        return ERR_COMMAN;
}
cmd_s *get_matched_cmd(console_s *ctrl)
{
    //int len1,len2;
    cmd_s *cmd = ctrl->cmd_list.head;
    
    //len1 = stringlenth(ctrl->cmdstr);
    while(cmd)
    {
        //len2 = stringlenth(cmd->cmd);
        if(is_match_cmd(ctrl->cmdstr,cmd->cmd))
        {
            return cmd;
        }
        cmd = cmd->next;
    }
    return NULL;
}
//分析命令字符串
err_t wind_decode_cmd(console_s *ctrl)
{
    u16_t len,i;
    err_t err;
    cmd_param_s *param;
    cmd_s *pcmd;// = ctrl->cmd_list.head;
    param = &ctrl->param;
    switch(ctrl->stat)
    {
    case CSLSTAT_USER:
        if(wind_check_user(ctrl->cmdstr) == ERR_OK)
        {
            ctrl->stat= CSLSTAT_PWD;
            memoryset((void*)ctrl->user,0,WIND_CTL_USRNAME_LEN);
            stringcopy(ctrl->user,ctrl->cmdstr);
            CONSOLE_OUT("\r\npassword:");
        }
        else
        {
            CONSOLE_OUT("has no such a user:%s\r\n",ctrl->cmdstr);
            CONSOLE_OUT("\r\nlogin:");
        }
        err = ERR_OK;
        break;
    case CSLSTAT_PWD:
        CONSOLE_OUT("\r\n");
        if(wind_check_pwd(ctrl->cmdstr) == ERR_OK)
        {
            
            memoryset((void*)ctrl->pwd,0,WIND_CTL_PWD_LEN);
            stringcopy(ctrl->pwd,ctrl->cmdstr);
            CONSOLE_OUT("%s login OK!\r\n",ctrl->user);
            ctrl->stat = CSLSTAT_CMD;
        }
        else
        {
            ctrl->stat = CSLSTAT_USER;
            CONSOLE_OUT("password error!\r\n");
            CONSOLE_OUT("\r\nlogin:",ctrl->user);
        }
        err = ERR_OK;
        break;
    case CSLSTAT_CMD:
        pcmd = get_matched_cmd(ctrl);
        if(NULL == pcmd)
        {
            if(stringlenth(ctrl->cmdstr) <= 0)
            {
                err = ERR_OK;
                break;
            }
            CONSOLE_OUT("unknown command:\"%s\" captured,input \"help\" to get helping infomation\r\n",ctrl->cmdstr);
        }
        get_param(ctrl,pcmd->cmd);
        pcmd->cmdFunc(param->cnt - 1,&param->param[1]);
        err = ERR_OK;
        //return err;
        while(pcmd)
        {
            if(is_string_equal(ctrl->cmdstr,pcmd->cmd))
            {
            }
            pcmd = pcmd->next;
        }
        len = stringlenth(ctrl->cmdstr);
        for(i = 0;i < len;i ++)
        {
            if(ctrl->cmdstr[i] == ' ')
            {
                ctrl->cmdstr[i] = 0;
                break;
            }
        }
        if(is_string_equal(ctrl->cmdstr,"exit"))
        {
            ctrl->stat = CSLSTAT_USER;
            CONSOLE_OUT("user:%s log out\r\n",ctrl->user);
            CONSOLE_OUT("\r\nlogin:");
            err = ERR_OK;
            break;
        }
        break;
    case CSLSTAT_APP://运行于应用程序模式
        ctrl->stat = CSLSTAT_USER;
        err = ERR_OK;
        break;
    default:
        ctrl->stat = CSLSTAT_USER;
        err = ERR_OK;
        break; 
    }
    return err;
}

/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

void console_framework_init(console_s *ctlobj)
{
    cmd_global_s *cgl = &ctlobj->cmd_list;
    cgl->head = NULL;
    cgl->tail = NULL;
    cgl->cnt = 0;
    WIND_INFO("register system commands.\r\n");
    register_cmd_echo(ctlobj);
    register_cmd_help(ctlobj);
    register_cmd_proc(ctlobj);
    register_cmd_show(ctlobj);
    register_cmd_stat(ctlobj);
    register_cmd_test(ctlobj);
    g_cmd_global = cgl;
    show_cmd_list();
}
err_t wind_cmd_register(cmd_global_s *cgl,cmd_s *cmd,int cnt)
{
    int i;
    cmd_global_s *cg = cgl;

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

err_t consoleProc(s32_t argc,char **argv)
{
    s32_t len;
    console_s *ctrl;
    s32_t i;
    char ch;

    if(argc >= WIND_CONSOLE_COUNT)
        return ERR_COMMAN;
    ctrl = &g_ctrl[argc];
    init_console_stat(ctrl,1);
    CONSOLE_OUT("\r\nlogin:");

    while(1)
    {
        len = console_read(ctrl->buf,WIND_CMD_MAX_LEN - 1);
        WIND_DEBUG("%s\r\n",ctrl->buf);
        if(len > 0)
        {
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
                    wind_console_echoback(ctrl,ch);
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
        }
        wind_thread_sleep(50);
    }
    return 0;
}

#define CTRL_STK_SIZE 2048
static stack_t ctrlstk[CTRL_STK_SIZE];//主任务堆栈
pthread_s lunch_console(void)
{
    pthread_s pthr = wind_thread_create("console",PRIO_LOW,consoleProc,
               0,NULL,ctrlstk,CTRL_STK_SIZE);
    wind_thread_changeprio(pthr,32760);
    return pthr;
}



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
