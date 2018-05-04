/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_softirq.h / wind_softirq.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ����ʵ��ϵͳ�����жϵĹ��ܣ����ж���Ҫ���ṩһ���ڸ����ȼ����е�����ģʽ���ڸ�ģ��ִ��
**              ִ���ڼ佫���ᱻ�������������жϣ���ȴ���Ա�Ӳ���ж�����ϵĸ����ȼ�ģʽ
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 2013.10.23δ�����
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_SOFTINT_H__
#define WIND_SOFTINT_H__

#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_SOFTIRQ_SUPPORT
typedef void (*softirq_fn)(void);

w_err_t _wind_softirq_init(void);
w_err_t _wind_create_softirq_thread(void);

w_err_t wind_softirq_reg(w_uint16_t irqid,softirq_fn func);
w_err_t wind_softirq_unreg(w_int32_t irqid);
w_err_t wind_softirq_trig(w_int32_t irqid);
w_err_t wind_softirq_int(w_int32_t irqid);

#else
#define _wind_softirq_init() ERR_OK;
#endif
#ifdef __cplusplus
}
#endif


#endif
