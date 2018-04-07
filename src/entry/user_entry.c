/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: entry.c
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os���û�̬�������
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

/*
����ļ����û��������ڣ��û�������wind_main�������뿪ʼִ�У�����������������̵߳�
���ȼ��ܸߣ���˲������û�����������ֱ��ִ�У�������û������ﴴ�������е����ȼ����߳�
��ת���µ��߳���ִ���û�����ͬʱ���������Ҫ��ѭ���������˳��������߳�������˳�ʱ
û�д��������û����򽫲����ٴα�ִ�е�����Ϊwind_main����ֻ�ᱻϵͳ����һ��
*/
#include "wind_core_inc.h"
#include "led.h"
#include "beep.h"

void led0_timer(void *arg)
{
    static w_bool_t f = B_TRUE;
    if(f)
        LED_On(0);
    else
        LED_Off(0);
    f = !f;
}

void led1_timer(void *arg)
{
    static w_bool_t f = B_TRUE;
    if(f)
        LED_On(1);
    else
        LED_Off(1);
    f = !f;
}

void led2_timer(void *arg)
{
    static w_bool_t f = B_TRUE;
    if(f)
        LED_On(2);
    else
        LED_Off(2);
    f = !f;
}

void led_start(void)
{
    LED_Init();
    wind_timer_create(300,led0_timer,NULL,B_TRUE);
    wind_timer_create(500,led1_timer,NULL,B_TRUE);
    wind_timer_create(800,led2_timer,NULL,B_TRUE);
}

w_err_t wind_main(void)
{
#if WIND_SEM_SUPPORT
    sem_s *psem = wind_sem_create("sem1",12);
    sem_s *psem1 = wind_sem_create("sem2",4);
    sem_s *psem2 = wind_sem_create("sem3",6);
#endif
    mutex_s *mutex1 = wind_mutex_create("mutex1");
    mutex_s *mutex2 = wind_mutex_create("mutex2");
    wind_notice("enter wind main.");
    led_start();
    BEEP_Init();
    return 0;
}
