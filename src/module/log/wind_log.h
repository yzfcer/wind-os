/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName    :    wind_log.h
  *** Author      :      Jason Zhou
  *Version:     1.0
  *Date:        2020/05/23
  *Description: wind-s log function header
  *Others:  
  *History:  
     1.Date:
       ** Author      :
       Modification:
**********************************************************************************/
#ifndef WIND_LOG_H__
#define WIND_LOG_H__
#include "wind_config.h"
#include "wind_type.h"

#if WIND_MODULE_LOG_SUPPORT
typedef enum
{
    WIND_LOG_CRIT=0,
    WIND_LOG_ERROR,
    WIND_LOG_WARN,
    WIND_LOG_NOTICE,
    WIND_LOG_INFO,
    WIND_LOG_TRACE,
    WIND_LOG_DEBUG,
}w_loglevel_e;

#define wind_log_debug(fmt,...) do{wind_log_printf(WIND_LOG_DEBUG,"%s"fmt"\r\n","[debug] ",##__VA_ARGS__);}while(0)

#define wind_log_trace(fmt,...) do{wind_log_printf(WIND_LOG_TRACE,"%s"fmt"\r\n","[trace] ",##__VA_ARGS__);}while(0)

#define wind_log_info(fmt,...) do{wind_log_printf(WIND_LOG_INFO,"%s"fmt"\r\n","[info] ",##__VA_ARGS__);}while(0)

#define wind_log_notice(fmt,...) do{wind_log_printf(WIND_LOG_NOTICE,"%s"fmt"\r\n","[notice] ",##__VA_ARGS__);}while(0)

#define wind_log_warn(fmt,...) do{wind_log_printf(WIND_LOG_WARN,"%s[%s,%d] "fmt"\r\n","[warn] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

#define wind_log_error(fmt,...) do{wind_log_printf(WIND_LOG_ERROR,"%s[%s,%d] "fmt"\r\n","[error] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

#define wind_log_critical(fmt,...) do{wind_log_printf(WIND_LOG_CRIT,"%s[%s,%d] "fmt"\r\n","[critical] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

w_err_t wind_log_set_level(w_int32_t level);
w_loglevel_e wind_log_get_level(void);
w_err_t wind_log_print_level(w_loglevel_e level);

w_err_t wind_log_open(void);
w_err_t wind_log_printf(w_loglevel_e level,const char *fmt,...);
w_err_t wind_log_close(void);

#endif
#endif

