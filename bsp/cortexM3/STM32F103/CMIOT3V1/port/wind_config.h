/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_config.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: wind os�����������ļ�
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

#ifndef WIND_CONFIG_H__
#define WIND_CONFIG_H__


//-------------------------------------------------------------------------------
//�豸��Ϣ
//-------------------------------------------------------------------------------
#define ARCH_NAME "Cortex-M3"
#define CPU_NAME "STM42F103ZET6"
#define BOARD_NAME "CMIOT3V1"
#define HW_VERSION 0x000003
#define SOFT_VERSION 0x000102


//-------------------------------------------------------------------------------
//�ں˻������ܶ���
//-------------------------------------------------------------------------------
#define WIND_THREAD_MAX_NUM 8 //��Ҫ���߳�����,���������Ҫ������
#define WIND_THREAD_CALLBACK_SUPPORT 1  //�Ƿ�֧���߳��¼��ٻص�����

#define WIND_STKPOOL_SUPPORT 0 //�Ƿ�֧���̶߳�ջͨ���ڴ�ط���
#define WIND_STK_MAX_NUM 4//��ջ������
#define WIND_STK_SIZE 512 //��ջ��С

#define WIND_MUTEX_SUPPORT 1 //�Ƿ�֧�ֻ�����
#define WIND_MUTEX_MAX_NUM 8 //������������

#define WIND_SEM_SUPPORT 1 //�Ƿ�֧���ź���
#define WIND_SEM_MAX_NUM 8 //�����ź���������

#define WIND_PIPE_SUPPORT 1  //�Ƿ�֧�ֹܵ�����
#define WIND_PIPE_MAX_NUM 2  //���Ĺܵ�����

#define WIND_MSGBOX_SUPPORT 1  //�Ƿ�֧����Ϣ����
#define WIND_MBOX_MAX_NUM 2//���������

#define WIND_EVENT_SUPPORT 1  //�Ƿ�֧���¼�����
#define WIND_EVENT_MAX_NUM 5//�¼�������

#define WIND_WATCHDOG_SUPPORT 1 //�Ƿ�֧���̼߳����Ź�
#define WIND_WATCHDOG_MAX_NUM 5//�̼߳����Ź�������

#define WIND_TIMER_SUPPORT 1  //�Ƿ�֧��������ʱ��
#define WIND_TIMER_MAX_NUM 2  //tick��ʱ��������

#define WIND_SOFTIRQ_SUPPORT 1 //�Ƿ�֧�����жϹ���
#define WIND_SOFTINT_MAX_NUM 256 //���ж��������ĳ���

#define WIND_USER_SUPPORT 1 //�Ƿ�֧���û��˺Ź���
#define WIND_USER_MAX_NUM 5 //�û��˺ŵ�����

#define WIND_FS_SUPPORT 0 //�Ƿ�֧���ڴ��ļ�ϵͳ
#define WIND_FS_MAX_NUM 5 //����֧�ֵ��ļ�ϵͳ������
#define WIND_FILE_MAX_NUM 5 //�����򿪵��ļ�������

#define WIND_DBGPOINT_SUPPORT 1//�Ƿ�֧�ֵ��Ե��ļ�ϵͳ

#define WIND_TICK_PER_SEC 500 //һ�����������ĵ�ʱ��ֽ���

#define WIND_REALTIME_CORE_SUPPORT 1 //�Ƿ�֧��ʵʱ�ںˣ�����ռʽ�л�

#define WIND_DAEMON_THREAD_SUPPORT 1    //�Ƿ�֧���ػ��߳�

#define WIND_STATI_THREAD_SUPPORT 1    //�Ƿ�֧��ͳ���߳�

#define WIND_STACK_SUPPORT 0   //�Ƿ�֧�ֶ�ջ

#define WIND_HEAP_SUPPORT 1   //�Ƿ�֧���ڴ�ѵķ���

#define WIND_RTC_SUPPORT 0  //�Ƿ�֧��ʵʱʱ��
#define WIND_DATETIME_SUPPORT 0  //�Ƿ�֧��ϵͳʱ��

#define WIND_DEBUG_SUPPORT 1 //�Ƿ�֧�ֵ������ģ��

