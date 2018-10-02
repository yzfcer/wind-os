/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_string.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ��д���ַ�������
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
#include "wind_type.h"
#include "wind_string.h"
#include "wind_debug.h"


char* wind_strcpy(char *dest,const char *src)
{
    char *tmp = dest;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

char* wind_strncpy(char *dest,const char *src,w_uint32_t count)
{
    char *tmp = dest;
    while (count-- && (*dest++ = *src++) != '\0');
    return tmp;
}



char *wind_strcat(char *dest, const char *src)
{
    char *tmp = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}


char* wind_strncat(char *dest, const char *src, w_uint32_t count)
{
    char *tmp = dest;
    if (count) {
        while (*dest)
            dest++;
        while (1) {
            *dest++ = *src++;
            if (--count == 0) {
                *dest = '\0';
                break;
            }
        }
    }
    return tmp;
}

w_int32_t wind_strcmp(const char *cs,const char *ct)
{
    char ret;
    while (1) {
        if ((ret = (char)(*cs - *ct++)) != 0 || !*cs++)
            break;
    }
    return ret;
}

w_int32_t wind_strncmp(const char* cs,const char *ct,w_uint32_t count)
{
    char ret = 0;

    while (count) {
        if ((ret = *cs - *ct++) != 0 || !*cs++)
            break;
        count--;
    }

    return ret;
}

char* wind_strchr(const char *s, w_uint32_t c)
{
    for(; *s != (char) c; ++s)
        if (*s == '\0')
            return (char*)W_NULL;
    return (char *) s;
}

char *wind_strrchr(const char *s, char c)
{
       const char *p = (const char *)(s + wind_strlen(s));
       do {
       if (*p == (char)c)
           return (char *)p;
       } while (--p >= s);
       return (char*)W_NULL;
}


w_int32_t wind_strlen(const char *s)
{
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc);
    return (w_int32_t)(sc - s);
}

w_uint32_t wind_strnlen(const char *s, w_uint32_t count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc);
    return sc - s;
}


w_uint32_t wind_strspn(const char *s, const char *accept)
{
    const char *p;
    const char *a;
    w_uint32_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a)
                break;
        }
        if (*a == '\0')
            return count;
        ++count;
    }

    return count;
}

char *wind_strpbrk(const char *cs,const char *ct)
{
    const char *sc1,*sc2;

    for( sc1 = cs; *sc1 != '\0'; ++sc1) {
        for( sc2 = ct; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *) sc1;
        }
    }
    return W_NULL;
}



void *wind_memset(void *s,char c,w_uint32_t count)
{
    char *xs = (char *) s;

    while (count--)
        *xs++ = c;

    return s;
}

char *wind_bcopy(const char *src, char *dest, w_uint32_t count)
{
    char *tmp = dest;

    while (count--)
        *tmp++ = *src++;

    return dest;
}

void *wind_memcpy(void *dest,const void *src,w_uint32_t count)
{
    char *tmp = (char *) dest, *s = (char *) src;

    while (count--)
        *tmp++ = *s++;

    return dest;
}


void *wind_memmove(void *dest,const void *src,w_uint32_t count)
{
    char *tmp, *s;

    if (dest <= src) 
    {
        tmp = (char *) dest;
        s = (char *) src;
        while (count--)
            *tmp++ = *s++;
    }
    else 
    {
        tmp = (char *) dest + count;
        s = (char *) src + count;
        while (count--)
            *--tmp = *--s;
    }

    return dest;
}

w_int32_t wind_memcmp(const void *cs,const void *ct,w_uint32_t count)
{
    const w_uint8_t *su1, *su2;
    signed char res = 0;

    for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

/*
 *find the first occurrence of byte 'c', or 1 past the area if none
 */
void *wind_memscan(void *addr, w_uint32_t c, w_uint32_t size)
{
    w_uint8_t *p = (w_uint8_t *) addr;

    while (size) {
        if (*p == c)
            return (void *) p;
        p++;
        size--;
    }
    return (void *) p;
}

char *wind_strstr(const char *s1,const char *s2)
{
    w_int32_t l1, l2;

    l2 = wind_strlen(s2);
    if (!l2)
        return (char *) s1;
    l1 = wind_strlen(s1);
    while (l1 >= l2) {
        l1--;
        if (!wind_memcmp(s1,s2,l2))
            return (char *) s1;
        s1++;
    }
    return W_NULL;
}

