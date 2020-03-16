/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_strsearch.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.05.07
**描        述: 字符串搜索算法KMP与BM
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.05.07
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.05.07
** 描　述: 
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
