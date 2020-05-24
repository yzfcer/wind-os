/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: entry.c
**Author: Jason Zhou
**Last Date: 2012.09.26
**Description: wind os的用户态程序入口
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2012.09.26
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2012.10.20
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

/*
这个文件是用户程序的入口，用户程序由wind_main函数进入开始执行，但这个函数的所在线程的
优先级很高，因此不建议用户程序在这里直接执行，最好是用户在这里创建具有中等优先级的线程
并转到新的线程中执行用户程序。同时这个程序不需要死循环，可以退出，但新线程如果在退出时
没有创建，则用户程序将不会再次被执行到。因为wind_main函数只会被系统调用一次
*/

#include "wind_debug.h"
#include "wind_timer.h"

w_err_t test_tmr(w_timer_s *timer,void *arg)
{
    static w_uint32_t cnt = 0;
    wind_printf("timer test run:%d\n",cnt++);
    return W_ERR_OK;
}

w_err_t wind_main(void)
{
    wind_notice("enter wind main.");
    //wind_timer_create("testtmr",5000,test_tmr,W_NULL,F_TIMER_REPEAT|F_TIMER_RUN);
    return 0;
}
