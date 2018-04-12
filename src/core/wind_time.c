/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_time.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os��ʱ��������
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
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_core.h"
#include "wind_time.h"
#include "wind_var.h"
static w_uint32_t g_wind_time_ms_cnt = 0;//�����ʱ

w_err_t wind_time_init(void)
{
    //wind_notice("sleep list initializing...");
    g_wind_time_ms_cnt = 0;
    return ERR_OK;
}

//��ȡ�����ʱ
w_uint32_t wind_get_time_count(void)
{
    return g_wind_time_ms_cnt;
}

//tick�жϵ��õĺ���
void wind_tick_callback(void)
{
    TICKS_CNT ++;//����tick������
    wind_thread_wakeup();
}

void wind_tick_isr(void)
{				   
    wind_enter_int();
    wind_tick_callback();
    wind_exit_int();       
}

