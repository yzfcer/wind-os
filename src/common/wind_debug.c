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
#define _WIND_ADDRESSOF(v)   (&reinterpret_cast<const char &>(v) )
#else
#define _WIND_ADDRESSOF(v)   (&(v))
#endif

#define _WIND_INTSIZEOF(n)   ((sizeof(n) + sizeof(w_int32_t) - 1) & ~(sizeof(w_int32_t) - 1))

#define _wind_crt_va_start(ap,v)  ( ap = (wind_va_list)_WIND_ADDRESSOF(v) + _WIND_INTSIZEOF(v) )
#define _wind_crt_va_arg(ap,t)    ( *(t *)((ap += _WIND_INTSIZEOF(t)) - _WIND_INTSIZEOF(t)) )
#define _wind_crt_va_end(ap)      ( ap = (wind_va_list)0 )

#define wind_va_start _wind_crt_va_start /* windows stdarg.h */
#define wind_va_arg _wind_crt_va_arg
#define wind_va_end _wind_crt_va_end
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

static w_int32_t wind_isdigit(w_int32_t ch)
{
    return (ch >= '0') && (ch <= '9');
}

static w_int32_t skip_atoi(const char **s)
{
    w_int32_t i = 0;

    while (wind_isdigit(**s))
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

#if 0
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
#endif

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
        if (wind_isdigit(*fmt))
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
            if (wind_isdigit(*fmt))
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
        #if 0
        case 'b':
        case 'B':
            vargint = wind_va_arg(args, w_uint32_t);
            str = wind_binary(str,vargint);
            break;
        #endif

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;
        case 'f':
            
            vargdouble = (w_fp64_t)wind_va_arg(args, w_fp64_t);
            /*wind_va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
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
#if 0
#include "wind_macro.h"
#define isspace(c) ((c) == ' '?W_TRUE:W_FALSE)
#define TOLOWER(c) LOWERCASE(c)
#define isdigit(c) DECCHK(c)
#define isxdigit(c) HEXCHK(c)
#define unlikely(x) (x)
#define _tolower(x) LOWERCASE(x)
const char *skip_spaces(const char *str)
{
    while(*str == ' ')
        ++str;
    return str;
}

static unsigned int simple_guess_base(const char *cp)
{
 if (cp[0] == '0') {
  if (TOLOWER(cp[1]) == 'x' && isxdigit(cp[2]))
   return 16;
  else
   return 8;
 } else {
  return 10;
 }
}

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
    unsigned long long result;
    unsigned int rv;

    cp = _parse_integer_fixup_radix(cp, &base);
    rv = _parse_integer(cp, base, &result);
    /* FIXME */
    cp += (rv & ~KSTRTOX_OVERFLOW);

    if (endp)
        *endp = (char *)cp;

    return result;
}

unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
    return simple_strtoull(cp, endp, base);
}

long long simple_strtoll(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -simple_strtoull(cp + 1, endp, base);
    return simple_strtoull(cp, endp, base);
}

long simple_strtol(const char *cp, char **endp, unsigned int base)
{
    if (*cp == '-')
        return -simple_strtoul(cp + 1, endp, base);
    return simple_strtoul(cp, endp, base);
}

