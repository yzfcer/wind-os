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
#ifndef BOOT_CONFIG_H__
#define BOOT_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************
 wind-boot依次需要定义如下的ROM数据存储空间:
【bootloader程序区】
【bootloader参数区】
【bootloader参数备份区】
【系统程序存储区】
【系统程序备份区】
【系统程序参数区】
【ROM文件系统】
【系统程序ROM运行区】
其中两份bootloader参数是完全一样的，两份系统程序可能不一样，
其中第一份是运行程序，第二份用于在第一份出错时修复使用,
文件系统不是必须的，如果不用文件系统，将size定义成0
***************************************************************/

#define BOOT_SIZE 0xE000
#define BT_PARA1_SIZE 0x1000
#define BT_PARA2_SIZE 0x1000
#define IMG1_SIZE 0x10000
#define IMG2_SIZE 0x10000
#define IMG_PARA_SIZE 0x1000
#define FS_SIZE 0x20000

/***************************************************************
 wind-boot需要定义如下的RAM数据存储空间:
【boot和系统共享参数区】
【系统程序下载/升级缓存区】
【系统程序RAM运行区】
***************************************************************/

#define SHARE_SIZE 0x1000
#define CACHE_SIZE 0x40000
#define SYSRUN_SIZE 0x10000


//使能debug信息打印功能
#define BOOT_DEBUG_ENABLE 0

//测试功能使能
#define BOOT_TEST_ENABLE 1

#define WIND_DEBUG_SUPPORT 1
//加密方式
#define ENCRYPT_NONE 0
#define ENCRYPT_AES 1
#define ENCRYPT_DES 2
#define ENCRYPT_RC4 3
#define ENCRYPT_TYPE ENCRYPT_RC4


//芯片锁定使能
#define MCU_LOCK_ENABLE 0

//等待键盘输入时间秒数
#define WAIT_KEY_SEC 2

//运行位置
#define RUN_IN_RAM 1
#define RUN_IN_ROM 2
#define RUN_SPACE_TYPE RUN_IN_RAM

//物理存储器的数量
#define MEDIA_COUNT 4

//存储器逻辑分区的数量
#define PART_COUNT 12


//用于数据拷贝的缓存buf大小
#define COMMBUF_SIZE 4096

//iamge文件加密密钥
#define ENCRYPT_KEY {0x23,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,0x01,0x23,0x45,0x67}


#ifdef __cplusplus
}
#endif
#endif

