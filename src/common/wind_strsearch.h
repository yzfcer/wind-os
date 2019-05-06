/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_strsearch.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.05.07
**��        ��: �ַ��������㷨KMP��BM
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.05.07
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.05.07
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STRSEARCH_H__
#define WIND_STRSEARCH_H__
#include "wind_type.h"
#define KMP_SUBSTR_LEN 32
typedef struct  __w_kmp_context_s
{
	w_int32_t len;
    w_int32_t next[KMP_SUBSTR_LEN];
}w_kmp_context_s;

void kmp_init(w_kmp_context_s *ctx);
    
void kmp_get_next(w_kmp_context_s *ctx,char *substr);
 
w_int32_t kmp_search(w_kmp_context_s *ctx,char *str, char *substr, w_int32_t pos);

void kmp_print_next(w_kmp_context_s *ctx);

w_int32_t bm_search(char *str, char *substr, w_int32_t pos);

#endif
