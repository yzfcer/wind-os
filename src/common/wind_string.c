/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_string.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 重写的字符串函数
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
#include "wind_type.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_macro.h"


char* wind_strcpy(char *dest,const char *src)
{
    w_int32_t i;
    for(i = 0;;i ++)
    {
        dest[i] = src[i];
        if(dest[i] == '\0')
            break;
    }
    return dest;
}

char* wind_strncpy(char *dest,const char *src,w_int32_t count)
{
    w_int32_t i;
    for(i = 0;i < count;i ++)
    {
        dest[i] = src[i];
        if(dest[i] == '\0')
            break;
    }
    dest[count -1] = '\0';
    return dest;
}



char *wind_strcat(char *dest, const char *src)
{
    w_int32_t i,j;
    i = wind_strlen(dest);
    for(j = 0;;j ++,i++)
    {
        dest[i] = src[j];
        if(dest[i] == '\0')
            break;
    }
    return dest;
}


char* wind_strncat(char *dest, const char *src, w_int32_t count)
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

w_int32_t wind_strcmp(const char *dest,const char *src)
{
    w_int32_t err;
    while (1) {
        if ((err = (*dest - *src++)) != 0 || !*dest++)
            break;
    }
    return err;
}

w_int32_t wind_strncmp(const char* dest,const char *src,w_int32_t count)
{
    w_int32_t err = 0;
    while (count) {
        if ((err = *dest - *src++) != 0 || !*dest++)
            break;
        count--;
    }

    return err;
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

w_int32_t wind_strnlen(const char *s, w_int32_t count)
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

char *wind_strpbrk(const char *dest,const char *src)
{
    const char *sc1,*sc2;

    for( sc1 = dest; *sc1 != '\0'; ++sc1) {
        for( sc2 = src; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *) sc1;
        }
    }
    return W_NULL;
}

char *wind_strupper(char *dest)
{
    w_int32_t i;
    for(i = 0;;i ++)
    {
        dest[i] = UPCASE(dest[i]);
        if(dest[i] == '\0')
            break;
    }
    return dest;
}

extern char *wind_strlower(char *dest)
{
    w_int32_t i;
    for(i = 0;;i ++)
    {
        dest[i] = LOWERCASE(dest[i]);
        if(dest[i] == '\0')
            break;
    }
    return dest;
}

//描述:从字符串中查找特定的子串
//s1     : 原字符串
//s2     : 需要查找的子串
//返回值 : 第一个匹配的位置
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


//描述:从字符串中跳过特定的子串
//str    : 原字符串
//charr  : 需要跳过的子串
//count  : 原字符串最大长度
//返回值 : 跳过子串后的新字符串
char *wind_strskip(char *str,char *charr,w_int32_t count)
{
    w_int32_t i;
    for(;;)
    {
        for(i = 0;i < count;i ++)
        {
            if(*str == charr[i])
            {
                str ++;
                break;
            }
                
        }
        if(i >= count)
            break;
    }
    if(*str == '\0')
        return W_NULL;
    return str;
}


w_fp64_t wind_strtod(const char *str,char **endptr) 
{
    const  char*  p = str; 
    w_lfp64_t value = 0.0L; 
    w_lfp64_t  factor; 
    w_int32_t i,sign  = 0; 
    w_int32_t ispositive = 1;
    w_int32_t exp=0;
    while ( *p==' ' )
        p++;
    if(*p == '-' || *p == '+') 
        sign = *p++;
    while (*p <='9'&&*p>='0' )   
        value = value*10 + (*p++ - '0'); 
    if ( *p == '.' ) 
    { 
        factor = 1; 
        p++; 
        while ( *p <='9'&&*p>='0' ) 
        { 
            factor *= 0.1; 
            value  += (*p++ - '0') * factor; 
        } 
    } 
    if(*p=='e'||*p=='E') 
    { 
        p++;
        if((*p)=='-') 
        {  
            ispositive=0;
        }
        if((*p)=='+') 
        { 
            ispositive=1; 
        }
        while(*++p) 
        {  
            exp+=exp*10+*p-'0';
        }

        if(ispositive==0)
        {
            for(i=0;i <exp;i++) 
                value*=0.1; 
        }
        else
            for(i=0;i <exp;i++) 
                value*=10; 
    } 
    return (sign == '-' ? -value : value); 
} 


//描述:将字符串按照特定字符分割，最多分割成maxcnt个字符串
//str : 原字符串
//ch  : 指定的分隔字符
//substr : 返回分割后的子串
//maxcnt : 最多分割的子串数量
//返回值 : 实际分割的子串数量
w_int32_t wind_strsplit(char *str,char ch,char **substr,w_int32_t maxcnt)
{
    w_int32_t i,j,cnt = 0;
    int len = wind_strlen(str)+1;
    j = 0;
    for(i = 0;i < maxcnt;i ++)
    {
        if(cnt >= maxcnt)
            return -1;
        substr[i] = &str[j];
        for(;j < len;j ++)
        {
            if((str[j] == ch) || (str[j] == '\0'))
            {
                cnt ++;
                str[j] = 0;
                j ++;
                break;
            }
        }
        if(j >= len)
            break;
    }
    cnt = (substr[cnt-1][0] != 0?cnt:cnt -1);
    return cnt;
}


void *wind_memset(void *s,char c,w_uint32_t count)
{
    char *xs = (char *) s;

    while (count--)
        *xs++ = c;

    return s;
}

char *wind_bcopy(const char *src, char *dest, w_int32_t count)
{
    char *tmp = dest;

    while (count--)
        *tmp++ = *src++;

    return dest;
}

void *wind_memcpy(void *dest,const void *src,w_int32_t count)
{
    char *tmp = (char *)dest;
    char *s = (char *) src;
    while (count--)
        *tmp++ = *s++;
    return dest;
}


void *wind_memmove(void *dest,const void *src,w_int32_t count)
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

w_int32_t wind_memcmp(const void *dest,const void *src,w_int32_t count)
{
    const w_uint8_t *su1, *su2;
    signed char res = 0;

    for( su1 = dest, su2 = src; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

void *wind_memscan(void *addr, w_uint32_t c, w_int32_t size)
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

