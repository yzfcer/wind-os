/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_config.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的总体配置文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26    
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CONFIG_H__
#define WIND_CONFIG_H__

//-------------------------------------------------------------------------------
//设备信息
//-------------------------------------------------------------------------------
#define ARCH_NAME "Cortex-M3"
#define CPU_NAME "STM42F103ZET6"
#define BOARD_NAME "ALIENTEKV3"
#define HW_VERSION 0x000003
#define SOFT_VERSION 0x000102

//-------------------------------------------------------------------------------
//宿主机操作系统类型
//-------------------------------------------------------------------------------
#define HOST_OS_NONE    1
#define HOST_OS_WINDOWS 2
#define HOST_OS_LINUX   3
#define HOST_OS_TYPE HOST_OS_NONE

//-------------------------------------------------------------------------------
//内核基本功能定制
//-------------------------------------------------------------------------------
#define WIND_THREAD_MAX_NUM 8 //需要的线程总数,建议根据需要来定义
#define WIND_THREAD_CALLBACK_SUPPORT 1  //是否支持线程事件毁回调函数

#define WIND_COROUTINE_SUPPORT 1 //是否支持协程
#define WIND_COROUTINE_MAX_NUM 8 //最大的协程的数量
#define WIND_COROUTINE_STKSIZE 128 //协程栈深度

#define WIND_STKPOOL_SUPPORT 0 //是否支持线程堆栈通过内存池分配
#define WIND_STK_MAX_NUM 4//堆栈的数量
#define WIND_STK_SIZE 512 //堆栈大小

#define WIND_MUTEX_SUPPORT 1 //是否支持互斥锁
#define WIND_MUTEX_MAX_NUM 10 //互斥锁的数量

#define WIND_SEM_SUPPORT 1 //是否支持信号量
#define WIND_SEM_MAX_NUM 8 //最大的信号量的数量

#define WIND_PIPE_SUPPORT 1  //是否支持管道机制
#define WIND_PIPE_MAX_NUM 2  //最大的管道数量

#define WIND_MSGBOX_SUPPORT 1  //是否支持消息机制
#define WIND_MBOX_MAX_NUM 2//邮箱的数量

#define WIND_EVENT_SUPPORT 1  //是否支持事件机制
#define WIND_EVENT_MAX_NUM 5//事件的数量

#define WIND_WATCHDOG_SUPPORT 1 //是否支持线程级看门狗
#define WIND_WATCHDOG_MAX_NUM 5//线程级看门狗的数量

#define WIND_TIMER_SUPPORT 1  //是否支持软件定时器
#define WIND_TIMER_MAX_NUM 2  //tick定时器的数量

#define WIND_SOFTIRQ_SUPPORT 1 //是否支持软中断功能
#define WIND_SOFTINT_MAX_NUM 256 //软中断向量表的长度

#define WIND_DAEMON_SUPPORT 1    //是否支持线程守护功能
#define WIND_DAEMON_MAX_NUM 5    //支持可守护线程数量

#define WIND_USER_SUPPORT 1 //是否支持用户账号功能
#define WIND_USER_MAX_NUM 5 //用户账号的数量

#define WIND_FSM_SUPPORT 1 //是否支持有限状态机功能
#define WIND_FSM_MAX_NUM 5 //有限状态机的数量

#define WIND_DBGPOINT_SUPPORT 1//是否支持调试点文件系统

#define WIND_TICK_PER_SEC 500 //一秒钟所经历的的时间节拍

#define WIND_REALTIME_CORE_SUPPORT 1 //是否支持实时内核，即抢占式切换

#define WIND_STATI_THREAD_SUPPORT 1    //是否支持统计线程

#define WIND_STACK_SUPPORT 0   //是否支持堆栈

#define WIND_HEAP_SUPPORT 1   //是否支持内存堆的分配

#define WIND_RTC_SUPPORT 0  //是否支持实时时间
#define WIND_DATETIME_SUPPORT 0  //是否支持系统时间

#define WIND_DEBUG_SUPPORT 1 //是否支持调试输出模块

#define WIND_DIAGNOSE_SUPPORT 1 //是否支持系统诊断功能

#define WIND_CHDEV_SUPPORT 0  //是否字符驱动框架

#define WIND_BLKDEV_SUPPORT 1  //是否块驱动框架
#define WIND_NULLDEV_MAX_NUM 5  //空块设备的数量

#define WIND_LUA_SUPPORT 0 //是否支持lua脚本语言

#define HARD_STACK_SIZE 40 //为硬件堆栈保留的空间大小，在某些硬件里可以不需要

#define WIND_XMODEM_SUPPORT 1

//-------------------------------------------------------------------------------
//线程栈空间大小定制
//-------------------------------------------------------------------------------
#define THREAD_DAEMON_STKSIZE 256
#define THREAD_IDLE_STKSIZE 64
#define THREAD_INIT_STKSIZE 256
#define THREAD_STATI_STKSIZE 256
#define THREAD_TIMER_STKSIZE 256
#define THREAD_SHELL_STKSIZE 2048
#define THREAD_COROUT_STKSIZE 256
#define THREAD_SOFTINT_STKSIZE 256 

