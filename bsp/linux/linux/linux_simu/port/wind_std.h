/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_cpu_port.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind os stdard input/output function header
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
#ifndef WIND_STD_H_
#define WIND_STD_H_
#include "wind_type.h"


#ifdef __cplusplus
extern "C" {
#endif
#define wind_std_lock() wind_disable_interrupt()
#define wind_std_unlock() wind_enable_interrupt()

w_err_t _wind_std_init(void);
w_int32_t wind_std_input(w_uint8_t *str,w_int32_t len);
w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);


#ifdef __cplusplus
}
#endif

#endif
