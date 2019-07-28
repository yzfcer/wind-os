/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_file.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ��־���湦��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "treefs.h"
#include "treefile.h"
#include "wind_debug.h"
#include "wind_string.h"

#if WIND_TREEFS_SUPPORT
static w_treefile_s *logfile = W_NULL;
w_err_t wind_log_open(void)
{
    w_treefs_s *tfs;
    tfs = wind_treefs_get("tfs0");
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_FAIL);
    logfile = treefile_open(tfs,"/sys.log",TF_FMODE_W | TF_FMODE_CRT);
    WIND_ASSERT_RETURN(logfile != W_NULL,W_ERR_FAIL);
    treefile_seek(logfile,0xffffff);
    return W_ERR_OK;
}

w_err_t wind_log_printf(const char *fmt,...)
{
    static char buff[256];
    wind_va_list args;
    w_int32_t count;
    WIND_ASSERT_RETURN(logfile != W_NULL,W_ERR_FAIL);
    wind_memset(buff,0,sizeof(buff));
    wind_va_start(args, fmt);
    count = wind_vsprintf(buff, fmt, args);
    wind_va_end(args);
    treefile_write(logfile,(w_uint8_t*)buff,count);
    return W_ERR_OK;
}

w_err_t wind_log_close(void)
{
    WIND_ASSERT_RETURN(logfile != W_NULL,W_ERR_FAIL);
    treefile_close(logfile);
    logfile = W_NULL;
    return W_ERR_OK;
}
#endif
