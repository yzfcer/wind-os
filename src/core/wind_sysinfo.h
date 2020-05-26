/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_sysinfo.h / wind_sysinfo.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os system infomation
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_SYSINFO_H__
#define WIND_SYSINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_SYSINFO_H__

