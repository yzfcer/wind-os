/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_core.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的核心启动和线程调度功能
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

#ifndef WIND_CORE_H__
#define WIND_CORE_H__
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _wind_corevar_init() 

#define _wind_thread_dispatch() 
#define _wind_switchto_thread(thread) 

#define wind_disable_switch() 
#define wind_enable_switch() 

#define wind_disable_interrupt() 
#define wind_enable_interrupt() 

#define wind_enter_irq() 
#define wind_exit_irq() 


#ifdef __cplusplus
}
#endif

#endif

