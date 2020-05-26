/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_type.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : wind-os data type description
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
#ifndef WIND_TYPES_H__
#define WIND_TYPES_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef w_uint8_t
typedef unsigned char w_uint8_t;
#endif

#ifndef w_uint16_t
typedef unsigned short w_uint16_t;
#endif

#ifndef w_uint32_t
typedef unsigned long w_uint32_t; 
#endif

#ifndef w_uint64_t
typedef unsigned long long w_uint64_t; 
#endif

#ifndef w_int8_t
typedef signed char w_int8_t; 
#endif

#ifndef w_int16_t
typedef signed short w_int16_t; 
#endif

#ifndef w_int32_t
typedef signed long w_int32_t; 
#endif

#ifndef w_int64_t
typedef signed long long w_int64_t; 
#endif

#ifndef w_size_t
typedef unsigned long w_size_t; 
#endif

//CPU interrupt status register, which needs to be saved when the interrupt is closed
typedef w_uint32_t w_irqreg_t;

#ifndef w_fp32_t
typedef float w_fp32_t;
#endif

#ifndef w_fp64_t
typedef double w_fp64_t;
#endif

#ifndef w_lfp64_t
typedef long double w_lfp64_t;
#endif

#ifndef w_addr_t
typedef unsigned long w_addr_t; 
#endif

#ifndef w_str16_t
typedef char w_str16_t[16] ;
#endif

#ifndef w_str32_t
typedef char w_str32_t[32] ;
#endif


#ifndef w_str64_t
typedef char w_str64_t[64] ;
#endif

#ifndef w_str128_t
typedef char w_str128_t[128] ;
#endif

#ifndef w_str256_t
typedef char w_str256_t[256] ;
#endif

#ifndef __INLINE__
#define __INLINE__ 
#endif


#ifndef W_NULL
#define W_NULL ((void *)0)
#endif

#ifndef w_bool_t
typedef enum __w_bool_t
{
    W_FALSE = 0,
    W_TRUE = 1
} w_bool_t;
#endif


#ifndef w_err_t
typedef w_int16_t w_err_t;
#endif


#ifndef w_stack_t  //Definition of call stack element type
typedef w_uint32_t w_stack_t;
#endif



typedef enum __lock_type_e
{
    LOCK_TYPE_NONE = 0,
    LOCK_TYPE_AREA = 1,
    LOCK_TYPE_GLOBAL = 2,
    LOCK_TYPE_BUTT
}lock_type_e;






#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  

