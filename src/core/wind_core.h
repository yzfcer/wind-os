/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_core.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�ĺ����������̵߳��ȹ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CORE_H__
#define WIND_CORE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_thread.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct __w_core_var_s
{
    w_bool_t usrthren;/*�û��߳��������ı�־ */
    volatile w_uint32_t idle_cnt;//���м�����
    w_uint32_t ticks_cnt;//tick��ʱ
    w_uint32_t idle_cnt_max;//��һ��ʱ���ڵ�idle����ļ���ֵ
    w_uint32_t cpu_usage;
}w_core_var_s;
extern w_core_var_s g_core;//�ں���صĲ�����
extern volatile w_bool_t gwind_start_flag;//��ʼ���ȵı�־
extern w_stack_t **gwind_high_stack;
extern w_stack_t **gwind_cur_stack;

void _wind_corevar_init(void);

#define WIND_CPU_USAGE (g_core.cpu_usage)
#define IDLE_CNT_PER_SEC (g_core.idle_cnt_max)
#define RUN_FLAG (gwind_start_flag)
#define TICKS_CNT (g_core.ticks_cnt)



void _wind_thread_dispatch(void);
void _wind_switchto_thread(w_thread_s *thread);

void wind_disable_switch(void);
void wind_enable_switch(void);

void wind_disable_interrupt(void);
void wind_enable_interrupt(void);

void wind_enter_irq(void);
void wind_exit_irq(void);


#ifdef __cplusplus
}
#endif

#endif

