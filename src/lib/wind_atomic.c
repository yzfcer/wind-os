/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : wind_atomic.c
** Author      : Jason Zhou
** Last Date   : 2014/10/26 2:20:23
** Description : Atomic variable operation function
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2014/10/26 2:20:23
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/



#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_atomic.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
















void atomic_inc(w_atomic_t atom)
{
    wind_disable_interrupt();
    atom.value ++;
    wind_enable_interrupt();
}

void atomic_dec(w_atomic_t atom)
{
    wind_disable_interrupt();
    atom.value --;
    wind_enable_interrupt();
}

void atomic_add(w_atomic_t atom,w_uint32_t va)
{
    wind_disable_interrupt();
    atom.value += va;
    wind_enable_interrupt();
}

void atomic_minus(w_atomic_t atom,w_uint32_t value)
{
    wind_disable_interrupt();
    atom.value -= value;
    wind_enable_interrupt();
}

void atomic_set(w_atomic_t atom,w_uint32_t value)
{
    wind_disable_interrupt();
    atom.value -= value;
    wind_enable_interrupt();

}

void atomic_get(w_atomic_t atom,w_uint32_t *value)
{
    wind_disable_interrupt();
    *value = atom.value;
    wind_enable_interrupt();   
}

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

