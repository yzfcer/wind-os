/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_log.h
**��   ��   ��: Jason Zhou
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
#ifndef WIND_FILE_H__
#define WIND_FILE_H__
#include "wind_config.h"
#include "wind_type.h"

#if WIND_FS_SUPPORT

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

