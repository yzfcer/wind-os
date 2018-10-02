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
#define SOFT_FLAG_ARR_CNT ((WIND_SOFTINT_MAX_NUM + 31) >> 5)
//���ж��̵߳Ķ�ջ
static w_stack_t softirq_stk[WIND_SOFTINT_STK_LEN];
static w_thread_s *softirq_thread = W_NULL;
w_softirq_fn softirq_vectors[WIND_SOFTINT_MAX_NUM];
w_uint32_t softirq_flag[SOFT_FLAG_ARR_CNT];

//��ʼ�����жϵ�һЩ��ز���
w_err_t _wind_softirq_mod_init(void)
{
    wind_memset(softirq_flag,0,sizeof(softirq_flag));
    wind_memset(softirq_vectors,0,sizeof(softirq_vectors));
    wind_memset(softirq_stk,0,WIND_SOFTINT_STK_LEN * sizeof(w_stack_t));
    return W_ERR_OK;
}


//�����ж�ģ��ע��һ���ж�������Ӧ����
w_err_t wind_softirq_reg(w_uint16_t irqid,w_softirq_fn func)
{
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    softirq_vectors[irqid] = func;
    return W_ERR_OK;
}

//ȡ��һ�����жϵ�ע��
w_err_t wind_softirq_unreg(w_int32_t irqid)
{
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    softirq_vectors[irqid] = W_NULL;
    return W_ERR_OK;
}

//����һ������ж�
w_err_t wind_softirq_trig(w_int32_t irqid)
{
    w_int32_t idx1,idx2;
    WIND_ASSERT_RETURN(irqid < WIND_SOFTINT_MAX_NUM,W_ERR_OVERFLOW);
    wind_disable_interrupt();
    idx1 = (irqid >> 5);
    idx2 = (irqid & 0x1f);
    softirq_flag[idx1] |= (1 << idx2);
    wind_thread_resume(softirq_thread);
    wind_enable_interrupt();
    _wind_switchto_thread(softirq_thread);
    return W_ERR_OK;
}

static w_softirq_fn get_irq_handle(void)
{
    w_int32_t i,j;
    w_int32_t idx1,idx2;
    
    for(i = 0;i < SOFT_FLAG_ARR_CNT;i ++)
    {
        if(softirq_flag[i] != 0)
        {
            idx1 = i;
            for(j = 0;j < 32;j ++)
            {
                if(softirq_flag[i] & (1 << j))
                {
                    idx2 = j;
                    softirq_flag[idx1] &= (~(0x01 << idx2));
                    return softirq_vectors[(idx1 << 5) + idx2];
                }
            }
        }
    }
    return W_NULL;
}

static w_err_t thread_softirq(w_int32_t argc,char **argv)
{
    w_softirq_fn func;
    softirq_thread->cause = CAUSE_COMMON;
    softirq_thread->runstat = THREAD_STATUS_SUSPEND;
    while(W_TRUE)
    {
        _wind_thread_dispatch();
        while(W_TRUE)
        {
            wind_disable_interrupt();
            func = get_irq_handle();
            if(func == W_NULL)
            {
                softirq_thread->cause = CAUSE_COMMON;
                softirq_thread->runstat = THREAD_STATUS_SUSPEND;
                wind_enable_interrupt();
                break;
            }
            wind_enable_interrupt();
            if(func != W_NULL)
                func();
        }
    }
}

//��������ж��߳�
w_err_t _wind_create_thread_softirq(void)
{
    wind_notice("create soft interrupt thread.");
    softirq_thread = wind_thread_create("softirq",thread_softirq,
                0,W_NULL,PRIO_HIGH,softirq_stk,WIND_SOFTINT_STK_LEN);
    WIND_ASSERT_RETURN(softirq_thread != W_NULL,W_ERR_FAIL);
    wind_thread_set_priority(softirq_thread,1);
    return W_ERR_OK;
}



#endif

