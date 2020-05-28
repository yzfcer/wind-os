/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : user_entry.c
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : User program entry of wind-os
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

/*
This file is the entry of the user program. The user program is created by wind_main function starts to execute, 
but the thread of this function has a high priority. Therefore, it is not recommended that the user program 
execute directly here. It is better for the user to create a thread with medium priority here and transfer it 
to a new thread to execute the user program. At the same time, this program does not need a dead cycle and can exit,
but if a new thread is not created at the time of exit, the user program will not be executed again. 
*/

#include "wind_debug.h"
#include "wind_timer.h"
#include "wind_thread.h"
w_err_t test_tmr(w_timer_s *timer,void *arg)
{
    static w_uint32_t cnt = 0;
    wind_printf("timer test run:%d\n",cnt++);
    return W_ERR_OK;
}

w_err_t wind_main(void)
{
    wind_notice("enter wind main.");
#if 0
	wind_timer_create("testtmr",5000,test_tmr,W_NULL,F_TIMER_REPEAT|F_TIMER_RUN);
	while(1)
	{
		wind_thread_sleep(1000);
		wind_printf("wind_main run\r\n");
	}
#endif
    return W_ERR_OK;
}
