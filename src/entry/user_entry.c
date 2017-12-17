/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: entry.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的用户态程序入口
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

/*
这个文件是用户程序的入口，用户程序由wind_main函数进入开始执行，但这个函数的所在线程的
优先级很高，因此不建议用户程序在这里直接执行，最好是用户在这里创建具有中等优先级的线程
并转到新的线程中执行用户程序。同时这个程序不需要死循环，可以退出，但新线程如果在退出时
没有创建，则用户程序将不会再次被执行到。因为wind_main函数只会被系统调用一次
*/
#include "wind_core_inc.h"

w_err_t wind_main(void)
{
#if WIND_SEM_SUPPORT
    sem_s *psem = wind_sem_create("sem1",12);
    sem_s *psem1 = wind_sem_create("sem2",4);
    sem_s *psem2 = wind_sem_create("sem3",6);
#endif
    lock_s *lock1 = wind_lock_create("lock1");
    lock_s *lock2 = wind_lock_create("lock2");
    wind_notice("enter wind main.");
    return 0;
}
