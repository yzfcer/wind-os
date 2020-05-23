/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2020/05/23
  *Description: wind-s log function source
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "wind_log.h"
#include "treefs.h"
#include "treefile.h"
#include "wind_debug.h"
#include "wind_string.h"

#if WIND_TREEFS_SUPPORT
static w_treefile_s *s_logfile = W_NULL;
static w_int32_t s_loglevel = WIND_LOG_NOTICE;
w_err_t wind_log_set_level(w_int32_t level)
{
    if((level <= WIND_LOG_DEBUG) && (level >= WIND_LOG_CRIT))
        s_loglevel = level;
    return W_ERR_OK;
}

w_loglevel_e wind_log_get_level(void)
{
    return s_loglevel;
}

w_err_t wind_log_print_level(w_loglevel_e level)
{
    char *levelstr[] = {"critical","error","warn","notice","info","trace","debug"};
    if(level < WIND_LOG_DEBUG || level > WIND_LOG_CRIT)
        return W_ERR_OK;
    wind_printf("%d : %s\r\n",level,levelstr[level]);
    return W_ERR_OK;
}


w_err_t wind_log_open(void)
{
    w_treefs_s *tfs;
    tfs = wind_treefs_get("tfs0");
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_FAIL);
    s_logfile = treefile_open(tfs,"/sys.log",TF_FMODE_W | TF_FMODE_CRT);
    WIND_ASSERT_RETURN(s_logfile != W_NULL,W_ERR_FAIL);
    treefile_seek(s_logfile,0xffffffff);
    return W_ERR_OK;
}

w_err_t wind_log_printf(w_loglevel_e level,const char *fmt,...)
{
    static char buff[256];
    wind_va_list args;
    w_int32_t count;
    WIND_ASSERT_RETURN(s_logfile != W_NULL,W_ERR_FAIL);
    if(level > s_loglevel)
        return W_ERR_OK;
    wind_memset(buff,0,sizeof(buff));
    wind_va_start(args, fmt);
    count = wind_vsprintf(buff, fmt, args);
    wind_va_end(args);
    treefile_write(s_logfile,(w_uint8_t*)buff,count);
    return W_ERR_OK;
}

w_err_t wind_log_close(void)
{
    WIND_ASSERT_RETURN(s_logfile != W_NULL,W_ERR_FAIL);
    treefile_close(s_logfile);
    s_logfile = W_NULL;
    return W_ERR_OK;
}
#endif
