/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_stat.h
**��   ��   ��: �ܽ���
**����޸�����: wind os����Դͳ����صĴ���
**��        ��: 
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2013.08.13
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STAT_H__
#define WIND_STAT_H__
#include "wind_type.h"
#include "wind_config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct __stat_s
{
    w_uint32_t tot;
    w_uint32_t used;
    w_uint32_t max;
    w_uint32_t err;
}w_stati_s;

#define WIND_STATI_INIT(stati,totel) \
            do{stati.tot = totel;\
            stati.used = 0;\
            stati.max = 0;\
            stati.err = 0;}while(0)
#define WIND_STATI_INC(stati) do{stati.used++;WIND_STATI_MAX(stati);}while(0)
#define WIND_STATI_DEC(stati) do{if(stati.used > 0) stati.used--;}while(0)
#define WIND_STATI_ERR_INC(stati) stati.err++
#define WIND_STATI_MAX(stati) \
        do{if(stati.used > stati.max) \
        stati.max = stati.used;}while(0)
#define WIND_STATI_ADD(stati,num) do{stati.used+=num;WIND_STATI_MAX(stati);}while(0)
#define WIND_STATI_MINUS(stati,num) do{if(stati.used >= (num)) stati.used-=(num);}while(0)

#ifdef __cplusplus
}
#endif


#endif

