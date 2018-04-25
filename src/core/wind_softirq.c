/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_softirq.h / wind_softirq.c
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ����ʵ��ϵͳ�����жϵĹ��ܣ����ж���Ҫ���ṩһ���ڸ����ȼ����е�����ģʽ���������������
**              �̵߳�������ȼ�����ִ������ʱ�������Լ����������򲻻ᱻ�κ��������̴߳���ڸ�ģ��ִ��
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
#include "wind_config.h"
#include "wind_type.h"
#include "wind_softirq.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_time.h"
#include "wind_core.h"
#include "wind_os_hwif.h"
#if WIND_SOFTIRQ_SUPPORT
#define WIND_SOFTINT_STK_LEN 256 //���ж��̵߳Ķ�ջ���
//���ж��̵߳Ķ�ջ
static w_stack_t softirq_stk[WIND_SOFTINT_STK_LEN];

static w_uint16_t softirq_index = 0;
static thread_s *softirq_thread = NULL;

//static w_handle_t softirq_handle = -1;
softirq_fn wind_soft_vectors[WIND_SOFTINT_MAX_NUM];
//��ʼ�����жϵ�һЩ��ز���
void _wind_softirq_init(void)
{
    wind_memset(softirq_stk,0,WIND_SOFTINT_STK_LEN *sizeof(w_stack_t));
}


//�����ж�ģ��ע��һ���ж�������Ӧ����
w_int32_t wind_softirq_reg(softirq_fn func)
{
    w_int32_t i;
    w_int32_t irq_id = -1;
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
    {
        if(wind_soft_vectors[i] == func)
        {
            return i;
        }
    }
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
    {
        if(wind_soft_vectors[i] == NULL)
        {
            irq_id = i;
            wind_soft_vectors[i] = func;
            break;
        }
    }
    return irq_id;
}

//ȡ��һ�����жϵ�ע��
w_err_t wind_softirq_unreg(w_int32_t irq_id)
{
    if(irq_id < 0 || irq_id >= WIND_SOFTINT_MAX_NUM)
        return ERR_PARAM_OVERFLOW;
    wind_soft_vectors[irq_id] = NULL;
    return ERR_OK;
}

//����һ������ж�
void wind_softirq_trig(w_int32_t irq_id)
{
    wind_disable_interrupt();
    softirq_index = irq_id;
    softirq_thread->runstat = THREAD_STATUS_READY;
    wind_enable_interrupt();
    _wind_switchto_thread(softirq_thread);
}

static w_err_t wind_softirq_thread(w_int32_t argc,w_int8_t **argv)
{
    w_int32_t i;
    
    for(i = 0;i < WIND_SOFTINT_MAX_NUM;i ++)
        wind_soft_vectors[i] = NULL;
    while(1)
    {
        wind_disable_interrupt();
        softirq_thread->runstat = THREAD_STATUS_SUSPEND;
        softirq_thread->cause = CAUSE_COM;
        wind_enable_interrupt();
        _wind_thread_dispatch();
        if(wind_soft_vectors[softirq_index] != NULL)
        {
            (wind_soft_vectors[softirq_index])();
        }
    }
    //return ERR_OK;
}

//��������ж��߳�
w_err_t _wind_create_softirq_thread(void)
{
    wind_notice("create soft interrupt thread.");
    softirq_thread = wind_thread_create("softirq",PRIO_HIGH,wind_softirq_thread,
                0,NULL,softirq_stk,WIND_SOFTINT_STK_LEN);
    WIND_ASSERT_RETURN(softirq_thread != NULL,ERR_COMMAN);
    wind_thread_set_priority(softirq_thread,1);
    return ERR_OK;
}



#endif

