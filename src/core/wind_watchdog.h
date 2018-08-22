/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_watchdog.h
**��   ��   ��: �ܽ���
**����޸�����: 2018.08.12
**��        ��: �߳����ȵĿ��Ź�����Ҫ�Ǽ���̵߳ò���CPU������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2018.08.12
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
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
#define WIND_WATCHDOG_MAGIC 0x33FA81

#define WDOG_WARN  0x00
#define WDOG_RESET 0x01

typedef struct _wind_watchdog
{
    w_uint32_t magic:24;
    w_uint32_t flag;
    const char* name;
    dnode_s watchdognode;
    w_int16_t time_max;    //��ʼ�����ź�����ֵ
    w_int16_t time_cur;    //��ǰ���ź�����ֵ
	thread_s *thread;
}watchdog_s;

w_err_t _wind_watchdog_init(void);
watchdog_s *wind_watchdog_get(const char *name);
watchdog_s *wind_watchdog_create(const char *name,w_uint32_t flag,w_int16_t timeout_1s);
w_err_t wind_watchdog_destroy(watchdog_s *watchdog);
w_err_t wind_watchdog_feed(watchdog_s *watchdog);
w_err_t wind_watchdog_print(dlist_s *list);
#else
#define _wind_watchdog_init() 
#endif
#ifdef __cplusplus
}
#endif
#endif//#ifndef WIND_WATCHDOG_H__

