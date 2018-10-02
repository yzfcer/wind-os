/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: console_framework.h / console_framework.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 20:08:03
** 描        述: 能够支持标准输入输出的控制台的框架
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
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "console_framework.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_cmd.h"
#include "wind_std.h"
#include "wind_core.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
/********************************************内部变量定义**********************************************/

w_console_s g_ctrl[WIND_CONSOLE_COUNT];


/********************************************内部函数定义*********************************************/
static w_err_t get_cmd_ch(char *ch)
{
    w_int32_t len;
    len = wind_std_input((w_uint8_t *)ch,1);
    return len > 0 ? W_ERR_OK : W_ERR_FAIL;
}

static w_bool_t insert_ch(w_console_s *ctrl,char ch,w_int32_t len)
{
    ctrl->buf[ctrl->index] = ch;
    ctrl->index ++;
    if(ctrl->index >= len)
    {
        ctrl->index --;
        ctrl->buf[ctrl->index] = 0;
        return W_FALSE;
    }
    return W_TRUE;
}

static w_bool_t handle_LF(w_console_s *ctrl)
{
    ctrl->buf[ctrl->index] = 0;
    console_printf("\r\n");
    return W_TRUE;
}

static w_bool_t handle_BKSPACE(w_console_s *ctrl)
{
    if(ctrl->index > 0)
    {
        ctrl->index --;
        console_printf("%c",WVK_BACKSPACE);
        console_printf(VT100_ERASE_END);
    }
    return W_FALSE;
}

static w_bool_t handle_ESC(w_console_s *ctrl)
{
    ctrl->key_evt_f = 1;
    ctrl->key_evt_len = 1;
    ctrl->key_evt_id = 0x1b;
    return W_FALSE;
}

static w_bool_t handle_key_evt_up(w_console_s *ctrl)
{
    w_err_t err;
    while(ctrl->index > 0)
        handle_BKSPACE(ctrl);
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_prev(&ctrl->his,ctrl->buf);
    if(W_ERR_OK == err)
    {
        ctrl->index = wind_strlen(ctrl->buf);
        console_printf("%s",ctrl->buf);
    }
    return W_FALSE;
}

static w_bool_t handle_key_evt_down(w_console_s *ctrl)
{
    w_err_t err;
    while(ctrl->index > 0)
        handle_BKSPACE(ctrl);
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_next(&ctrl->his,ctrl->buf);
    if(W_ERR_OK == err)
    {
        ctrl->index = wind_strlen(ctrl->buf);
        console_printf("%s",ctrl->buf);
    }
    return W_FALSE;
}


static w_bool_t handle_key_evt(w_console_s *ctrl,char ch)
{
    if(!ctrl->key_evt_f)
        return W_FALSE;
    ctrl->key_evt_id <<= 8;
    ctrl->key_evt_id += ch;
    ctrl->key_evt_len ++;

        
    switch(ctrl->key_evt_id)
    {
        case KEY_EVT_UP:
            handle_key_evt_up(ctrl);
            goto key_evt_done;
        case KEY_EVT_DOWN:
            handle_key_evt_down(ctrl);
            goto key_evt_done;
            
        case KEY_EVT_LEFT://         0x1B5B44
        case KEY_EVT_RIGHT://        0x1B5B43

        case KEY_EVT_HOME://         0x1B5B317E
        case KEY_EVT_INS://          0x1B5B327E
        case KEY_EVT_DEL://          0x1B5B337E
        case KEY_EVT_END://          0x1B5B347E
        case KEY_EVT_PGUP://         0x1B5B357E
        case KEY_EVT_PGDN://         0x1B5B367E
            goto key_evt_done;
        default:
            if(ctrl->key_evt_len >= 4)
            {
                console_printf("unknown KEY event:0x%X\r\n",ctrl->key_evt_id);
                goto key_evt_done;
            }
            goto key_evt_ret;
    }
key_evt_done:
    ctrl->key_evt_f = 0;
    ctrl->key_evt_len = 0;
    ctrl->key_evt_id = 0;
key_evt_ret:
    return W_TRUE;
    
}



static w_bool_t handle_default(w_console_s *ctrl,char ch)
{
    if(ctrl->stat != CSLSTAT_PWD)
        console_printf("%c",ch);
    return W_FALSE;
}

