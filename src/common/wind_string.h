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

extern char*  wind_strcpy(char *dest,const char *src);

extern char* wind_strncpy(char *dest,const char *src,w_uint32_t count);

extern char *wind_strcat(char *dest, const char *src);

extern char* wind_strncat(char *dest, const char *src, w_uint32_t count);

extern w_int32_t wind_strcmp(const char *dest,const char *src);

extern w_int32_t wind_strncmp(const char* cs,const char *ct,w_uint32_t count);

extern char* wind_strchr(const char *s, w_uint32_t c);

extern char *wind_strrchr(const char *s, char c);

extern w_int32_t wind_strlen(const char *str);

extern w_uint32_t wind_strnlen(const char *s, w_uint32_t count);

extern w_uint32_t wind_strspn(const char *s, const char *accept);

extern char *wind_strpbrk(const char *cs,const char *ct);

extern void *wind_memset(void *s,char c,w_uint32_t count);

extern char *wind_bcopy(const char *src, char *dest, w_uint32_t count);

extern void *wind_memcpy(void *dest,const void *src,w_uint32_t count);

extern void *wind_memmove(void *dest,const void *src,w_uint32_t count);

extern w_int32_t wind_memcmp(const void *cs,const void *ct,w_uint32_t count);

extern void *wind_memscan(void *addr, w_uint32_t c, w_uint32_t size);

extern char *wind_strstr(const char *s1,const char *s2);



#ifdef __cplusplus
}
#endif

#endif

