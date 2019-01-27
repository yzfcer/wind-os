/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_file.h
**创   建   人: 周江村
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
#include "treefs.h"
#include "wind_debug.h"
#include "wind_string.h"

#if WIND_FS_SUPPORT
static treefile_s *tf = W_NULL;
w_err_t wind_log_open(void)
{
    tf = treefile_open("/sys.log",TF_FMODE_W | TF_FMODE_CRT);
    WIND_ASSERT_RETURN(tf != W_NULL,W_ERR_FAIL);
    treefile_seek(tf,0xffffff);
    return W_ERR_OK;
}

w_err_t wind_log_printf(const char *fmt,...)
{
    static char buff[256];
    wind_va_list args;
    w_int32_t count;
    WIND_ASSERT_RETURN(tf != W_NULL,W_ERR_FAIL);
    wind_memset(buff,0,sizeof(buff));
    wind_va_start(args, fmt);
    count = wind_vsprintf(buff, fmt, args);
    wind_va_end(args);
    treefile_write(tf,(w_uint8_t*)buff,count);
    return W_ERR_OK;
}

w_err_t wind_log_close(void)
{
    WIND_ASSERT_RETURN(tf != W_NULL,W_ERR_FAIL);
    treefile_close(tf);
    tf = W_NULL;
    return W_ERR_OK;
}
#endif
