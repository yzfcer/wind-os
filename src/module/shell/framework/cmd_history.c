/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_history.c
** Author      : Jason Zhou
** Last Date   : 
** Description : 处理控制台的历史命令记录
**              
**--------------History---------------------------------------------------------------------------------
Author: Jason Zhou
** Version     : v1.0
** Date        : 2013.08.10
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_debug.h"
#include "wind_string.h"
#include "cmd_history.h"
#include "wind_cmd.h"
#if WIND_MODULE_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif

w_err_t cmd_history_print(w_cmd_his_s *his)
{
    int i;
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < CMD_HISTORY_COUNT;i ++)
    {
        wind_printf("cmd:%s\r\n",his->hiscmd[i]);
    }
    return W_ERR_OK;
}

w_err_t cmd_history_init(w_cmd_his_s *his)
{
    w_int32_t i;
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_PTR_NULL);
    his->hiscnt = 0;
    his->buf_used = 0;
    his->curidx = -1;
    his->curcmd = W_NULL;
    for(i =0 ;i < CMD_HISTORY_COUNT;i ++)
        his->hiscmd[i] = W_NULL;
    wind_memset(his->cmdbuf,0,CMD_HISBUF_LENTH);
    return W_ERR_OK;
}


static w_int32_t get_cmd_index(w_cmd_his_s *his,char *cmd)
{
    w_int32_t i;
    WIND_ASSERT_RETURN(his != W_NULL,-1);
    WIND_ASSERT_RETURN(cmd != W_NULL,-1);
    for(i = 0;i < his->hiscnt;i ++)
    {
        if(wind_strcmp(his->hiscmd[i],cmd) == 0)
            return i;
    }
    return -1;
}

static w_err_t remove_cmd_index(w_cmd_his_s *his,w_int32_t index)
{
    w_int32_t i,cmdlen,mvlen;
    WIND_ASSERT_RETURN(his != W_NULL,-1);
    cmdlen = wind_strlen(his->hiscmd[index])+1;
	mvlen = (CMD_HISBUF_LENTH - cmdlen) - (w_int32_t)((w_addr_t)&his->hiscmd[index] - (w_addr_t)&his->hiscmd[0]);
    wind_memcpy(his->hiscmd[index],his->hiscmd[index]+cmdlen,mvlen);
    for(i = index;i < his->hiscnt - 1 ;i ++)
        his->hiscmd[i] = his->hiscmd[i+1] - cmdlen;
    his->hiscmd[his->hiscnt-1] = W_NULL;
    his->hiscnt -= 1;
    his->buf_used -= cmdlen;
    wind_memset(&his->cmdbuf[his->buf_used],0,cmdlen);
    his->curidx = his->hiscnt - 1;
    his->curcmd = his->hiscmd[his->curidx];
    return W_ERR_OK;
}

static w_err_t remove_exist_cmd(w_cmd_his_s *his,char *cmd)
{
    w_int32_t index;
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_INVALID);
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_INVALID);
    if(his->hiscnt <= 0)
        return W_ERR_OK;
    index = get_cmd_index(his,cmd);
    if(index < 0)
        return W_ERR_OK;
    return remove_cmd_index(his,index);
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
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(cmd[0] != 0,W_ERR_INVALID);
    WIND_CHECK_RETURN(cmd[0] != 0x1b,W_ERR_INVALID);
    len = wind_strlen(cmd);
    WIND_CHECK_RETURN(len < CMD_HISBUF_LENTH,W_ERR_FAIL);
    
    remove_exist_cmd(his,cmd);
    if(his->hiscnt >= CMD_HISTORY_COUNT)
        remove_cmd_index(his,0);
    rest = CMD_HISBUF_LENTH - his->buf_used;
    while(rest < len + 1)
    {
        err = remove_cmd_index(his,0);
        WIND_ASSERT_RETURN(err == W_ERR_OK,err);
        rest = CMD_HISBUF_LENTH - his->buf_used;
    }
    add_hiscmd(his,cmd);
    //cmd_history_print(his);
    return W_ERR_FAIL;
}

w_err_t cmd_history_get_next(w_cmd_his_s *his,char *cmd)
{
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_PTR_NULL);
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
    WIND_CHECK_RETURN(his->curcmd != W_NULL,W_ERR_FAIL);
    wind_strcpy(cmd,his->curcmd);
    return W_ERR_OK;
}

w_err_t cmd_history_get_prev(w_cmd_his_s *his,char *cmd)
{
    
    WIND_ASSERT_RETURN(his != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cmd != W_NULL,W_ERR_PTR_NULL);
    if(his->curidx < 0)
    {
        his->curidx = -1;
        his->curcmd = W_NULL;
        cmd[0] = 0; 
        return W_ERR_FAIL;
    }
    his->curcmd = his->hiscmd[his->curidx];
    WIND_CHECK_RETURN(his->curcmd != W_NULL,W_ERR_FAIL);
    wind_strcpy(cmd,his->curcmd);
    if(his->curidx > 0)
        his->curidx -= 1;
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif
#endif
