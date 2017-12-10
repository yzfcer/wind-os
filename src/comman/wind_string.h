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
#endif

extern char*  wind_strcpy(char *dest,const char *src);

extern char* wind_strncpy(char *dest,const char *src,w_uint32_t count);

extern char * wind_strcat(char * dest, const char * src);

extern char* wind_strncat(char *dest, const char *src, w_uint32_t count);

extern w_int32_t wind_strcmp(const char *dest,const char *src);

extern w_int32_t wind_strncmp(const char* cs,const char * ct,w_uint32_t count);

extern char* wind_strchr(const char * s, int c);

extern char * wind_strrchr(const char * s, int c);

extern w_int16_t wind_strlen(const char *str);

extern w_uint32_t wind_strnlen(const char * s, w_uint32_t count);

extern w_uint32_t wind_strspn(const char *s, const char *accept);

extern char * wind_strpbrk(const char * cs,const char * ct);

extern void * wind_memset(void * s,char c,w_uint32_t count);

extern char * wind_bcopy(const char * src, char * dest, int count);

extern void * wind_memcpy(void * dest,const void *src,w_uint32_t count);

extern void * wind_memmove(void * dest,const void *src,w_uint32_t count);

extern int wind_memcmp(const void * cs,const void * ct,w_uint32_t count);

extern void * wind_memscan(void * addr, int c, w_uint32_t size);

extern char * wind_strstr(const char * s1,const char * s2);



#ifdef __cplusplus
}
#endif

#endif

