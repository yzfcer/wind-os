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
#include "wind_strsearch.h"
#include <wind_string.h>
#include <wind_debug.h>

void kmp_init(w_kmp_context_s *ctx)
{
    if(ctx == W_NULL)
        return;
    wind_memset(ctx,0,sizeof(w_kmp_context_s));
}

void kmp_get_next(w_kmp_context_s *ctx,char *substr)
{
    w_int32_t k = -1;
    w_int32_t j = 0;
    w_int32_t len = wind_strlen(substr);
    ctx->next[j] = k;
    while (j < len)
    {
        if ( (k == -1) || (substr[j] == substr[k]) ) //注意等号是==，而不是=
        {
            ++k; // 注意是先加后使用
            ++j;
            ctx->next[j] = k;
            ctx->len = j + 1;
        }
        else
            k = ctx->next[k]; 
    }
}
 
w_int32_t kmp_search(w_kmp_context_s *ctx,char *str, char *substr, w_int32_t pos)
{
    w_int32_t i;
    w_int32_t j;
    w_int32_t slen,sublen;
    if(ctx == W_NULL)
        return -1;
    if(ctx->len == 0)
        return -1;
 
    i = pos;
    j = 0; 
    slen = wind_strlen(str);
    sublen = wind_strlen(substr);
    while ((i < slen) && (j < sublen))
    {
        if ( (j == -1)  || str[i] == substr[j])
        {
            i++;
            j++;
        }
        else
            j = ctx->next[j];
    }
 
    if (sublen == j)
        return i - sublen;
    else
        return -1;
}
 
void kmp_print_next(w_kmp_context_s *ctx)
{
   w_int32_t i;
   for (i = 0; i < ctx->len; i++) 
       wind_printf("next[%d] = %d\n", i, ctx->next[i]);
}

w_int32_t bm_search(char *str, char *substr, w_int32_t pos)
{
    w_int32_t i,j;
    w_int32_t slen,sublen;
 
    i = pos;
    j = 0;
    slen = wind_strlen(str);
    sublen = wind_strlen(substr);
    while ( (i < slen) && (j < sublen) )
    {
        if (str[i] == substr[j])
        {
            i++;
            j++;
        }
        else
        {
            i = i - j + 1; 
            j = 0;
        }
    }
 
    if (sublen == j)
        return i - sublen;
    else
        return -1;
}

#if 0
w_kmp_context_s ctx;
w_int32_t strsearch_test(void)
{
    char *str = "ababcabcacbab";
    char *substr = "abcac";
    w_int32_t pos = 0;
    w_int32_t index;

 
    wind_printf("================ BM ==============\n");
    index = bm_search(str, substr, pos);
    wind_printf("index = %d\n", index);
 
    wind_printf("================ KMP ==============\n");
    kmp_get_next(&ctx,substr);
    kmp_print_next(&ctx);
 
    index = kmp_search(&ctx,str, substr, pos);
    wind_printf("index = %d\n", index);
}

#endif
