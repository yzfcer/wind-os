/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_conv.h
**��   ��   ��: Jason Zhou
**����޸�����: 2017.12.10
**��        ��: 
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 2017.12.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2017.12.10
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_CONV_H__
#define WIND_CONV_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

typedef enum
{
    ENDIAN_BIG = 0x01,
    ENDIAN_LITTLE = 0x02,
}w_endian_e;

//Byte order conversion
#define BE2LE_2(value16) (((value16) >> 8) + ((value16) << 8))
#define BE2LE_4(value32) (((value32 >> 24) & 0xff)  + (((value32) >> 16) & 0xff00) + (((value32) << 16) & 0xff0000) + (((value32) << 24) & 0xff000000))

//Assign to itself after byte order conversion
#define SELF_BE2LE_2(value16) value16 = BE2LE_2(value16)
#define SELF_BE2LE_4(value32) value32 = BE2LE_4(value32)

//Check whether the current environment is big end or little end
w_endian_e wind_endian(void);

//convert string to integer type
w_bool_t wind_str_to_int(char *str,w_int32_t *value);
w_bool_t wind_str_to_uint(char *str,w_uint32_t *value);
w_bool_t wind_hexstr_to_int(char *hex,w_uint32_t *value);

//convert integer to string type
void wind_int_to_str(char *str,w_int32_t value);
void wind_uint_to_str(char *str,w_uint32_t value);
void wind_int_to_hexstr(char *hex,w_uint32_t value);

//Convert number to string
w_bool_t wind_from_uint16(w_uint8_t *arr,w_uint16_t value);
w_bool_t wind_from_uint32(w_uint8_t *arr,w_uint32_t value);
w_bool_t wind_from_uint64(w_uint8_t *arr,w_uint64_t value);
w_bool_t wind_from_float32(w_uint8_t *arr,w_fp32_t value);
w_bool_t wind_from_float64(w_uint8_t *arr,w_fp64_t value);

//Convert string to number
w_bool_t wind_to_uint16(w_uint8_t *arr,w_uint16_t *value);
w_bool_t wind_to_uint32(w_uint8_t *arr,w_uint32_t *value);
w_bool_t wind_to_uint64(w_uint8_t *arr,w_uint64_t *value);
w_bool_t wind_to_float32(w_uint8_t *arr,w_fp32_t *value);
w_bool_t wind_to_float64(w_uint8_t *arr,w_fp64_t *value);

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_CONV_H__

