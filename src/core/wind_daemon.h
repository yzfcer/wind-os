/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_daemon.h
** FileName    : wind_daemon.h
** Author      : Jason Zhou
** Last Date   : 2019.02.23
** Description : wind-os thread guard function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.02.23
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_DAEMON_H__
#define WIND_DAEMON_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_DAEMON_SUPPORT
#define WIND_DAEMON_MAGIC 0x576C5A4C


typedef struct __w_daemon_s w_daemon_s;
typedef w_err_t (*w_daemon_fn)(void);



struct __w_daemon_s
{
    w_obj_s obj;            //Basic object information
    w_daemon_fn daemon_func;//Daemon function
};


w_err_t _wind_daemon_mod_init(void);
w_daemon_s *wind_daemon_get(const char *name);
w_err_t wind_daemon_init(w_daemon_s *daemon,const char *name,w_daemon_fn daemon_func);
w_daemon_s *wind_daemon_create(const char *name,w_daemon_fn daemon_func);

w_err_t wind_daemon_destroy(w_daemon_s *daemon);

w_err_t _wind_daemon_period_check(void);

w_err_t wind_daemon_print_list(void);


#endif // #if WIND_DAEMON_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif  // #ifndef WIND_DAEMON_H__