//-------------------------------------------------------------------------------
//独立模块定制，前提是内核支持模块功能
//-------------------------------------------------------------------------------
#define WIND_MODULE_SUPPORT 1 // 是否支持独立模块功能
#if WIND_MODULE_SUPPORT
#define WIND_MODULE_SHELL_SUPPORT 1 //是否支持控制台模块
#define WIND_MODULE_CUTEST_SUPPORT 1 //是否支持单元测试框架模块
#define WIND_MODULE_VFS_SUPPORT 1 //是否支持虚拟文件系统框架模块
#define WIND_MODULE_DB_SUPPORT 0 //是否支持DB功能
#define WIND_MODULE_LUA_SUPPORT 1 //是否支持lua功能模块模块
#define WIND_MODULE_CJSON_SUPPORT 1 //是否支持cjson功能模块
#define WIND_MODULE_DICT_SUPPORT 1 //是否支持数据字典功能模块

#define WIND_MODULE_TOOLS_SUPPORT 0 //是否wind-os相关工具集模块
#define WIND_MODULE_GAME_SUPPORT 0 //是否支持游戏模块

#endif

//-------------------------------------------------------------------------------
//调试命令定制，前提是内核支持控制台功能
//-------------------------------------------------------------------------------
#if WIND_MODULE_SHELL_SUPPORT
#define CMD_BLKDEV_SUPPORT 0 //是否支持块设备操作命令
#define CMD_CHDEV_SUPPORT 0 //是否支持字符设备操作命令
#define CMD_DATETIME_SUPPORT 0 //是否支持日期时间命令
#define CMD_ECHO_SUPPORT 1 //是否支持回显命令
#define CMD_FS_SUPPORT 1 //是否支持文件系统命令
#define CMD_LIST_SUPPORT 1 //是否支持内核对象列表命令
#define CMD_MEM_SUPPORT 1 //是否支持内存值查看命令
#define CMD_RESET_SUPPORT 1 //是否支持系统复位命令
#define CMD_STATI_SUPPORT 1 //是否支持统计命令
#define CMD_THREAD_SUPPORT 1 //是否支持线程操作命令
#define CMD_XMODEM_SUPPORT 0 //是否支持xmodem协议命令
#define CMD_SYSINFO_SUPPORT 1 //是否支持系统信息命令
#define CMD_CUTEST_SUPPORT 1 //是否支持单元测试命令
#define CMD_DBGPOINT_SUPPORT 1 //是否支持系统调试点功能命令
#define CMD_USER_SUPPORT 1 //是否支持用户管理命令
#define CMD_WATCHDOG_SUPPORT 1 //是否支持软件看门狗命令
#define CMD_DAEMON_SUPPORT 1 //是否支持线程守护命令
#define CMD_DIAGNOSE_SUPPORT 1 //是否支持系统诊断命令
#define CMD_TIMER_SUPPORT 0 //是否支持定时器操作命令
#define CMD_DB_SUPPORT 0 //是否支持DB模块操作命令

#define CMD_BEEP_SUPPORT 0 //是否支持蜂鸣器命令
#define CMD_LED_SUPPORT 0 //是否支持LED操作命令
#endif

//-------------------------------------------------------------------------------
//单元测试功能定制，前提是内核支持单元测试框架,并且支持对应的功能模块
//-------------------------------------------------------------------------------
#if WIND_MODULE_CUTEST_SUPPORT
#define TEST_HEAP_SUPPORT 0 //是否支持内存堆测试
#define TEST_MSGBOX_SUPPORT 1 //是否支持消息邮箱测试
#define TEST_MUTEX_SUPPORT 1 //是否支持互斥所测试
#define TEST_PIPE_SUPPORT 1 //是否支持管道测试
#define TEST_POOL_SUPPORT 1 //是否支持内存池测试
#define TEST_QUEUE_SUPPORT 1 //是否支持队列测试
#define TEST_SEM_SUPPORT 1 //是否支持信号量测试
#define TEST_EVENT_SUPPORT 1 //是否支持事件测试
#define TEST_SOFTIRQ_SUPPORT 1 //是否支持软中断测试
#define TEST_TREE_SUPPORT 1 //是否支持树形结构对象测试
#define TEST_TREEFS_SUPPORT 1 //是否支持treefs文件系统测试
#define TEST_LISTFS_SUPPORT 1 //是否支持listfs文件系统测试
#define TEST_HOSTFS_SUPPORT 0 //是否支持hostfs主机文件系统测试
#define TEST_FS_SUPPORT 1 //是否支持fs文件系统接口测试
#define TEST_FILEPATH_SUPPORT 1 //是否支持文件系统路径测试
#define TEST_DIAGNOSE_SUPPORT 0 //是否支持诊断功能测试
#define TEST_DICT_SUPPORT 0     //是否支持参数字典功能测试
#endif


//-------------------------------------------------------------------------------
//文件系统功能定制，前提是系统支持虚拟文件系统vfs框架
//-------------------------------------------------------------------------------
#if WIND_MODULE_VFS_SUPPORT
#define WIND_FS_MAX_NUM 5 //允许支持的文件系统的数量
#define WIND_FILE_MAX_NUM 5 //允许同时打开的文件的数量

#define WIND_TREEFS_SUPPORT 1 //是否支持内存文件系统
#define WIND_TREEFS_MAX_NUM 2 //支持treefs结构的数量

#define WIND_LISTFS_SUPPORT 1//是否支持listfs文件系统

#define WIND_HOSTFS_SUPPORT 0//是否支持宿主机文件系统

#endif

#endif
