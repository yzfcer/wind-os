/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_conv.h
**创   建   人: Jason Zhou
**最后修改日期: 2017.12.10
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2017.12.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2017.12.10
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_CONV_H__
#define WIND_CONV_H__
#include "wind_type.h"
typedef enum
{
    ENDIAN_BIG = 0x01,
    ENDIAN_LITTLE = 0x02,
}w_endian_e;

//将大端格式转换成小端格式或者将小端格式转换成大端格式
#define BE2LE_2(value16) ((value16 >> 8) + (value16 << 8))
#define BE2LE_4(value32) (((value32 >> 24) & 0xff) \
            + ((value32 >> 16) & 0xff00)\
            + ((value32 << 16) & 0xff0000)\
            + ((value32 << 24) & 0xff000000))

w_endian_e wind_endian(void);
//数值与字符串之间的转换
w_bool_t wind_str_to_int(char *str,w_int32_t *value);
w_bool_t wind_str_to_uint(char *str,w_uint32_t *value);
w_bool_t wind_hexstr_to_int(char *hex,w_uint32_t *value);

void wind_int_to_str(char *str,w_int32_t value);
void wind_uint_to_str(char *str,w_uint32_t value);
void wind_int_to_hexstr(char *hex,w_uint32_t value);

//数值与字节数组之间的转换
w_bool_t wind_from_uint16(w_uint8_t *arr,w_uint16_t value);
w_bool_t wind_from_uint32(w_uint8_t *arr,w_uint32_t value);
w_bool_t wind_from_uint64(w_uint8_t *arr,w_uint64_t value);
w_bool_t wind_from_float32(w_uint8_t *arr,w_fp32_t value);
w_bool_t wind_from_float64(w_uint8_t *arr,w_fp64_t value);

w_bool_t wind_to_uint16(w_uint8_t *arr,w_uint16_t *value);
w_bool_t wind_to_uint32(w_uint8_t *arr,w_uint32_t *value);
w_bool_t wind_to_uint64(w_uint8_t *arr,w_uint64_t *value);
w_bool_t wind_to_float32(w_uint8_t *arr,w_fp32_t *value);
w_bool_t wind_to_float64(w_uint8_t *arr,w_fp64_t *value);


#endif

