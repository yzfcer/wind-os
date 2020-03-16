/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_softirq.h / wind_softirq.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: ����ʵ��ϵͳ�����жϵĹ��ܣ����ж���Ҫ���ṩһ���ڸ����ȼ����е�����ģʽ���ڸ�ģ��ִ��
**              ִ���ڼ佫���ᱻ�������������жϣ���ȴ���Ա�Ӳ���ж�����ϵĸ����ȼ�ģʽ
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
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
#endif // #ifdef __cplusplus


#if WIND_SOFTIRQ_SUPPORT
typedef void (*w_softirq_fn)(void);

w_err_t _wind_softirq_mod_init(void);
w_err_t _wind_create_thread_softirq(void);

w_err_t wind_softirq_reg(w_uint16_t irqid,w_softirq_fn func);
w_err_t wind_softirq_unreg(w_int32_t irqid);
w_err_t wind_softirq_trig(w_int32_t irqid);
w_err_t wind_softirq_int(w_int32_t irqid);

#endif //if WIND_SOFTIRQ_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_SOFTINT_H__