int wind_vsscanf(const char *buf, const char *fmt, wind_va_list args)
{
    const char *str = buf;
    char *next;
    char digit;
    int num = 0;
    w_uint8_t qualifier;
    unsigned int base;
    union {
        long long s;
        unsigned long long u;
    } val;
    w_int16_t field_width;
    w_bool_t is_sign;

    while (*fmt) {
        /* skip any white space in format */
        /* white space in format matchs any amount of
         * white space, including none, in the input.
         */
        if (isspace(*fmt)) {
            fmt = skip_spaces(++fmt);
            str = skip_spaces(str);
        }

        /* anything that is not a conversion must match exactly */
        if (*fmt != '%' && *fmt) {
            if (*fmt++ != *str++)
                break;
            continue;
        }

        if (!*fmt)
            break;
        ++fmt;

        /* skip this conversion.
         * advance both strings to next white space
         */
        if (*fmt == '*') {
            if (!*str)
                break;
            while (!isspace(*fmt) && *fmt != '%' && *fmt)
                fmt++;
            while (!isspace(*str) && *str)
                str++;
            continue;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) {
            field_width = skip_atoi(&fmt);
            if (field_width <= 0)
                break;
        }

        /* get conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
            _tolower(*fmt) == 'z') {
            qualifier = *fmt++;
            if (unlikely(qualifier == *fmt)) {
                if (qualifier == 'h') {
                    qualifier = 'H';
                    fmt++;
                } else if (qualifier == 'l') {
                    qualifier = 'L';
                    fmt++;
                }
            }
        }

        if (!*fmt)
            break;

        if (*fmt == 'n') {
            /* return number of characters read so far */
            *wind_va_arg(args, int *) = str - buf;
            ++fmt;
            continue;
        }

        if (!*str)
            break;

        base = 10;
        is_sign = W_FALSE;

        switch (*fmt++) {
        case 'c':
        {
            char *s = (char *)wind_va_arg(args, char*);
            if (field_width == -1)
                field_width = 1;
            do {
                *s++ = *str++;
            } while (--field_width > 0 && *str);
            num++;
        }
        continue;
        case 's':
        {
            char *s = (char *)wind_va_arg(args, char *);
            if (field_width == -1)
                field_width = SHRT_MAX;
            /* first, skip leading white space in buffer */
            str = skip_spaces(str);

            /* now copy until next white space */
            while (*str && !isspace(*str) && field_width--)
                *s++ = *str++;
            *s = '\0';
            num++;
        }
        continue;
        case 'o':
            base = 8;
            break;
        case 'x':
        case 'X':
            base = 16;
            break;
        case 'i':
            base = 0;
        case 'd':
            is_sign = W_TRUE;
        case 'u':
            break;
        case '%':
            /* looking for '%' in str */
            if (*str++ != '%')
                return num;
            continue;
        default:
            /* invalid format; stop here */
            return num;
        }

        /* have some sort of integer conversion.
         * first, skip white space in buffer.
         */
        str = skip_spaces(str);

        digit = *str;
        if (is_sign && digit == '-')
            digit = *(str + 1);

        if (!digit
            || (base == 16 && !isxdigit(digit))
            || (base == 10 && !isdigit(digit))
            || (base == 8 && (!isdigit(digit) || digit > '7'))
            || (base == 0 && !isdigit(digit)))
            break;

        if (is_sign)
            val.s = qualifier != 'L' ?
                simple_strtol(str, &next, base) :
                simple_strtoll(str, &next, base);
        else
            val.u = qualifier != 'L' ?
                simple_strtoul(str, &next, base) :
                simple_strtoull(str, &next, base);

        if (field_width > 0 && next - str > field_width) {
            if (base == 0)
                _parse_integer_fixup_radix(str, &base);
            while (next - str > field_width) {
                if (is_sign)
                    val.s = div_s64(val.s, base);
                else
                    val.u = div_u64(val.u, base);
                --next;
            }
        }

        switch (qualifier) {
        case 'H':    /* that's 'hh' in format */
            if (is_sign)
                *wind_va_arg(args, signed char *) = val.s;
            else
                *wind_va_arg(args, unsigned char *) = val.u;
            break;
        case 'h':
            if (is_sign)
                *wind_va_arg(args, short *) = val.s;
            else
                *wind_va_arg(args, unsigned short *) = val.u;
            break;
        case 'l':
            if (is_sign)
                *wind_va_arg(args, long *) = val.s;
            else
                *wind_va_arg(args, unsigned long *) = val.u;
            break;
        case 'L':
            if (is_sign)
                *wind_va_arg(args, long long *) = val.s;
            else
                *wind_va_arg(args, unsigned long long *) = val.u;
            break;
        //case 'Z':
        //case 'z':
        //    *wind_va_arg(args, size_t*) = val.u;
        //    break;
        default:
            if (is_sign)
                *wind_va_arg(args, int *) = val.s;
            else
                *wind_va_arg(args, unsigned int *) = val.u;
            break;
        }
        num++;

        if (!next)
            break;
        str = next;
    }

    return num;
}
#endif
 
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


