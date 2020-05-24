/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_history.h
** Author      : 周江村
** Last Date   : 
** Description : 处理控制台的历史命令记录
**              
**--------------History---------------------------------------------------------------------------------
Author: 周江村
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
#ifndef CMD_HISTORY_H__
#define CMD_HISTORY_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_MODULE_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif
#define CMD_HISTORY_COUNT 10
#define CMD_HISBUF_LENTH 1024

struct __cmd_his_s;
typedef struct __w_cmd_his_s w_cmd_his_s;
struct __w_cmd_his_s
{
    w_int32_t hiscnt;
    w_int32_t buf_used;
    
    w_int32_t curidx;
    char *curcmd;
    
    char *hiscmd[CMD_HISTORY_COUNT];
    char cmdbuf[CMD_HISBUF_LENTH];
};


w_err_t cmd_history_init(w_cmd_his_s *his);
w_err_t cmd_history_append(w_cmd_his_s *his,char *cmd);
w_err_t cmd_history_get_next(w_cmd_his_s *his,char *cmd);
w_err_t cmd_history_get_prev(w_cmd_his_s *his,char *cmd);

#ifdef __cplusplus
}
#endif
#endif
#endif
