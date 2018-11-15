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


/***************************************************************
 wind-boot������Ҫ�������µ�ROM���ݴ洢�ռ�:
��bootloader��������
��bootloader��������
��bootloader������������
��ϵͳ����洢����
��ϵͳ���򱸷�����
��ϵͳ�����������
��ROM�ļ�ϵͳ��
��ϵͳ����ROM��������
��������bootloader��������ȫһ���ģ�����ϵͳ������ܲ�һ����
���е�һ�������г��򣬵ڶ��������ڵ�һ�ݳ���ʱ�޸�ʹ��,
�ļ�ϵͳ���Ǳ���ģ���������ļ�ϵͳ����size�����0
***************************************************************/

#define BOOT_SIZE 0xE000
#define BT_PARA1_SIZE 0x1000
#define BT_PARA2_SIZE 0x1000
#define IMG1_SIZE 0x10000
#define IMG2_SIZE 0x10000
#define IMG_PARA_SIZE 0x1000
#define FS_SIZE 0x20000

/***************************************************************
 wind-boot��Ҫ�������µ�RAM���ݴ洢�ռ�:
��boot��ϵͳ�����������
��ϵͳ��������/������������
��ϵͳ����RAM��������
***************************************************************/

#define SHARE_SIZE 0x1000
#define CACHE_SIZE 0x40000
#define SYSRUN_SIZE 0x10000


//ʹ��debug��Ϣ��ӡ����
#define BOOT_DEBUG_ENABLE 0

//���Թ���ʹ��
#define BOOT_TEST_ENABLE 1

#define WIND_DEBUG_SUPPORT 1
//���ܷ�ʽ
#define ENCRYPT_NONE 0
#define ENCRYPT_RC4 1
#define ENCRYPT_AES 2
#define ENCRYPT_DES 3
#define ENCRYPT_TYPE ENCRYPT_RC4


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

//iamge�ļ�������Կ
#define ENCRYPT_KEY {0x23,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,0x01,0x23,0x45,0x67}

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

#define CMD_BEEP_SUPPORT 0 //�Ƿ�֧�ַ���������
#define CMD_LED_SUPPORT 0 //�Ƿ�֧��LED��������

#ifdef __cplusplus
}
#endif
#endif

