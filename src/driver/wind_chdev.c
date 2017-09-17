/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_driver.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: 系统的驱动的相关的源文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "port/wind_config.h"
#include "port/wind_type.h"
#include "lib/wind_err.h"
#include "driver/wind_driver.h"
#include "core/wind_lock.h"
#include "port/wind_cpu_port.h"
pchdev_s g_chdev_list = NULL;
pchdev_s g_chdev_last = NULL;
void wind_insert_chdev(pchdev_s dev)
{
    wind_close_interrupt();
    if(g_chdev_list == NULL)
    {
        g_chdev_list = dev;
        g_chdev_last = dev;
        g_chdev_list->next = NULL;
        wind_open_interrupt();
        return;
    }
    g_chdev_last = dev;
    g_chdev_last->next = NULL;
    wind_open_interrupt();
}

err_t wind_remove_chdev(pchdev_s dev)
{
    return ERR_OK;
    
}

