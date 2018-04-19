/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_config.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os�����������ļ�
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

#ifndef WIND_CONFIG_H__
#define WIND_CONFIG_H__


//-------------------------------------------------------------------------------

#define WIND_MUTEX_SUPPORT 0 //�Ƿ�֧�ֻ�����

#define WIND_SEM_SUPPORT 0 //�Ƿ�֧���ź���

#define WIND_PIPE_SUPPORT 0  //�Ƿ�֧�ֹܵ�����

#define WIND_MSGBOX_SUPPORT 0  //�Ƿ�֧����Ϣ����

#define WIND_REALTIME_CORE_SUPPORT 1 //�Ƿ�֧��ʵʱ�ںˣ�����ռʽ�л�

#define WIND_STKPOOL_SUPPORT 0 //�Ƿ�֧���̶߳�ջͨ���ڴ�ط���

#define WIND_DAEMON_THREAD_SUPPORT 0    //�Ƿ�֧���ػ��߳�

#define WIND_STATI_THREAD_SUPPORT 0    //�Ƿ�֧��ͳ���߳�

#define WIND_STACK_SUPPORT 1   //�Ƿ�֧�ֶ�ջ

#define WIND_TIMER_SUPPORT 0  //�Ƿ�֧�������ʱ��

#define WIND_RTC_SUPPORT 0  //�Ƿ�֧��ʵʱʱ��

#define WIND_CONSOLE_SUPPORT 0 //�Ƿ�֧�ֿ���̨ģ��

#define WIND_DEBUG_SUPPORT 0 //�Ƿ�֧�ֵ������ģ��

#define WIND_SOFTINT_SUPPORT 0 //�Ƿ�֧�����жϹ���

#define WIND_HEAP_SUPPORT 0   //�Ƿ�֧���ڴ�ѵķ���

#define WIND_PRIVATE_HEAP_SUPPORT 0  //�Ƿ�֧���̵߳�˽�жѵķ���

#define WIND_THREAD_CALLBACK_SUPPORT 0  //�Ƿ�֧���߳��¼��ٻص�����

#define WIND_DRVFRAME_SUPPORT 0  //�Ƿ��ַ��������

#define WIND_BLK_DRVFRAME_SUPPORT 0  //�Ƿ���������

#define WIND_FS_SUPPORT 1 //�Ƿ�֧���ڴ��ļ�ϵͳ

//-----------------------------------------------------------------------

#define WIND_THREAD_MAX_NUM 4 //��Ҫ���߳�����,���������Ҫ������

#define WIND_TICK_PER_SEC 1000 //һ�����������ĵ�ʱ��ֽ���

#define HARD_STACK_SIZE 40 //ΪӲ����ջ�����Ŀռ��С����ĳЩӲ������Բ���Ҫ


#define WIND_PIPE_MAX_NUM 10  //���Ĺܵ�����

#define WIND_MBOX_MAX_NUM 5//���������

#define WIND_SEM_MAX_NUM 10 //�����ź���������

#define WIND_TIMER_MAX_NUM 10  //tick��ʱ���ڴ�ص�����

#define WIND_MUTEX_MAX_NUM 20 //������������

#define WIND_SOFTINT_MAX_NUM 32 //���ж�������ĳ���

#define WIND_STK_MAX_NUM 4//��ջ������

#define WIND_STK_SIZE 512


#endif
