/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_string.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ��д���ַ���������ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_STRING_H_
#define WIND_STRING_H_


#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

char* wind_strcpy(char *dest,const char *src);

char* wind_strncpy(char *dest,const char *src,w_int32_t count);

char *wind_strcat(char *dest, const char *src);

char* wind_strncat(char *dest, const char *src, w_int32_t count);

w_int32_t wind_strcmp(const char *dest,const char *src);

w_int32_t wind_strncmp(const char* dest,const char *src,w_int32_t count);

char* wind_strchr(const char *dest, w_uint32_t c);

char *wind_strrchr(const char *dest, char c);

w_int32_t wind_strlen(const char *dest);

w_int32_t wind_strnlen(const char *dest, w_int32_t count);

w_uint32_t wind_strspn(const char *dest, const char *accept);

char *wind_strpbrk(const char *dest,const char *src);

char *wind_strupper(char *dest);

char *wind_strlower(char *dest);

char *wind_strstr(const char *str,const char *sub);

char *wind_strskip(char *str,char *sub,w_int32_t count);

w_fp64_t wind_strtod(const char *str,char **endptr);

w_int32_t wind_strsplit(char *str,char ch,char **substr,w_int32_t maxcnt);

void *wind_memset(void *mem,char c,w_uint32_t count);

char *wind_bcopy(const char *src, char *dest, w_int32_t count);

void *wind_memcpy(void *dest,const void *src,w_int32_t count);

void *wind_memmove(void *dest,const void *src,w_int32_t count);

w_int32_t wind_memcmp(const void *dest,const void *src,w_int32_t count);

void *wind_memscan(void *mem, w_uint32_t c, w_int32_t count);

#ifdef __cplusplus
}
#endif

#endif

