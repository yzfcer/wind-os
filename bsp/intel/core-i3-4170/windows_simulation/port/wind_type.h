/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_type.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os�����Ͷ���
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �ܽ���
** �ա���: 2012.10.20
** �衡��: 
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
typedef char w_int8_t; 
#endif

#ifndef w_int16_t
typedef short w_int16_t; 
#endif

#ifndef w_int32_t
typedef long w_int32_t; 
#endif

#ifndef w_int64_t
typedef long long w_int64_t; 
#endif

//SREG��CPU״̬�Ĵ�����Ӧ������λ�����ر��ж�ʱ��Ҫ��������Ĵ���
typedef w_uint32_t w_sreg_t;

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
typedef w_int32_t w_err_t;
#endif


#ifndef w_stack_t  //��ջ��ȵĶ���
typedef w_uint32_t w_stack_t;//,*w_pstack_t;
typedef w_uint32_t *w_pstack_t;
#endif


#ifndef w_handle_t 
typedef w_int16_t w_handle_t;
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

