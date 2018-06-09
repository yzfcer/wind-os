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

#define WIND_MUTEX_SUPPORT 1 //�Ƿ�֧�ֻ�����

#define WIND_SEM_SUPPORT 1 //�Ƿ�֧���ź���

#define WIND_PIPE_SUPPORT 1  //�Ƿ�֧�ֹܵ�����

#define WIND_MSGBOX_SUPPORT 1  //�Ƿ�֧����Ϣ����

#define WIND_EVENT_SUPPORT 0  //�Ƿ�֧���¼�����

#define WIND_REALTIME_CORE_SUPPORT 1 //�Ƿ�֧��ʵʱ�ںˣ�����ռʽ�л�

#define WIND_STKPOOL_SUPPORT 1 //�Ƿ�֧���̶߳�ջͨ���ڴ�ط���

#define WIND_DAEMON_THREAD_SUPPORT 1    //�Ƿ�֧���ػ��߳�

#define WIND_STATI_THREAD_SUPPORT 1    //�Ƿ�֧��ͳ���߳�

#define WIND_STACK_SUPPORT 1   //�Ƿ�֧�ֶ�ջ

#define WIND_TIMER_SUPPORT 1  //�Ƿ�֧�������ʱ��

#define WIND_RTC_SUPPORT 0  //�Ƿ�֧��ʵʱʱ��

#define WIND_DATETIME_SUPPORT 1  //�Ƿ�֧��ϵͳʱ��

#define WIND_CONSOLE_SUPPORT 1 //�Ƿ�֧�ֿ���̨ģ��

#define WIND_DEBUG_SUPPORT 1 //�Ƿ�֧�ֵ������ģ��

#define WIND_SOFTIRQ_SUPPORT 1 //�Ƿ�֧�����жϹ���

#define WIND_HEAP_SUPPORT 1   //�Ƿ�֧���ڴ�ѵķ���

#define WIND_THREAD_CALLBACK_SUPPORT 1  //�Ƿ�֧���߳��¼��ٻص�����

#define WIND_DRVFRAME_SUPPORT 1  //�Ƿ��ַ��������

#define WIND_BLK_DRVFRAME_SUPPORT 1  //�Ƿ���������

#define WIND_FS_SUPPORT 1 //�Ƿ�֧���ڴ��ļ�ϵͳ

#define WIND_LUA_SUPPORT 1 //�Ƿ�֧��lua�ű�����

#define WIND_XMODEM_SUPPORT 1 //�Ƿ�֧��XMODEMЭ��

//-----------------------------------------------------------------------

#define WIND_THREAD_MAX_NUM 8 //��Ҫ���߳�����,���������Ҫ������

#define WIND_TICK_PER_SEC 500 //һ�����������ĵ�ʱ��ֽ���

#define HARD_STACK_SIZE 40 //ΪӲ����ջ�����Ŀռ��С����ĳЩӲ������Բ���Ҫ

#define WIND_PIPE_MAX_NUM 10  //���Ĺܵ�����

#define WIND_MBOX_MAX_NUM 5//���������

#define WIND_EVENT_MAX_NUM 5//�¼�������

#define WIND_SEM_MAX_NUM 10 //�����ź���������

#define WIND_TIMER_MAX_NUM 10  //tick��ʱ���ڴ�ص�����

#define WIND_MUTEX_MAX_NUM 20 //������������

#define WIND_SOFTINT_MAX_NUM 256 //���ж�������ĳ���

#define WIND_STK_MAX_NUM 4//��ջ������

#define WIND_STK_SIZE 512 //��ջ��С

#define WIND_FS_MAX_NUM 5 //����֧�ֵ��ļ�ϵͳ������

#define WIND_FILE_MAX_NUM 5 //����򿪵��ļ�������


#endif