//返回true则表示有一个完整的命令
//返回false表示命令不完整
static w_bool_t console_prehandle_char(w_console_s *ctrl,char ch,w_int32_t len)
{
    w_bool_t ret;
    ret = handle_key_evt(ctrl,ch);
    if(W_TRUE == ret)
        return W_FALSE;
    if(ch == WVK_BACKSPACE)
    {
        return handle_BKSPACE(ctrl);
    }
    else if(ch == WVK_ENTER)
    {
        return handle_LF(ctrl);
    }
    else if(ch == WVK_ESCAPE)
    {
        return handle_ESC(ctrl);
    }
    else
    {
        handle_default(ctrl,ch);
        ret = insert_ch(ctrl,ch,len);
        if(W_TRUE == ret)
            return W_FALSE;
        return W_TRUE;
    }
}

static void console_clear_buf(w_console_s *ctrl)
{
    ctrl->index = 0;
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
}

w_int32_t console_read_line(w_console_s *ctrl,w_int32_t len)
{
    w_err_t err;
    char ch;
    console_clear_buf(ctrl);
    while(1)
    {
        err = get_cmd_ch(&ch);
        if(err != W_ERR_OK)
            wind_thread_sleep(20);
        else
        {
            if(console_prehandle_char(ctrl,ch,len))
                return ctrl->index;
        }
    }
}


static void init_console_stat(w_console_s *ctrl)
{
#if USER_AUTHENTICATION_EN
    ctrl->stat = CSLSTAT_USER;
#else
    ctrl->stat = CSLSTAT_CMD;
#endif
    ctrl->index = 0;
    ctrl->key_evt_f = 0;
    ctrl->key_evt_len = 0;
    ctrl->key_evt_id = 0;
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    wind_memset(ctrl->user,0,WIND_CTL_USRNAME_LEN);
    wind_memset(ctrl->pwd,0,WIND_CTL_PWD_LEN);
    cmd_history_init(&ctrl->his);
    wind_cmd_init(ctrl);
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

void wind_cmd_init(w_console_s *ctrl)
{
    DLIST_INIT(g_core.cmdlist);
    _wind_register_all_cmd(ctrl);
}

w_cmd_s *wind_cmd_get(const char *name)
{
    w_cmd_s *cmd;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_core.cmdlist)
    {
        cmd = DLIST_OBJ(dnode,w_cmd_s,cmdnode);
        if(wind_strcmp(name,cmd->name) == 0)
        {
            wind_enable_switch();
            return cmd;
        }
    }
    wind_enable_switch();
    return W_NULL;
}

w_err_t wind_cmd_register(w_cmd_s *cmd,int cnt)
{
    int i;
    w_cmd_s *old;
    w_dlist_s *cgl = &g_core.cmdlist;
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < cnt;i ++)
    {
        old = wind_cmd_get(cmd->name);
        if(old != W_NULL)
            continue;
        wind_disable_switch();
        dlist_insert_tail(cgl,&cmd[i].cmdnode);
        wind_enable_switch();
    }
    return W_ERR_OK;
}

w_err_t wind_cmd_print(void)
{
    w_dnode_s *dnode;
    w_cmd_s *cmd;
    wind_printf("\r\ncommand list as following:\r\n");
    foreach_node(dnode,&g_core.cmdlist)
    {
        cmd = DLIST_OBJ(dnode,w_cmd_s,cmdnode);
        console_printf("%-10s : ",cmd->name);
        cmd->showdisc();
    }
    return W_ERR_OK;
}

#if USER_AUTHENTICATION_EN
static w_err_t check_user_name(w_console_s *ctrl)
{
    if(wind_strcmp(ctrl->buf,"root") != 0)
    {
        console_printf("\r\nlogin:");
        return W_ERR_FAIL;
    }
    wind_strcpy(ctrl->user,ctrl->buf);
    ctrl->stat = CSLSTAT_PWD;
    console_printf("\r\npasswd:");
    return W_ERR_OK;
}


static w_err_t check_user_pwd(w_console_s *ctrl)
{
    if(wind_strcmp(ctrl->user,"root") != 0 ||
        wind_strcmp(ctrl->buf,"wind") != 0)
    {
        ctrl->stat = CSLSTAT_USER;
        console_printf("\r\nlogin:");
        return W_ERR_FAIL;
    }
    wind_strcpy(ctrl->pwd,ctrl->buf);
    ctrl->stat = CSLSTAT_CMD;
    console_printf("\r\n%s@wind-os>",ctrl->user);
    return W_ERR_OK;
}
#endif
static w_int32_t find_char_index(char *str,char c)
{
    w_int32_t idx = 0;
    for(idx = 0;idx < WIND_CMD_MAX_LEN;idx ++)
    {
        if(str[idx] == c)
            return idx;
        if(str[idx] == 0)
            return -1;
    }
    return -1;
}

