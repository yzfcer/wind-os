/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_log.h
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 日志保存功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LOG_H__
#define WIND_LOG_H__
#include "wind_config.h"
#include "wind_type.h"

#if WIND_MODULE_VFS_SUPPORT

#define wind_log_debug(fmt,...) do{wind_log_printf("%s"fmt"\r\n","[debug] ",##__VA_ARGS__);}while(0)

#define wind_log_notice(fmt,...) do{wind_log_printf("%s"fmt"\r\n","[notice] ",##__VA_ARGS__);}while(0)

#define wind_log_warn(fmt,...) do{wind_log_printf("%s[%s,%d] "fmt"\r\n","[warn] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

#define wind_log_error(fmt,...) do{wind_log_printf("%s[%s,%d] "fmt"\r\n","[error] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

#define wind_log_critical(fmt,...) do{wind_log_printf("%s[%s,%d] "fmt"\r\n","[critical] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)

w_err_t wind_log_open(void);
w_err_t wind_log_printf(const char *fmt,...);
w_err_t wind_log_close(void);
#endif
#endif

