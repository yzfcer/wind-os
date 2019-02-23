/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_sysinfo.h / wind_sysinfo.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ��Ĵ���
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
#ifndef WIND_SYSINFO_H__
#define WIND_SYSINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
#define WIND_OS_VERSION 0x00010117
typedef struct 
{
    char *archname;
    char *cpuname;
    char *boardname;
    w_uint32_t os_ver;
    w_uint32_t hw_ver;
    w_uint32_t soft_ver;
}w_sysinfo_s;

w_err_t wind_sysinfo_init(void);

void _wind_os_print_logo(void);

void _wind_print_sysinfo(void);




#ifdef __cplusplus
}
#endif


#endif

