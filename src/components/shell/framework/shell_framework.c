/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: shell_framework.h / shell_framework.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2015/1/24 20:08:03
** 描        述: 能够支持标准输入输出的控制台的框架
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
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
#include "shell_framework.h"
#include "wind_debug.h"
#include "wind_cmd.h"
#include "wind_std.h"
#include "wind_conv.h"
//#include "wind_core.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************头文件定义***********************************************/
/********************************************内部变量定义**********************************************/

w_dlist_s g_cmdlist;
w_shell_ctx_s g_shell_ctx[WIND_SHELL_CTX_COUNT];


/********************************************内部函数定义*********************************************/
static w_err_t get_cmd_ch(char *ch)
{
    w_int32_t len;
    len = wind_std_input((w_uint8_t *)ch,1);
    return len > 0 ? W_ERR_OK : W_ERR_FAIL;
}

static w_bool_t insert_ch(w_shell_ctx_s *ctx,char ch,w_int32_t len)
{
    ctx->buf[ctx->index] = ch;
    ctx->index ++;
    if(ctx->index >= len)
    {
        ctx->index --;
        ctx->buf[ctx->index] = 0;
        return W_FALSE;
    }
    return W_TRUE;
}

static w_bool_t handle_LF(w_shell_ctx_s *ctx)
{
    ctx->buf[ctx->index] = 0;
    wind_printf("\r\n");
    return W_TRUE;
}

static w_bool_t handle_BKSPACE(w_shell_ctx_s *ctx)
{
    if(ctx->index > 0)
    {
        ctx->index --;
        wind_printf("\b \b");
    }
    return W_FALSE;
}

static w_bool_t handle_ESC(w_shell_ctx_s *ctx)
{
    ctx->key_evt_f = 1;
    ctx->key_evt_len = 1;
    ctx->key_evt_id = 0x1b;
    return W_FALSE;
}

static w_bool_t handle_DIR(w_shell_ctx_s *ctx)
{
    ctx->key_evt_f = 1;
    ctx->key_evt_len = 1;
    ctx->key_value= 0xE0;
    return W_FALSE;
}

