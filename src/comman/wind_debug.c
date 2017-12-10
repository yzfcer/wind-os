/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "wind_type.h"
#include "wind_debug.h"

#ifdef _USE_USER_PRINT
#include "wind_os_hwif.h"
extern w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
w_int32_t wind_printf(const char *fmt, ...) 
{ 
    static char sprint_buf[1024];
    va_list args; 
    w_int32_t n;
    wind_close_interrupt();
    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    sprint_buf[n] = 0;
    va_end(args);
    wind_std_output((w_uint8_t *)sprint_buf, n);
    wind_open_interrupt();
    return n; 
}
#endif


