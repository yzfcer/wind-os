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

//��ֵ���ַ���֮���ת��
w_bool_t wind_atoi(char *str,w_int32_t *value);
w_bool_t wind_atoui(char *str,w_uint32_t *value);
w_bool_t wind_htoi(char *hex,w_uint32_t *value);

void wind_itoa(char *str,w_int32_t value);
void wind_uitoa(char *str,w_uint32_t value);
void wind_itoh(char *hex,w_uint32_t value);

w_fp64_t wind_strtod(const char *str,char **endptr);

//��ֵ���ֽ�����֮���ת��
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

