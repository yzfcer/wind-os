/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_softirq.h / wind_softirq.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 用于实现系统的软中断的功能，软中断主要是提供一种在高优先级运行的任务模式，在该模块执行
**              执行期间将不会被其他的任务所中断，但却可以被硬件中断所打断的该优先级模式
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 2013.10.23未完待续
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_SOFTINT_H__
#define WIND_SOFTINT_H__
#include "wind_config.h"
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


#if WIND_SOFTIRQ_SUPPORT
typedef void (*w_softirq_fn)(void);

w_err_t _wind_softirq_mod_init(void);
w_err_t _wind_create_thread_softirq(void);

w_err_t wind_softirq_reg(w_uint16_t irqid,w_softirq_fn func);
w_err_t wind_softirq_unreg(w_int32_t irqid);
w_err_t wind_softirq_trig(w_int32_t irqid);
w_err_t wind_softirq_int(w_int32_t irqid);

#endif //if WIND_SOFTIRQ_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif // #ifndef WIND_SOFTINT_H__
