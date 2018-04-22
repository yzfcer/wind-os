/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_debug.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ������Ϣ�������ʽ
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "wind_type.h"
#include "wind_debug.h"
#if WIND_DEBUG_SUPPORT
#ifdef _USE_USER_PRINT
#include "wind_std.h"
extern w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
w_int32_t wind_printf(const char *fmt, ...) 
{ 
    static char sprint_buf[512];
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
#endif


