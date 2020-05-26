/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_strsearch.h
** Author      : Jason Zhou
** Last Date   : 2019.05.07
** Description : KMP and BM string search algorithm
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.05.07
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.05.07
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_STRSEARCH_H__
#define WIND_STRSEARCH_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#define KMP_SUBSTR_LEN 32
typedef struct  __w_kmp_context_s
{
    w_int32_t len;
    w_int32_t next[KMP_SUBSTR_LEN];
}w_kmp_context_s;

w_err_t kmp_init(w_kmp_context_s *ctx,char *substr);

w_int32_t kmp_search(w_kmp_context_s *ctx,char *str,char *substr,w_int32_t pos);

w_err_t kmp_print_next(w_kmp_context_s *ctx);

w_int32_t bm_search(char *str,char *substr,w_int32_t pos);

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_STRSEARCH_H__
