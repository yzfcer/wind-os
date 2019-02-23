/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_sysinfo.h / wind_sysinfo.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的核心代码
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

