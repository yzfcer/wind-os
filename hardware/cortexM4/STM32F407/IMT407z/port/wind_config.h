/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_config.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的总体配置文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26    
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_CONFIG_H__
#define WIND_CONFIG_H__


//-------------------------------------------------------------------------------

#define WIND_STAT_SUPPORT 1 //是否支持资源统计功能


#define WIND_SEM_SUPPORT 1 //是否支持信号量

#define WIND_PIPE_SUPPORT 1  //是否支持管道机制

#define WIND_MESSAGE_SUPPORT 1  //是否支持消息机制

#define WIND_REALTIME_CORE_SUPPORT 1 //是否支持实时内核，即抢占式切换

#define WIND_STKPOOL_SUPPORT 1 //是否支持线程堆栈通过内存池分配

#define WIND_DAEMON_SUPPORT 1    //是否支持守护线程

#define WIND_QUEUE_SUPPORT 1  //是否支持队列

#define WIND_STACK_SUPPORT 1   //是否支持堆栈

#define WIND_MEM_SUPPORT 1   //是否支持内存管理

#define WIND_TIMER_SUPPORT 1  //是否支持软件定时器

#define WIND_RTC_SUPPORT 0  //是否支持实时时间

#define WIND_CONSOLE_SUPPORT 1 //是否支持控制台模块

#define WIND_SOFTINT_SUPPORT 1 //是否支持软中断功能

#define WIND_HEAP_SUPPORT 1 //是否支持内存堆的分配

#define WIND_PRIVATE_HEAP_SUPPORT 1//是否支持线程的私有堆的分配

#define WIND_THREAD_CALLBACK_SUPPORT 1//是否支持线程事件毁回调函数

#define WIND_FS_SUPPORT 0 //是否支持内存文件系统
//-----------------------------------------------------------------------

#define WIND_THREAD_MAX_NUM 40 //需要的线程总数,建议根据需要来定义

#define WIND_TICK_PER_SEC 1000 //一秒钟所经历的的时间街节拍

#define HARD_STACK_SIZE 40 //为硬件堆栈保留的空间大小，在某些硬件里可以不需要


#define WIND_PIPE_MAX_NUM 50  //最大的管道数量

#define WIND_MESSAGE_MAX_NUM 50 //最大的消息数量
#define WIND_MBOX_MAX_NUM 20//邮箱的数量

#define WIND_SEM_MAX_NUM 50 //最大的信号量的数量

#define WIND_NODE_MAX_NUM 200  //链表节点内存池的容量

#define WIND_TIMER_MAX_NUM 30  //tick定时器内存池的容量

#define WIND_LOCK_NUM 10 //互斥锁的数量

#define WIND_SOFTINT_MAX_NUM 32 //软中断向量表的长度

#define WIND_STK_MAX_NUM 10//堆栈的数量
#define WIND_STK_SIZE 512


#endif
