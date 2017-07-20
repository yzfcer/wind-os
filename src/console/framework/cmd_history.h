#ifndef CMD_HISTORY_H__
#define CMD_HISTORY_H__
#include "wind_types.h"
#define CMD_HISTORY_COUNT 10
#define CMD_HSIBUF_LENTH 256
struct __cmd_his_s;
typedef struct __cmd_his_s cmd_his_s;
struct __cmd_his_s
{
    w_int32_t hiscnt;
    w_int32_t buf_used;
    w_int32_t curidx;
    char *curcmd;
    char *hiscmd[CMD_HISTORY_COUNT];
    char cmdbuf[CMD_HSIBUF_LENTH];
};


w_err_t cmd_history_init(cmd_his_s *his);
w_err_t cmd_history_append(cmd_his_s *his,char *cmd);
w_err_t cmd_history_get_next(cmd_his_s *his,char *cmd);
w_err_t cmd_history_get_prev(cmd_his_s *his,char *cmd);

#endif
