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
#include "wind_config.h"
#include "wind_type.h"
#include "console_framework.h"
#include "wind_debug.h"
#include "wind_var.h"
#include "wind_cmd.h"
#include "wind_std.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if WIND_CONSOLE_SUPPORT

/*********************************************头文件定义***********************************************/
/********************************************内部变量定义**********************************************/

console_s g_ctrl[WIND_CONSOLE_COUNT];
cmd_list_s *g_cmd_global;
//临时使用的数据读取函数


/********************************************内部函数定义*********************************************/
static w_err_t core_get_cmd_ch(w_int8_t *ch)
{
    w_int32_t len;
    len = wind_std_input((w_uint8_t *)ch,1);
    return len > 0?ERR_OK:ERR_COMMAN;
}

static void show_cmd_list(void)
{
    cmd_s *cmd = g_cmd_global->head;
    while(cmd)
    {
        console_printf("%s : ",cmd->cmd);
        cmd->showdisc();
        cmd = cmd->next;
    }
}

static w_bool_t insert_ch(console_s *ctrl,char ch,w_int32_t len)
{
    ctrl->buf[ctrl->index] = ch;
    ctrl->index ++;
    if(ctrl->index >= len)
    {
        ctrl->index --;
        ctrl->buf[ctrl->index] = 0;
        return B_FALSE;
    }
    return B_TRUE;
}

static w_bool_t handle_LF(console_s *ctrl)
{
    ctrl->buf[ctrl->index] = 0;
    console_printf("\r\n");
    return B_TRUE;
}

static w_bool_t handle_DEL(console_s *ctrl)
{
    //ctrl->index --;
    if(ctrl->index > 0)
    {
        ctrl->index --;
        console_printf("%c",WVK_DEL);
    }
    return B_FALSE;
}

static w_bool_t handle_ESC(console_s *ctrl)
{
    ctrl->key_evt_f = 1;
    ctrl->key_evt_len = 1;
    ctrl->key_evt_id = 0x1b;
    return B_FALSE;
}

static w_bool_t handle_key_evt_up(console_s *ctrl)
{
    w_err_t err;
    while(ctrl->index > 0)
        handle_DEL(ctrl);
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_prev(&ctrl->his,ctrl->buf);
    if(ERR_OK == err)
    {
        ctrl->index = wind_strlen(ctrl->buf);
        console_printf("%s",ctrl->buf);
    }
    return B_FALSE;
}

static w_bool_t handle_key_evt_down(console_s *ctrl)
{
    w_err_t err;
    while(ctrl->index > 0)
        handle_DEL(ctrl);
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_next(&ctrl->his,ctrl->buf);
    if(ERR_OK == err)
    {
        ctrl->index = wind_strlen(ctrl->buf);
        console_printf("%s",ctrl->buf);
    }
    return B_FALSE;
}


