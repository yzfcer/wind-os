/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_softirq.h
** Author      : Jason Zhou
** Last Date   : 
** Description : It is used to realize the function of soft interrupt of the system. Soft interrupt mainly 
**               provides a task mode that runs at high priority. During the execution of the module, it 
**               will not be interrupted by other tasks, but it can be interrupted by hardware interrupt
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
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
