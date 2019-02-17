/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_watchdog.h
**��   ��   ��: Jason Zhou
**����޸�����: 2018.08.12
**��        ��: �߳����ȵĿ��Ź�����Ҫ�Ǽ���̵߳ò���CPU������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018.08.12
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2018.08.12
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_WATCHDOG_H__
#define WIND_WATCHDOG_H__


#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_WATCHDOG_SUPPORT
#define WIND_WATCHDOG_MAGIC 0x33FA81A6

#define WDOG_WARN  0x00
#define WDOG_RESET 0x01

#define F_WATCHDOG_POOL 0x01 
#define IS_F_WATCHDOG_POOL(watchdog) ((watchdog->flag & F_WATCHDOG_POOL) == F_WATCHDOG_POOL)
#define SET_F_WATCHDOG_POOL(watchdog) (watchdog->flag |= F_WATCHDOG_POOL)
#define CLR_F_WATCHDOG_POOL(watchdog) (watchdog->flag &= (~F_WATCHDOG_POOL))

#define F_WATCHDOG_WARN 0x02
#define IS_F_WATCHDOG_WARN(watchdog) ((watchdog->flag & F_WATCHDOG_WARN) == F_WATCHDOG_WARN)
#define SET_F_WATCHDOG_WARN(watchdog) (watchdog->flag |= F_WATCHDOG_WARN)
#define CLR_F_WATCHDOG_WARN(watchdog) (watchdog->flag &= (~F_WATCHDOG_WARN))

#define F_WATCHDOG_RESET 0x03
#define IS_F_WATCHDOG_RESET(watchdog) ((watchdog->flag & F_WATCHDOG_RESET) == F_WATCHDOG_RESET)
#define SET_F_WATCHDOG_RESET(watchdog) (watchdog->flag |= F_WATCHDOG_RESET)
#define CLR_F_WATCHDOG_RESET(watchdog) (watchdog->flag &= (~F_WATCHDOG_RESET))

typedef struct _wind_watchdog
{
    w_uint32_t magic;
    const char* name;
    w_dnode_s watchdognode;
    w_uint32_t flag;
    w_int16_t time_max;    //��ʼ�����ź�����ֵ
    w_int16_t time_cur;    //��ǰ���ź�����ֵ
	w_thread_s *thread;
}w_watchdog_s;

w_err_t _wind_watchdog_mod_init(void);
w_watchdog_s *wind_watchdog_get(const char *name);
w_err_t wind_watchdog_init(w_watchdog_s *watchdog,const char *name,w_int16_t timeout_1s,w_uint32_t flag);
w_watchdog_s *wind_watchdog_create(const char *name,w_int16_t timeout_1s,w_uint32_t flag);
w_err_t wind_watchdog_destroy(w_watchdog_s *watchdog);
w_err_t wind_watchdog_feed(w_watchdog_s *watchdog);
w_err_t wind_watchdog_print(void);

#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_WATCHDOG_H__

