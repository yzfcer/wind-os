#include "wind_debug.h"
#include "wind_string.h"
#include "cmd_history.h"
#include "wind_cmd.h"

void cmd_history_print(cmd_his_s *his)
{
    int i;
    for(i = 0;i < CMD_HISTORY_COUNT;i ++)
    {
        console_printf("cmd:%s\r\n",his->hiscmd[i]);
    }
}

w_err_t cmd_history_init(cmd_his_s *his)
{
    w_int32_t i;
    his->hiscnt = 0;
    his->buf_used = 0;
    his->curidx = -1;
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
    len = wind_strlen(his->hiscmd[0])+1;
    if(his->hiscnt > 1)
    {
        wind_memcpy(&his->cmdbuf[0],his->hiscmd[1],his->buf_used-len);
        for(i = 0;i < his->hiscnt -1 ;i ++)
            his->hiscmd[i] = his->hiscmd[i+1] - len;
    }
    his->hiscmd[his->hiscnt-1] = NULL;
    his->hiscnt -= 1;
    his->buf_used -= len;
    wind_memset(&his->cmdbuf[his->buf_used],0,len);
    his->curidx =his->hiscnt - 1;
    his->curcmd = his->hiscmd[his->curidx];
    return ERR_OK;
}

static w_err_t add_hiscmd(cmd_his_s *his,char *cmd)
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
    //console_printf("add cmd:%s\r\n",cmd);
    //cmd_history_print(his);
    return ERR_OK;
    
}

w_err_t cmd_history_append(cmd_his_s *his,char *cmd)
{
    w_err_t err;
    w_int32_t rest;
    w_int32_t len;
    if(his == NULL || cmd == NULL)
        return ERR_NULL_POINTER;
    if(cmd[0] == 0 || cmd[0] == 0x1b)
        return ERR_INVALID_PARAM;
    len = wind_strlen(cmd);
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
    //console_printf("append cmd:%s\r\n",cmd);
    add_hiscmd(his,cmd);
    return ERR_COMMAN;
}

w_err_t cmd_history_get_next(cmd_his_s *his,char *cmd)
{
    if(his->curidx < his->hiscnt - 1)
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
    //console_printf("next cmd:%s\r\n",his->curcmd);
    wind_strcpy(cmd,his->curcmd);
    
    return ERR_OK;
}

w_err_t cmd_history_get_prev(cmd_his_s *his,char *cmd)
{
    
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
    //console_printf("prev cmd:%s\r\n",his->curcmd);
    wind_strcpy(cmd,his->curcmd);
    if(his->curidx > 0)
        his->curidx -= 1;
    return ERR_OK;
}



