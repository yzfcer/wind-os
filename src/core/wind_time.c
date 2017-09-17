/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_time.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码
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
#include "wind_type.h"
#include "wind_os_hwif.h"
#include "wind_debug.h"
#include "wind_thread.h"
#include "wind_core.h"
#include "wind_time.h"
#include "wind_var.h"
static w_uint32_t g_wind_time_ms_cnt = 0;//毫秒计时

w_err_t wind_time_init(void)
{
    WIND_INFO("sleep list initializing...\r\n");
    g_wind_time_ms_cnt = 0;
    return ERR_OK;
}

//获取毫秒计时
w_uint32_t wind_get_time_count(void)
{
    return g_wind_time_ms_cnt;
}

//tick中断调用的函数
void wind_tick_callback(void)
{
    TICKS_CNT ++;//更新tick计数器
    wind_thread_wakeup();
}

void wind_tick_isr(void)
{				   
    wind_enter_int();
    wind_tick_callback();
    wind_exit_int();       
}

