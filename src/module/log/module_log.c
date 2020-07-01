/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : module_log.c
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2020/05/23
** Description : wind-s log module entry
** Others      : 
** Modify      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/

#include "wind_module.h"
#include "wind_log.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




#if (WIND_MODULE_LOG_SUPPORT)

















MODULE_INIT(log)
{
    w_err_t err;
    err = wind_log_open();
	return err;//_wind_log_mod_init();
}

MODULE_EXIT(log)
{
    w_err_t err;
    err = wind_log_close();
	return err;
}

MODULE_DEF(log, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