static w_int32_t get_string(char *str,w_int32_t idx,char ** arg)
{
    int i = idx,index;
    /*跳过空格*/
    while(str[i] == ' ') i ++;
    if(str[i] == '\"')
    {/*引号内的空格不分割，去掉引号*/
        i ++;
        *arg = &str[i];
        index = find_char_index(&str[i],'\"');
        if(index <= 0)
            return -1;
        i += index;
        str[i] = 0;
    }
    else
    {
        while(str[i] && (str[i] == ' '))
            i ++;
        if(str[i] == 0)
            return -1;
        *arg = &str[i];
        while(str[i] && (str[i] != ' '))
            i ++;
        str[i] = 0;
    }
    
    return i + 1;
}

static w_err_t spit_cmd(w_console_s *ctrl)
{
    w_err_t idx = 0,i;
    w_cmd_param_s *prm = &ctrl->param;
    prm->argc = 0;
    for(i = 0;i < CMD_PARAM_CNT;i ++)
    {
        prm->argv[i] = W_NULL;
        idx = get_string(ctrl->buf,idx,&prm->argv[i]);
        if(idx > 0)
            prm->argc ++;
        else
            break;
    }
    if(i >= CMD_PARAM_CNT)
        return W_ERR_FAIL;
    return W_ERR_OK;
}


static w_err_t execute_cmd(w_console_s *ctrl)
{
    w_err_t err;
    w_cmd_s *cmd;
    err = spit_cmd(ctrl);
    if(err < 0)
        return err;
    if(wind_strcmp(ctrl->param.argv[0],"?") == 0)
    {
        wind_cmd_print();
        return W_ERR_OK;
    }
    cmd = wind_cmd_get(ctrl->param.argv[0]);//get_matched_cmd(ctrl);
    if(cmd == W_NULL)
        return W_ERR_FAIL;
    if(wind_strcmp(ctrl->param.argv[1],"?") == 0)
    {
        cmd->showusage();
        return W_ERR_OK;
    }
    cmd->execute(ctrl->param.argc,ctrl->param.argv);
    return W_ERR_OK;
}


w_err_t console_thread(w_int32_t argc,char **argv)
{
    w_int32_t len;
    w_console_s *ctrl;
    if(argc >= WIND_CONSOLE_COUNT)
        return W_ERR_FAIL;
    ctrl = &g_ctrl[argc];
    cmd_history_init(&ctrl->his);
    init_console_stat(ctrl);

    //console_printf("\r\nlogin:");
    while(1)
    {
        len = console_read_line(ctrl,WIND_CMD_MAX_LEN);
        if(len >= 0)
        {
            switch(ctrl->stat)
            {
#if USER_AUTHENTICATION_EN
                case CSLSTAT_USER:
                    check_user_name(ctrl);
                    break;
                case CSLSTAT_PWD:
                    check_user_pwd(ctrl);
                    break;
#endif 
                case CSLSTAT_CMD:
                    cmd_history_append(&ctrl->his,ctrl->buf);
                    execute_cmd(ctrl);
#if USER_AUTHENTICATION_EN
                    if(wind_strcmp(ctrl->buf,"exit") == 0)
                    {
                        ctrl->stat = CSLSTAT_USER;
                        console_printf("\r\nlogin:");
                    }
                    else
#endif
                        console_printf("\r\n%s@wind-os>",ctrl->user);
                    break;
                default:
                    console_printf("\r\nlogin:");
                    ctrl->stat = CSLSTAT_USER;
                    break;
            }
        }
    }
    
}



#define CTRL_STK_SIZE 2048
static w_stack_t ctrlstk[CTRL_STK_SIZE];//主任务堆栈
w_err_t _create_console_thread(void)
{
    w_thread_s *thread;
    thread = wind_thread_create("console",console_thread,
               0,W_NULL,PRIO_LOW,ctrlstk,CTRL_STK_SIZE);
    WIND_ASSERT_RETURN(thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(thread,32760);
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
