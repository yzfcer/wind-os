/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_cpu_port.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: wind os的时间管理代码头文件
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
#ifndef WIND_STD_H_
#define WIND_STD_H_
#include "wind_type.h"
#include "wind_core.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_DEBUG_SUPPORT
#define wind_std_lock() wind_disable_interrupt()
#define wind_std_unlock() wind_enable_interrupt()

void      _wind_std_init(void);
w_int32_t wind_std_input(w_uint8_t *str,w_int32_t len);
w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
#else
#define wind_std_init()
#endif

#ifdef __cplusplus
}
#endif

#endif
