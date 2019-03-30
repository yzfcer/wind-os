/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_history.h
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
#ifndef CMD_HISTORY_H__
#define CMD_HISTORY_H__
#include "wind_config.h"
#include "wind_type.h"
#if WIND_SHELL_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif
#define CMD_HISTORY_COUNT 10
#define CMD_HISBUF_LENTH 256

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
