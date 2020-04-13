/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_string.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 重写的字符串函数的头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_STRING_H_
#define WIND_STRING_H_


#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

char* wind_strcpy(char *dest,const char *src);

char* wind_strncpy(char *dest,const char *src,w_int32_t count);

char *wind_strcat(char *dest,const char *src);

char* wind_strncat(char *dest,const char *src,w_int32_t count);

w_int32_t wind_strcmp(const char *dest,const char *src);

w_int32_t wind_strncmp(const char* dest,const char *src,w_int32_t count);

char* wind_strchr(const char *dest,w_uint32_t c);

char* wind_strnchr(const char *dest,w_uint32_t c,w_int32_t count);

char *wind_strrchr(const char *dest,char c);

w_int32_t wind_strlen(const char *dest);

w_int32_t wind_strnlen(const char *dest,w_int32_t count);

w_uint32_t wind_strspn(const char *dest,const char *accept);

char *wind_strpbrk(const char *dest,const char *src);

char *wind_strupper(char *dest);

char *wind_strlower(char *dest);

char *wind_strstr(const char *str,const char *sub);

char *wind_strskip(char *str,char *sub,w_int32_t count);

void *wind_strrpc(char *str,char oldch,char newch);

w_fp64_t wind_strtod(const char *str,char **endptr);

w_int32_t wind_strsplit(char *str,char ch,char **substr,w_int32_t maxcnt);

void *wind_memset(void *mem,char c,w_uint32_t count);

char *wind_bcopy(const char *src,char *dest,w_int32_t count);

void *wind_memcpy(void *dest,const void *src,w_int32_t count);

void *wind_memmove(void *dest,const void *src,w_int32_t count);

w_int32_t wind_memcmp(const void *dest,const void *src,w_int32_t count);

void *wind_memscan(void *mem,w_uint32_t c,w_int32_t count);


#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_STRING_H_

