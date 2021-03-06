/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: wind_atomic.h / wind_atomic.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2014/10/26 2:20:23
** 描        述: 基本原子操作函数方法
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2014/10/26 2:20:23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/


/*********************************************头文件定义***********************************************/
#include "wind_config.h"
#include "wind_type.h"
#include "wind_core.h"
#include "wind_atomic.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus



/********************************************内部变量定义**********************************************/



/********************************************内部函数定义*********************************************/



/********************************************全局变量定义**********************************************/



/********************************************全局函数定义**********************************************/
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

