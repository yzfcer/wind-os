/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_ticktimer.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os����tickΪ���ȵĴ���ʱ���Ĵ��룬�Ǿ�ȷ��ʱ������ʱ�����Ϊһ��tick����
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TIMER_H__
#define WIND_TIMER_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#ifdef __cplusplus
extern "C" {
#endif

#if WIND_TIMER_SUPPORT
#define TIMER_PERIOD 10
#define WIND_TIMER_MAGIC 0x34F574B2
typedef void (*w_timer_fn)(void *arg);
typedef struct __w_timer_s
{
    w_uint32_t magic;
    const char *name;
    w_dnode_s timernode;
    w_uint32_t value;
    w_uint32_t period;
    w_uint32_t flag_running:1;
    w_uint32_t flag_repeat:1;
    w_timer_fn handle;
    void *arg;
}w_timer_s;

w_err_t _wind_timer_mod_init(void);
void _wind_timer_event(void);
w_timer_s* wind_timer_get(char *name);

w_timer_s* wind_timer_create(const char *name,
                            w_uint32_t period_ms,
                            w_timer_fn func,
                            void *arg,
                            w_uint32_t flag_run,
                            w_uint32_t flag_repeat);
w_err_t wind_timer_start(w_timer_s* timer);
w_err_t wind_timer_stop(w_timer_s* timer);
w_err_t wind_timer_destroy(w_timer_s* timer);
w_err_t wind_timer_set_period(w_timer_s* timer,w_uint32_t period_ms);
w_err_t wind_timer_print(w_dlist_s *list);

#endif //#if WIND_TIMER_SUPPORT
#ifdef __cplusplus
}
#endif

#endif //#define WIND_TICKTIMER_H__