static w_bool_t handle_key_evt_up(w_shell_ctx_s *ctx)
{
    w_err_t err;
    while(ctx->index > 0)
        handle_BKSPACE(ctx);
    wind_memset(ctx->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_prev(&ctx->his,ctx->buf);
    if(W_ERR_OK == err)
    {
        ctx->index = wind_strlen(ctx->buf);
        wind_printf("%s",ctx->buf);
    }
    return W_FALSE;
}

static w_bool_t handle_key_evt_down(w_shell_ctx_s *ctx)
{
    w_err_t err;
    while(ctx->index > 0)
        handle_BKSPACE(ctx);
    wind_memset(ctx->buf,0,WIND_CMD_MAX_LEN);
    err = cmd_history_get_next(&ctx->his,ctx->buf);
    if(W_ERR_OK == err)
    {
        ctx->index = wind_strlen(ctx->buf);
        wind_printf("%s",ctx->buf);
    }
    return W_FALSE;
}


static w_bool_t handle_key_evt(w_shell_ctx_s *ctx,char ch)
{
    if(!ctx->key_evt_f)
        return W_FALSE;
    ctx->key_evt_id <<= 8;
    ctx->key_evt_id += ch;
    ctx->key_value <<= 8;
    ctx->key_value += ch;
    ctx->key_evt_len ++;
    if((ctx->key_evt_id == KEY_EVT_UP)||(ctx->key_value == KEY_UP_PRT))
    {
        handle_key_evt_up(ctx);
        goto key_evt_done;
    }
    if((ctx->key_evt_id == KEY_EVT_DOWN)||(ctx->key_value == KEY_DOWN_PRT))
    {
        handle_key_evt_down(ctx);
        goto key_evt_done;
    }

        
    switch(ctx->key_evt_id)
    {            
        case KEY_EVT_LEFT://         0x1B5B44
        case KEY_EVT_RIGHT://        0x1B5B43

        case KEY_EVT_HOME://         0x1B5B317E
        case KEY_EVT_INS://          0x1B5B327E
        case KEY_EVT_DEL://          0x1B5B337E
        case KEY_EVT_END://          0x1B5B347E
        case KEY_EVT_PGUP://         0x1B5B357E
        case KEY_EVT_PGDN://         0x1B5B367E
            wind_printf("key:0x%x\r\n",ctx->key_evt_id);
            goto key_evt_done;
        default:
            if(ctx->key_evt_len >= 4)
            {
                wind_printf("unknown KEY event:0x%X\r\n",ctx->key_evt_id);
                goto key_evt_done;
            }
            goto key_evt_ret;
    }
key_evt_done:
    ctx->key_evt_f = 0;
    ctx->key_evt_len = 0;
    ctx->key_evt_id = 0;
    ctx->key_value= 0;
key_evt_ret:
    return W_TRUE;
    
}



static w_bool_t handle_default(w_shell_ctx_s *ctx,char ch)
{
    if(ctx->stat != CSLSTAT_PWD)
        wind_printf("%c",ch);
    return W_FALSE;
}

//返回true则表示有一个完整的命令
//返回false表示命令不完整
static w_bool_t shell_prehandle_char(w_shell_ctx_s *ctx,w_uint8_t ch,w_int32_t len)
{
    w_bool_t ret;

    ret = handle_key_evt(ctx,ch);
    if(W_TRUE == ret)
        return W_FALSE;
    if(ch == WVK_BACKSPACE)
    {
        return handle_BKSPACE(ctx);
    }
    else if(ch == WVK_ENTER)
    {
        return handle_LF(ctx);
    }
    else if(ch == WVK_ESCAPE)
    {
        return handle_ESC(ctx);
    }
    else if(ch == 0xE0)
    {
        return handle_DIR(ctx);
    }
    else
    {
        handle_default(ctx,ch);
        ret = insert_ch(ctx,ch,len);
        if(W_TRUE == ret)
            return W_FALSE;
        return W_TRUE;
    }
}

static void shell_clear_buf(w_shell_ctx_s *ctx)
{
    ctx->index = 0;
    wind_memset(ctx->buf,0,WIND_CMD_MAX_LEN);
}

w_int32_t shell_read_line(w_shell_ctx_s *ctx,w_int32_t len)
{
    w_err_t err;
    char ch;
    shell_clear_buf(ctx);
    while(1)
    {
        err = get_cmd_ch(&ch);
        if(err != W_ERR_OK)
            wind_thread_sleep(20);
        else
        {
            if(shell_prehandle_char(ctx,ch,len))
                return ctx->index;
        }
    }
}


static void shell_stat_init(w_shell_ctx_s *ctx)
{
#if USER_AUTH_ENABLE
    ctx->stat = CSLSTAT_USER;
#else
    ctx->stat = CSLSTAT_CMD;
#endif
    ctx->index = 0;
    ctx->key_evt_f = 0;
    ctx->key_evt_len = 0;
    ctx->key_evt_id = 0;
    ctx->key_value = 0;
    ctx->autherr_cnt = 0;
    wind_memset(ctx->buf,0,WIND_CMD_MAX_LEN);
    wind_memset(ctx->user,0,WIND_CTL_USRNAME_LEN);
    wind_strcpy(ctx->user,"anonymous");
    wind_memset(ctx->pwd,0,WIND_CTL_PWD_LEN);
    cmd_history_init(&ctx->his);
    wind_cmd_init(ctx);
}



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/

void wind_cmd_init(w_shell_ctx_s *ctx)
{
    DLIST_INIT(g_cmdlist);
    _wind_register_all_cmd(ctx);
}

w_cmd_s *wind_cmd_get(const char *name)
{
    w_cmd_s *cmd;
    w_dnode_s *dnode;
    wind_disable_switch();
    foreach_node(dnode,&g_cmdlist)
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
    w_dlist_s *cgl = &g_cmdlist;
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
    foreach_node(dnode,&g_cmdlist)
    {
        cmd = DLIST_OBJ(dnode,w_cmd_s,cmdnode);
        wind_printf("%-10s : ",cmd->name);
        cmd->showdisc();
    }
    return W_ERR_OK;
}

#if USER_AUTH_ENABLE
static w_err_t check_user_account(w_shell_ctx_s *ctx)
{
    if(wind_strcmp(ctx->buf,"root") != 0)
    {
        wind_printf("\r\nlogin:");
        return W_ERR_FAIL;
    }
    wind_strcpy(ctx->user,ctx->buf);
    ctx->stat = CSLSTAT_PWD;
    wind_printf("\r\npasswd:");
    return W_ERR_OK;
}

static w_err_t auth_err_suspend(w_shell_ctx_s *ctx)
{
    w_int32_t i;
    wind_printf("\r\nyouhave input error for %d times.\r\n",USER_AUTH_ERR_MAX);
    wind_printf("\r\nnow wait for %2dS",USER_AUTH_WAIT_SEC);
    for(i = USER_AUTH_WAIT_SEC;i >= 0;i --)
    {
        wind_thread_sleep(1000);
        wind_printf("\b \b\b \b\b \b");
        wind_printf("%2dS",i);
    }
    ctx->autherr_cnt = 0;
    return W_ERR_OK;
}


static w_err_t check_user_passwd(w_shell_ctx_s *ctx)
{
    w_int32_t wait_sec = USER_AUTH_ERR_MAX;
    if(wait_sec >= 99)
        wait_sec = 99;
    if(wind_strcmp(ctx->user,"root") != 0 ||
        wind_strcmp(ctx->buf,"wind") != 0)
    {
        ctx->autherr_cnt ++;
        if(ctx->autherr_cnt >= USER_AUTH_ERR_MAX)
            auth_err_suspend(ctx);
        ctx->stat = CSLSTAT_USER;
        wind_printf("\r\nlogin:");
        return W_ERR_OK;
    }
    wind_strcpy(ctx->pwd,ctx->buf);
    ctx->autherr_cnt = 0;
    ctx->stat = CSLSTAT_CMD;
    wind_printf("\r\n%s@%s>",ctx->user,BOARD_NAME);
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

static w_err_t spit_cmd(w_shell_ctx_s *ctx)
{
    w_err_t i;
    w_int32_t idx = 0;
    w_cmd_param_s *prm = &ctx->param;
    prm->argc = 0;
    for(i = 0;i < CMD_PARAM_CNT;i ++)
    {
        prm->argv[i] = W_NULL;
        idx = get_string(ctx->buf,idx,&prm->argv[i]);
        if(idx > 0)
            prm->argc ++;
        else
            break;
    }
    if(i >= CMD_PARAM_CNT)
        return W_ERR_FAIL;
    return W_ERR_OK;
}


static w_err_t execute_cmd(w_shell_ctx_s *ctx)
{
    w_err_t err;
    w_cmd_s *cmd;
    err = spit_cmd(ctx);
    if((err < 0) || (ctx->param.argc == 0))
        return err;
    if(wind_strcmp(ctx->param.argv[0],"?") == 0)
    {
        wind_cmd_print();
        return W_ERR_OK;
    }
    cmd = wind_cmd_get(ctx->param.argv[0]);
    if(cmd == W_NULL)
        return W_ERR_FAIL;
    if((ctx->param.argc >= 2)&&(wind_strcmp(ctx->param.argv[1],"?") == 0))
    {
        wind_printf("\r\ncmd %s usage:\r\n",cmd->name);
        cmd->showusage();
        return W_ERR_OK;
    }
    err = cmd->execute(ctx->param.argc,ctx->param.argv);
    if(err != W_ERR_OK)
    {
        wind_printf("\r\ncmd %s usage:\r\n",cmd->name);
        cmd->showusage();
    }
        
    return W_ERR_OK;
}


w_err_t thread_shell(w_int32_t argc,char **argv)
{
    w_int32_t len;
    w_int32_t index = 0;
    w_shell_ctx_s *ctx;
    if(argc >= 2)
        wind_to_uint32((w_uint8_t*)argv[1],(w_uint32_t*)&index);
    else 
        index = 0;
    WIND_ASSERT_RETURN(index >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(index < WIND_SHELL_CTX_COUNT,W_ERR_INVALID);
    ctx = &g_shell_ctx[index];
    cmd_history_init(&ctx->his);
    shell_stat_init(ctx);

    while(1)
    {
        len = shell_read_line(ctx,WIND_CMD_MAX_LEN);
        if(len >= 0)
        {
            switch(ctx->stat)
            {
#if USER_AUTH_ENABLE
                case CSLSTAT_USER:
                    check_user_account(ctx);
                    break;
                case CSLSTAT_PWD:
                    check_user_passwd(ctx);
                    break;
#endif 
                case CSLSTAT_CMD:
                    cmd_history_append(&ctx->his,ctx->buf);
                    execute_cmd(ctx);
#if USER_AUTH_ENABLE
                    if(wind_strcmp(ctx->buf,"exit") == 0)
                    {
                        ctx->stat = CSLSTAT_USER;
                        wind_printf("\r\nlogin:");
                    }
                    else
#endif
                        wind_printf("\r\n%s@%s>",ctx->user,BOARD_NAME);
                    break;
                default:
                    wind_printf("\r\nlogin:");
                    ctx->stat = CSLSTAT_USER;
                    break;
            }
        }
    }
    
}




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
