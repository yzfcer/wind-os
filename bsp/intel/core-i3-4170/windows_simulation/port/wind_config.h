/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef WIND_CONFIG_H__
#define WIND_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------
//�豸��Ϣ
//-------------------------------------------------------------------------------
#define ARCH_NAME "x86_x64"
#define CPU_NAME "core-i3-4170"
#define BOARD_NAME "windows_simulation"
#define HW_VERSION 0x000003
#define SOFT_VERSION 0x000102


//-------------------------------------------------------------------------------
//boot��ز�������
//-------------------------------------------------------------------------------
#define BOOT_SIZE 0xE000    //bootloader������
#define PARAM1_SIZE 0x1000  //bootloader������
#define PARAM2_SIZE 0x1000  //bootloader����������
#define IMG1_SIZE 0x10000   //ϵͳ����洢��
#define IMG2_SIZE 0x10000   //ϵͳ���򱸷���
#define IMG_PARA_SIZE 0x1000  //ϵͳ���������
#define FS_SIZE 0x20000     //�ļ�ϵͳ����

#define SHARE_SIZE 0x1000   //boot��ϵͳ���������
#define CACHE_SIZE 0x40000  //ϵͳ��������/����������
#define SYSRUN_SIZE 0x10000 //ϵͳ����������(ROM��RAM)

#define BOOT_DEBUG_ENABLE 0 //ʹ��debug��Ϣ��ӡ����
#define BOOT_TEST_ENABLE 1 //���Թ���ʹ��

//���ܷ�ʽ
#define ENCRYPT_NONE 0
#define ENCRYPT_RC4 1
#define ENCRYPT_AES 2
#define ENCRYPT_DES 3
#define ENCRYPT_TYPE ENCRYPT_RC4//ENCRYPT_NONE//

//iamge�ļ�������Կ
#define ENCRYPT_KEY {0x23,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,0x01,0x23,0x45,0x67}

//оƬ����ʹ��
#define MCU_LOCK_ENABLE 0

//�ȴ���������ʱ������
#define WAIT_KEY_SEC 2

//����λ��
#define RUN_IN_RAM 1
#define RUN_IN_ROM 2
#define RUN_SPACE_TYPE RUN_IN_RAM

//����洢��������
#define MEDIA_COUNT 4

//�洢���߼�����������
#define PART_COUNT 12

//�������ݿ����Ļ���buf��С
#define COMMBUF_SIZE 4096





//-------------------------------------------------------------------------------
//����ϵͳ�ں˻������ܶ���
//-------------------------------------------------------------------------------
#define WIND_THREAD_MAX_NUM 8 //��Ҫ���߳�����,���������Ҫ������
#define WIND_THREAD_CALLBACK_SUPPORT 1  //�Ƿ�֧���߳��¼��ٻص�����

#define WIND_STKPOOL_SUPPORT 0 //�Ƿ�֧���̶߳�ջͨ���ڴ�ط���
#define WIND_STK_MAX_NUM 4//��ջ������
#define WIND_STK_SIZE 512 //��ջ��С

#define WIND_MUTEX_SUPPORT 1 //�Ƿ�֧�ֻ�����
#define WIND_MUTEX_MAX_NUM 10 //������������

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

#define WIND_TIMER_SUPPORT 1  //�Ƿ�֧�������ʱ��
#define WIND_TIMER_MAX_NUM 2  //tick��ʱ��������

#define WIND_SOFTIRQ_SUPPORT 1 //�Ƿ�֧�����жϹ���
#define WIND_SOFTINT_MAX_NUM 256 //���ж�������ĳ���

#define WIND_USER_SUPPORT 0 //�Ƿ�֧���û��˺Ź���
#define WIND_USER_MAX_NUM 5 //�û��˺ŵ�����

#define WIND_FS_SUPPORT 1 //�Ƿ�֧���ڴ��ļ�ϵͳ
#define WIND_FS_MAX_NUM 2 //����֧�ֵ��ļ�ϵͳ������
#define WIND_FILE_MAX_NUM 5 //����򿪵��ļ�������

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

#define WIND_BLK_DRVFRAME_SUPPORT 1  //�Ƿ���������

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
#define CMD_LIST_SUPPORT 0 //�Ƿ�֧���ں˶����б�����
#define CMD_LUA_SUPPORT 0 //�Ƿ�֧��lua�ű�����
#define CMD_MEM_SUPPORT 0 //�Ƿ�֧���ڴ�ֵ�鿴����
#define CMD_RESET_SUPPORT 0 //�Ƿ�֧��ϵͳ��λ����
#define CMD_STATI_SUPPORT 0 //�Ƿ�֧��ͳ������
#define CMD_THREAD_SUPPORT 0 //�Ƿ�֧���̲߳�������
#define CMD_XMODEM_SUPPORT 0 //�Ƿ�֧��xmodemЭ������
#define CMD_SYSINFO_SUPPORT 0 //�Ƿ�֧��ϵͳ��Ϣ����
#define CMD_CUTEST_SUPPORT 0 //�Ƿ�֧�ֵ�Ԫ��������
#define CMD_MKFS_SUPPORT 1 //�Ƿ�֧�ִ����ļ�ϵͳ��������
#define CMD_DBGPOINT_SUPPORT 0 //�Ƿ�֧��ϵͳ���Ե㹦������
#define CMD_USER_SUPPORT 0 //�Ƿ�֧���û���������
#define CMD_WATCHDOG_SUPPORT 1 //�Ƿ�֧��������Ź�����

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
#define TEST_TREEFS_SUPPORT 1 //�Ƿ�֧��treefs�ļ�ϵͳ����
#define TEST_FS_SUPPORT 1 //�Ƿ�֧��fs�ļ�ϵͳ�ӿڲ���



#ifdef __cplusplus
}
#endif
#endif

