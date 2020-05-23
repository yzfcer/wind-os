/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_log.c
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
//#include "treefs.h"
//#include "treefile.h"
#include "wind_file.h"
#include "wind_debug.h"
#include "wind_string.h"

#if WIND_MODULE_LOG_SUPPORT
#define LOG_FILE "/sys.log"
static w_file_s *s_logfile = W_NULL;
static w_int32_t s_loglevel = WIND_LOG_NOTICE;
w_err_t wind_log_set_level(w_int32_t level)
{
    if(level < WIND_LOG_CRIT)
        level = WIND_LOG_CRIT;
    if(level > WIND_LOG_DEBUG)
        level = WIND_LOG_DEBUG;
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
    if(level > WIND_LOG_DEBUG || level < WIND_LOG_CRIT)
        return W_ERR_OK;
    wind_printf("%d : %s\r\n",level,levelstr[level]);
    return W_ERR_OK;
}


w_err_t wind_log_open(void)
{
    WIND_ASSERT_RETURN(s_logfile == W_NULL,W_ERR_FILE_OPENED);
    s_logfile = wind_fopen(LOG_FILE,FMODE_W | FMODE_CRT);
    WIND_ASSERT_RETURN(s_logfile != W_NULL,W_ERR_FAIL);
    wind_fseek(s_logfile,0xffffffff);
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
    wind_fwrite(s_logfile,(w_uint8_t*)buff,count);
    return W_ERR_OK;
}

w_err_t wind_log_close(void)
{
    WIND_ASSERT_RETURN(s_logfile != W_NULL,W_ERR_FAIL);
    wind_fclose(s_logfile);
    s_logfile = W_NULL;
    return W_ERR_OK;
}
#endif #if WIND_MODULE_LOG_SUPPORT
