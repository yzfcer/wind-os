#include "wind_debug.h"
#include "wind_string.h"
#include "cmd_history.h"




w_err_t cmd_history_init(cmd_his_s *his)
{
    w_int32_t i;
    his->hiscnt = 0;
    his->buf_used = 0;
    his->curidx = 0;
    his->curcmd = NULL;
    for(i =0 ;i < CMD_HISTORY_COUNT;i ++)
        his->hiscmd[i] = NULL;
    wind_memset(his->cmdbuf,0,CMD_HSIBUF_LENTH);
    return ERR_OK;
}

static w_err_t remove_old_history(cmd_his_s *his)
{
    w_int32_t i;
    w_int32_t len;
    if(his->hiscnt <= 0)
        return ERR_COMMAN;
    len = wind_strlen(his->hiscmd[0]);
    len += 1;
    wind_memcpy(his->cmdbuf,his->hiscmd[1],CMD_HSIBUF_LENTH - len);
    his->hiscnt -= 1;
    for(i = 0;i < his->hiscnt;i ++)
        his->hiscmd[i] = his->hiscmd[i+1];
    his->hiscmd[i] = NULL;
    his->curcmd = his->hiscmd[i-1];
    his->curidx = i - 1;
    his->buf_used -= len;
    return ERR_OK;
}

static w_err_t add_hiscmd(cmd_his_s *his,char *cmd)
{
    w_int32_t len;
    char *buf;
    if(his->curcmd == NULL)
        buf = his->cmdbuf;
    else
    {
        len = wind_strlen(his->curcmd);
        len += 1;
        buf = his->curcmd + len;
    }
    wind_strcpy(buf,cmd);
    his->hiscmd[his->hiscnt] = buf;
    his->hiscnt ++;
    his->curcmd = buf;
    his->curidx = his->hiscnt - 1;
    len = wind_strlen(cmd);
    len += 1;
    his->buf_used += len;
    return ERR_OK;
    
}

w_err_t cmd_history_append(cmd_his_s *his,char *cmd)
{
    w_err_t err;
    w_int32_t rest;
    w_int32_t len = wind_strlen(cmd);
    if(his->hiscnt >= CMD_HISTORY_COUNT)
        remove_old_history(his);
    rest = CMD_HSIBUF_LENTH - his->buf_used;
    while(rest < len + 1)
    {
        err = remove_old_history(his);
        if(err != ERR_OK)
            return err;
        rest = CMD_HSIBUF_LENTH - his->buf_used;
    }
    add_hiscmd(his,cmd);
    return ERR_COMMAN;
}

w_err_t cmd_history_get_next(cmd_his_s *his,char *cmd)
{
    his->curidx += 1;
    if(his->curidx >= his->hiscnt)
    {
        his->curidx = his->hiscnt;
        his->curcmd = NULL;
        cmd[0] = 0; 
        return ERR_COMMAN;
    }
    his->curcmd = his->hiscmd[his->curidx];
    if(his->curcmd == NULL)
        return ERR_COMMAN;
    wind_strcpy(cmd,his->curcmd);
    return ERR_OK;
}

w_err_t cmd_history_get_prev(cmd_his_s *his,char *cmd)
{
    his->curidx -= 1;
    if(his->curidx < 0)
    {
        his->curidx = -1;
        his->curcmd = NULL;
        cmd[0] = 0; 
        return ERR_COMMAN;
    }
    his->curcmd = his->hiscmd[his->curidx];
    if(his->curcmd == NULL)
        return ERR_COMMAN;
    wind_strcpy(cmd,his->curcmd);
    return ERR_OK;
}