#define WIND_DRVFRAME_SUPPORT 0  //�Ƿ��ַ��������

#define WIND_BLK_DRVFRAME_SUPPORT 0  //�Ƿ���������

#define WIND_LUA_SUPPORT 0 //�Ƿ�֧��lua�ű�����

#define HARD_STACK_SIZE 40 //ΪӲ����ջ�����Ŀռ��С����ĳЩӲ������Բ���Ҫ

//-------------------------------------------------------------------------------
//��������ƣ�ǰ�����ں�֧�ֿ���̨����
//-------------------------------------------------------------------------------
#define WIND_CONSOLE_SUPPORT 1 //�Ƿ�֧�ֿ���̨ģ��

#define CMD_BLKDEV_SUPPORT 0 //�Ƿ�֧�ֿ��豸��������
#define CMD_DATETIME_SUPPORT 0 //�Ƿ�֧������ʱ������
#define CMD_ECHO_SUPPORT 1 //�Ƿ�֧�ֻ�������
#define CMD_FS_SUPPORT 0 //�Ƿ�֧���ļ�ϵͳ����
#define CMD_LIST_SUPPORT 1 //�Ƿ�֧���ں˶����б�����
#define CMD_LUA_SUPPORT 0 //�Ƿ�֧��lua�ű�����
#define CMD_MEM_SUPPORT 1 //�Ƿ�֧���ڴ�ֵ�鿴����
#define CMD_RESET_SUPPORT 1 //�Ƿ�֧��ϵͳ��λ����
#define CMD_STATI_SUPPORT 1 //�Ƿ�֧��ͳ������
#define CMD_THREAD_SUPPORT 1 //�Ƿ�֧���̲߳�������
#define CMD_XMODEM_SUPPORT 0 //�Ƿ�֧��xmodemЭ������
#define CMD_SYSINFO_SUPPORT 1 //�Ƿ�֧��ϵͳ��Ϣ����
#define CMD_CUTEST_SUPPORT 1 //�Ƿ�֧�ֵ�Ԫ��������
#define CMD_MKFS_SUPPORT 0 //�Ƿ�֧�ִ����ļ�ϵͳ��������
#define CMD_DBGPOINT_SUPPORT 1 //�Ƿ�֧��ϵͳ���Ե㹦������
#define CMD_USER_SUPPORT 1 //�Ƿ�֧���û���������

#define CMD_BEEP_SUPPORT 0 //�Ƿ�֧�ַ���������
#define CMD_LED_SUPPORT 0 //�Ƿ�֧��LED��������

//-------------------------------------------------------------------------------
//��Ԫ���Թ��ܶ��ƣ�ǰ�����ں�֧�ֵ�Ԫ���Կ��,����֧�ֶ�Ӧ�Ĺ���ģ��
//-------------------------------------------------------------------------------
#define WIND_CUTEST_SUPPORT 1 //�Ƿ�֧�ֵ�Ԫ���Կ��

#define TEST_HEAP_SUPPORT 0 //�Ƿ�֧���ڴ�Ѳ���
#define TEST_MSGBOX_SUPPORT 1 //�Ƿ�֧����Ϣ�������
#define TEST_MUTEX_SUPPORT 1 //�Ƿ�֧�ֻ���������
#define TEST_PIPE_SUPPORT 1 //�Ƿ�֧�ֹܵ�����
#define TEST_POOL_SUPPORT 1 //�Ƿ�֧���ڴ�ز���
#define TEST_QUEUE_SUPPORT 1 //�Ƿ�֧�ֶ��в���
#define TEST_SEM_SUPPORT 1 //�Ƿ�֧���ź�������
#define TEST_EVENT_SUPPORT 1 //�Ƿ�֧���¼�����
#define TEST_SOFTIRQ_SUPPORT 1 //�Ƿ�֧�����жϲ���
#define TEST_TREE_SUPPORT 1 //�Ƿ�֧�����νṹ�������
#define TEST_TREEFS_SUPPORT 0 //�Ƿ�֧��treefs�ļ�ϵͳ����
#define TEST_FS_SUPPORT 0 //�Ƿ�֧��fs�ļ�ϵͳ�ӿڲ���

#endif