static w_bool_t handle_key_evt(console_s *ctrl,char ch)
{
    if(!ctrl->key_evt_f)
        return B_FALSE;
    ctrl->key_evt_id <<= 8;
    ctrl->key_evt_id += ch;
    ctrl->key_evt_len ++;

        
    switch(ctrl->key_evt_id)
    {
        case KEY_EVT_UP:
            //console_printf("receive KEY_UP\r\n");
            handle_key_evt_up(ctrl);
            goto key_evt_done;
        case KEY_EVT_DOWN:
            //console_printf("receive KEY_EVT_DOWN\r\n");
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
    return B_TRUE;
    
}



static w_bool_t handle_default(console_s *ctrl,char ch)
{
    if(ctrl->stat != CSLSTAT_PWD)
        console_printf("%c",ch);
    return B_FALSE;
}

//返回true则表示有一个完整的命令
//返回false表示命令不完整
static w_bool_t console_prehandle_char(console_s *ctrl,char ch,w_int32_t len)
{
    w_bool_t ret;
    ret = handle_key_evt(ctrl,ch);
    if(B_TRUE == ret)
        return B_FALSE;
    if(ch == WVK_DEL)
    {
        return handle_DEL(ctrl);
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
        if(B_TRUE == ret)
            return B_FALSE;
        return B_TRUE;
    }
}

static void console_clear_buf(console_s *ctrl)
{
    ctrl->index = 0;
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
}

static w_int32_t console_read_line(console_s *ctrl,w_int32_t len)
{
    w_err_t err;
    char ch;
    console_clear_buf(ctrl);
    while(1)
    {
        err = core_get_cmd_ch(&ch);
        if(err != ERR_OK)
            wind_thread_sleep(20);
        else
        {
            if(console_prehandle_char(ctrl,ch,len))
                return ctrl->index;
        }
    }
}


static void init_console_stat(console_s *ctrl)
{
#if USER_AUTHENTICATION_EN
    ctrl->stat = CSLSTAT_USER;
#else
    ctrl->stat = CSLSTAT_APP;
#endif
    ctrl->index = 0;
    ctrl->key_evt_f = 0;
    ctrl->key_evt_len = 0;
    ctrl->key_evt_id = 0;
    wind_memset(ctrl->buf,0,WIND_CMD_MAX_LEN);
    wind_memset(ctrl->user,0,WIND_CTL_USRNAME_LEN);
    wind_memset(ctrl->pwd,0,WIND_CTL_PWD_LEN);
    cmd_history_init(&ctrl->his);
    console_framework_init(ctrl);
    //ctrl->ops.printf = console_printf;
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
    register_all_cmd(ctrl);
    g_cmd_global = cgl;
    show_cmd_list();
}

w_bool_t is_in_list(cmd_list_s *list,cmd_s *cmd)
{
    w_int32_t i;
    cmd_s *c = list->head;
    for(i = 0;i < list->cnt;i ++)
    {
        if(c == cmd)
            return B_TRUE;
        c = c->next;
    }
    return B_FALSE;
}

w_err_t wind_cmd_register(cmd_list_s *cgl,cmd_s *cmd,int cnt)
{
    int i;
    cmd_list_s *cg = cgl;

    WIND_ASSERT_RETURN(cmd != NULL,ERR_NULL_POINTER);
    for(i = 0;i < cnt;i ++)
    {
        if(is_in_list(cg,&cmd[i]))
            continue;
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

#if USER_AUTHENTICATION_EN
static w_err_t check_user_name(console_s *ctrl)
{
    if(wind_strcmp(ctrl->buf,"root") != 0)
    {
        console_printf("\r\nlogin:");
        return ERR_COMMAN;
    }
    wind_strcpy(ctrl->user,ctrl->buf);
    ctrl->stat = CSLSTAT_PWD;
    console_printf("\r\npasswd:");
    return ERR_OK;
}


static w_err_t check_user_pwd(console_s *ctrl)
{
    if(wind_strcmp(ctrl->user,"root") != 0 ||
        wind_strcmp(ctrl->buf,"wind") != 0)
    {
        ctrl->stat = CSLSTAT_USER;
        console_printf("\r\nlogin:");
        return ERR_COMMAN;
    }
    wind_strcpy(ctrl->pwd,ctrl->buf);
    ctrl->stat = CSLSTAT_APP;
    console_printf("\r\n%s@wind-os>",ctrl->user);
    return ERR_OK;
}
#endif

static w_int32_t get_string(char *str,w_int32_t idx,char ** arg)
{
    int i;
    while(str[i] && (str[i] == ' '))
        i ++;
    if(str[i] == 0)
        return -1;
    *arg = &str[i];
    while(str[i] && (str[i] != ' '))
        i ++;
    str[i] = 0;
    return i + 1;
}

static w_err_t spit_cmd(console_s *ctrl)
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

static w_err_t execute_cmd(console_s *ctrl)
{
    w_err_t err;
    cmd_s *cmd;
    err = spit_cmd(ctrl);
    if(err < 0)
        return err;
    if(wind_strcmp(ctrl->param.argv[0],"?") == 0)
    {
        show_cmd_list();
        return ERR_OK;
    }
    cmd = get_matched_cmd(ctrl);
    if(cmd == NULL)
        return ERR_COMMAN;
    if(wind_strcmp(ctrl->param.argv[1],"?") == 0)
    {
        cmd->showusage();
        return ERR_OK;
    }
    cmd->execute(ctrl->param.argc,ctrl->param.argv);
    return ERR_OK;
}



w_err_t console_thread(w_int32_t argc,char **argv)
{
    w_int32_t len;
    console_s *ctrl;
    if(argc >= WIND_CONSOLE_COUNT)
        return ERR_COMMAN;
    ctrl = &g_ctrl[argc];
    cmd_history_init(&ctrl->his);
    init_console_stat(ctrl);
    test_init(ctrl);
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
                case CSLSTAT_APP:
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
                    ctrl->stat = CSLSTAT_APP;
                    break;
            }
        }
    }
    
}



#define CTRL_STK_SIZE 256
static w_stack_t ctrlstk[CTRL_STK_SIZE];//主任务堆栈
void create_console_thread(void)
{
    g_core.pctrl = wind_thread_create("console",PRIO_LOW,console_thread,
               0,NULL,ctrlstk,CTRL_STK_SIZE);
    wind_thread_changeprio(g_core.pctrl,32760);
}

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
