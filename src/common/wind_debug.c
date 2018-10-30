/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.c
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式
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
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_std.h"

#if WIND_DEBUG_SUPPORT


#ifdef  __cplusplus
#define _ADDRESSOF(v)   (&reinterpret_cast<const char &>(v) )
#else
#define _ADDRESSOF(v)   (&(v))
#endif

#define _INTSIZEOF(n)   ((sizeof(n) + sizeof(w_int32_t) - 1) & ~(sizeof(w_int32_t) - 1))

#define _crt_va_start(ap,v)  ( ap = (wind_va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
#define _crt_va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define _crt_va_end(ap)      ( ap = (wind_va_list)0 )

#define wind_va_start _crt_va_start /* windows stdarg.h */
#define wind_va_arg _crt_va_arg
#define wind_va_end _crt_va_end
#define do_div(n,base) _div(&n,base)

#define ZEROPAD 1       
#define SIGN    2       
#define PLUS    4       
#define SPACE   8      
#define LEFT    16      
#define SPECIAL 32      
#define LARGE   64      

w_int32_t _div(w_int32_t* n,unsigned base)
{
     w_int32_t __res; 
     __res = ((w_uint32_t) *n) % (unsigned) base; 
     *n = ((w_uint32_t) *n) / (unsigned) base; 
     return __res;
}

static w_int32_t isdigit(w_int32_t ch)
{
    return (ch >= '0') && (ch <= '9');
}

static w_int32_t skip_atoi(const char **s)
{
    w_int32_t i = 0;

    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}


static char * wind_dec(char *str,int dec)
{
    if(dec==0)
    {
        return str;
    }
    str = wind_dec(str,dec/10);
    *str++ = (char)(dec%10 + '0');
    return str;
}

static char * wind_fp64(char *str,w_fp64_t flt)
{
    int tmpint = 0;

    tmpint = (int)flt;
    str = wind_dec(str,tmpint);
    *str++ = '.';
    flt = flt - tmpint;
    tmpint = (int)(flt * 1000000);
    str = wind_dec(str,tmpint);
    return str;
}

static char *wind_number(char *str, w_int32_t num, w_int32_t base, w_int32_t size, w_int32_t precision,w_int32_t type)
{
    char c, sign, tmp[66];
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    w_int32_t i;

    if (type & LARGE)
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[i++] = digits[do_div(num, base)];
        }
    }

    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL) {
        if (base == 8)
            *str++ = '0';
        else if (base == 16) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

char * wind_binary(char *str,w_uint32_t bin)
{
    if(bin == 0)
    {
        //*str++ = '0';
        //*str++ = 'b';
        return str;
    }
    str = wind_binary(str,bin/2);
    *str++ = (char)(bin%2 + '0');
    return str;
}

w_int32_t wind_vsprintf(char *buf, const char *fmt, wind_va_list args)
{
    w_int32_t len;
    w_uint32_t num;
    w_int32_t i, base;
    char *str;
    w_fp64_t vargdouble;
    w_uint32_t vargint;
    const char *s;
    w_int32_t flags;      /* flags to wind_number() */
    w_int32_t field_width;    /* width of output field */
    w_int32_t precision;      /* min. # of digits for integers; max
                   wind_number of chars for from string */
    w_int32_t qualifier;      /* 'h', 'l', or 'L' for integer fields */

    for (str = buf; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        /* process flags */
        flags = 0;
          repeat:
        ++fmt;      /* this also skips first '%' */
        switch (*fmt) {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;
            /* it's the next argument */
            field_width = wind_va_arg(args, w_int32_t);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                /* it's the next argument */
                precision = wind_va_arg(args, w_int32_t);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char)wind_va_arg(args, w_int32_t);
            while (--field_width > 0)
                *str++ = ' ';
            continue;

        case 's':
            s = wind_va_arg(args, char *);
            len = wind_strnlen(s, precision);

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            continue;

        case 'p':
            if (field_width == -1) {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }
            str = wind_number(str,
                     (w_uint32_t)wind_va_arg(args, void *), 16,
                     field_width, precision, flags);
            continue;

        case 'n':
            if (qualifier == 'l') {
                w_int32_t *ip = wind_va_arg(args, w_int32_t *);
                *ip = (str - buf);
            } else {
                w_int32_t *ip = wind_va_arg(args, w_int32_t *);
                *ip = (str - buf);
            }
            continue;

        case '%':
            *str++ = '%';
            continue;

            /* integer wind_number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'b':
        case 'B':
            vargint = wind_va_arg(args, w_uint32_t);
            str = wind_binary(str,vargint);
            break;


        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;
        case 'f':
            
            vargdouble = (w_fp64_t)wind_va_arg(args, w_fp64_t);
            /*va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
            mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
            str = wind_fp64(str,vargdouble);
            break;
        default:
            *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            continue;
        }
        if (qualifier == 'l')
            num = wind_va_arg(args, w_uint32_t);
        else if (qualifier == 'h') {
            num = (unsigned short)wind_va_arg(args, w_int32_t);
            if (flags & SIGN)
                num = (short)num;
        } else if (flags & SIGN)
            num = wind_va_arg(args, w_int32_t);
        else
            num = wind_va_arg(args, w_uint32_t);
        str = wind_number(str, num, base, field_width, precision, flags);
    }
    *str = '\0';
    return str - buf;
}
 
 
w_int32_t wind_printf(const char *fmt, ...)
{
    static char buff[512];
    wind_va_list args;
    w_int32_t count;
    wind_std_lock();
    wind_memset(buff,0,sizeof(buff));
    wind_va_start(args, fmt);
    count = wind_vsprintf(buff, fmt, args);
    wind_va_end(args);
    wind_std_output((w_uint8_t *)buff, count);
    wind_std_unlock();
    return count;
}

w_int32_t wind_sprintf(char *buff, const char *fmt, ...)
{
    wind_va_list args;
    w_int32_t count;
    wind_va_start(args, fmt);
    count = wind_vsprintf(buff, fmt, args);
    wind_va_end(args);
    return count;
}

void wind_print_space(w_int32_t space8_cnt)
{
    w_int32_t i;
    for(i = 0;i < space8_cnt;i ++)
        wind_printf("--------");
    wind_printf("\r\n");
}



#endif


