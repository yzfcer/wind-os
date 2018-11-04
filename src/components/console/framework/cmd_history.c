/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_history.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 处理控制台的历史命令记录
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.08.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_debug.h"
#include "wind_string.h"
#include "cmd_history.h"
#include "wind_cmd.h"
#if WIND_CONSOLE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

void cmd_history_print(w_cmd_his_s *his)
{
    int i;
    for(i = 0;i < CMD_HISTORY_COUNT;i ++)
    {
        console_printf("cmd:%s\r\n",his->hiscmd[i]);
    }
}

w_err_t cmd_history_init(w_cmd_his_s *his)
{
    w_int32_t i;
    his->hiscnt = 0;
    his->buf_used = 0;
    his->curidx = -1;
    his->curcmd = W_NULL;
    for(i =0 ;i < CMD_HISTORY_COUNT;i ++)
        his->hiscmd[i] = W_NULL;
    wind_memset(his->cmdbuf,0,CMD_HSIBUF_LENTH);
    return W_ERR_OK;
}

static w_err_t remove_old_history(w_cmd_his_s *his)
{
    w_int32_t i;
    w_int32_t len;
    if(his->hiscnt <= 0)
        return W_ERR_FAIL;
    len = wind_strlen(his->hiscmd[0])+1;
    if(his->hiscnt > 1)
    {
        wind_memcpy(&his->cmdbuf[0],his->hiscmd[1],his->buf_used-len);
        for(i = 0;i < his->hiscnt -1 ;i ++)
            his->hiscmd[i] = his->hiscmd[i+1] - len;
    }
    his->hiscmd[his->hiscnt-1] = W_NULL;
    his->hiscnt -= 1;
    his->buf_used -= len;
    wind_memset(&his->cmdbuf[his->buf_used],0,len);
    his->curidx =his->hiscnt - 1;
    his->curcmd = his->hiscmd[his->curidx];
    return W_ERR_OK;
}

static w_err_t remove_exist_cmd(w_cmd_his_s *his,char *cmd)
{
    w_int32_t i,j,len;
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_INVALID);
    if(his->hiscnt <= 0)
        return W_ERR_OK;
    for(i = 0;i < his->hiscnt;i ++)
    {
        if(wind_strcmp(his->hiscmd[i],cmd) == 0)
        {
            len = wind_strlen(his->hiscmd[i])+1;
            wind_memcpy(&his->hiscmd[i],his->hiscmd[i+1],his->buf_used-(w_uint32_t)his->hiscmd[i+1]);
            for(j = i + 1;j < his->hiscnt -1 ;j ++)
                his->hiscmd[i] = his->hiscmd[i+1] - len;
            his->hiscmd[his->hiscnt-1] = W_NULL;
            his->hiscnt -= 1;
            his->buf_used -= len;
            wind_memset(&his->cmdbuf[his->buf_used],0,len);
            his->curidx =his->hiscnt - 1;
            his->curcmd = his->hiscmd[his->curidx];
            break;
        }
    }
    
    return W_ERR_OK;
}


static w_err_t add_hiscmd(w_cmd_his_s *his,char *cmd)
{
    w_int32_t len;
    char *buf;
    
    len = wind_strlen(cmd)+1;
    buf = &his->cmdbuf[his->buf_used];
    wind_memcpy(buf,cmd,len);
    his->hiscmd[his->hiscnt] = buf;
    his->buf_used += len;
    his->hiscnt ++;
    his->curcmd = buf;
    his->curidx = his->hiscnt - 1;
    return W_ERR_OK;
}

w_err_t cmd_history_append(w_cmd_his_s *his,char *cmd)
{
    w_err_t err;
    w_int32_t rest;
    w_int32_t len;
    if(his == W_NULL || cmd == W_NULL)
        return W_ERR_PTR_NULL;
    if(cmd[0] == 0 || cmd[0] == 0x1b)
        return W_ERR_INVALID;
    remove_exist_cmd(his,cmd);
    len = wind_strlen(cmd);
    if(his->hiscnt >= CMD_HISTORY_COUNT)
        remove_old_history(his);
    rest = CMD_HSIBUF_LENTH - his->buf_used;
    while(rest < len + 1)
    {
        err = remove_old_history(his);
        if(err != W_ERR_OK)
            return err;
        rest = CMD_HSIBUF_LENTH - his->buf_used;
    }
    add_hiscmd(his,cmd);
    return W_ERR_FAIL;
}

w_err_t cmd_history_get_next(w_cmd_his_s *his,char *cmd)
{
    if(his->curidx < his->hiscnt - 1)
        his->curidx += 1;
    if(his->curidx >= his->hiscnt)
    {
        his->curidx = his->hiscnt;
        his->curcmd = W_NULL;
        cmd[0] = 0; 
        return W_ERR_FAIL;
    }
    his->curcmd = his->hiscmd[his->curidx];
    if(his->curcmd == W_NULL)
        return W_ERR_FAIL;
    wind_strcpy(cmd,his->curcmd);
    return W_ERR_OK;
}

w_err_t cmd_history_get_prev(w_cmd_his_s *his,char *cmd)
{
    
    if(his->curidx < 0)
    {
        his->curidx = -1;
        his->curcmd = W_NULL;
        cmd[0] = 0; 
        return W_ERR_FAIL;
    }
    his->curcmd = his->hiscmd[his->curidx];
    if(his->curcmd == W_NULL)
        return W_ERR_FAIL;
    wind_strcpy(cmd,his->curcmd);
    if(his->curidx > 0)
        his->curidx -= 1;
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif
#endif
