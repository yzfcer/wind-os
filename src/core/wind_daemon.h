/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_daemon.h
**文   件   名: wind_daemon.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.02.23
**描        述: wind os的线程守护对象
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.02.23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
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

#define F_DAEMON_POOL (0x01 << 0) //标记daemon对象是否通过内存池分配
#define IS_F_DAEMON_POOL(daemon) ((daemon->obj.flag & F_DAEMON_POOL) == F_DAEMON_POOL)
#define SET_F_DAEMON_POOL(daemon) (daemon->obj.flag |= F_DAEMON_POOL)
#define CLR_F_DAEMON_POOL(daemon) (daemon->obj.flag &= (~F_DAEMON_POOL))

#define F_DAEMON_ENABLE (0x01 << 1) //标记daemon对象是否使能
#define IS_F_DAEMON_ENABLE(daemon) ((daemon->obj.flag & F_DAEMON_ENABLE) == F_DAEMON_ENABLE)
#define SET_F_DAEMON_ENABLE(daemon) (daemon->obj.flag |= F_DAEMON_ENABLE)
#define CLR_F_DAEMON_ENABLE(daemon) (daemon->obj.flag &= (~F_DAEMON_ENABLE))


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

w_err_t wind_daemon_setflag(w_daemon_s *daemon,w_int16_t flag);
w_err_t wind_daemon_clrflag(w_daemon_s *daemon,w_int16_t flag);

w_err_t _wind_daemon_period_check(void);

w_err_t wind_daemon_print_list(void);


#endif // #if WIND_DAEMON_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif  // #ifndef WIND_DAEMON_H__

