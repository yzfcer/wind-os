/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_cpu_port.h
**��   ��   ��: �ܽ���
**����޸�����: 2012.09.26
**��        ��: wind os��ʱ���������ͷ�ļ�
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
#ifndef WIND_STD_H_
#define WIND_STD_H_
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_DEBUG_SUPPORT
void      _wind_std_init(void);
w_int32_t wind_std_input(w_uint8_t *str,w_int32_t len);
w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
#else
#define _wind_std_init()
#endif

#ifdef __cplusplus
}
#endif

#endif