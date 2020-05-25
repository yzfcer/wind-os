/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_debug.h
** Author      : Jason Zhou
** Last Date   : 2012.09.26
** Description : Debug information printing module
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2012.09.26
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2012.10.20
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_conv.h"
#include "wind_std.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

#if WIND_DEBUG_SUPPORT
extern void wind_disable_switch(void);
extern void wind_enable_switch(void);
w_err_t wind_std_wait(w_int32_t ms);

#define ZEROPAD 1       
#define SIGN    2       
#define PLUS    4       
#define SPACE   8      
#define LEFT    16      
#define SPECIAL 32      
#define LARGE   64      

w_int32_t _div(w_int32_t* n,unsigned base)
{
     w_int32_t res; 
     res = ((w_uint32_t) *n) % (unsigned) base; 
     *n = ((w_uint32_t) *n) / (unsigned) base; 
     return res;
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


static char * wind_dec(char *str,w_int32_t dec)
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
    w_int32_t tmpint = 0;

    tmpint = (w_int32_t)flt;
    str = wind_dec(str,tmpint);
    *str++ = '.';
    flt = flt - tmpint;
    tmpint = (w_int32_t)(flt * 1000000);
    str = wind_dec(str,tmpint);
    return str;
}

static char *wind_number(char *str,w_int32_t num,w_int32_t base,w_int32_t size,w_int32_t precision,w_int32_t type)
{
    char c,sign,tmp[66];
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
        }else if (type & PLUS) {
            sign = '+';
            size--;
        }else if (type & SPACE) {
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
            tmp[i++] = digits[do_div(num,base)];
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

w_int32_t wind_vsprintf(char *buf,const char *fmt,wind_va_list args)
{
    w_int32_t len;
    w_uint32_t num;
    w_int32_t i,base;
    char *str;
    w_fp64_t vargdouble;
    //w_uint32_t vargint;
    const char *s;
    w_int32_t flags;      /* flags to wind_number() */
    w_int32_t field_width;    /* width of output field */
    w_int32_t precision;      /* min. # of digits for integers; max
                   wind_number of chars for from string */
    w_int32_t qualifier;      /* 'h','l',or 'L' for integer fields */

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
            field_width = wind_va_arg(args,w_int32_t);
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
                precision = wind_va_arg(args,w_int32_t);
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
            *str++ = (w_uint8_t)wind_va_arg(args,w_int32_t);
            while (--field_width > 0)
                *str++ = ' ';
            continue;

        case 's':
            s = wind_va_arg(args,char *);
            len = wind_strnlen(s,65535);

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
                     (w_int32_t)(w_addr_t)wind_va_arg(args,void *),16,
                     field_width,precision,flags);
            continue;

        case 'n':
            if (qualifier == 'l') {
                w_int32_t *ip = wind_va_arg(args,w_int32_t *);
                *ip = (str - buf);
            }else {
                w_int32_t *ip = wind_va_arg(args,w_int32_t *);
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
            vargint = wind_va_arg(args,w_uint32_t);
            str = wind_binary(str,vargint);
            break;
        #endif

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;
        case 'f':
        case 'g':
            
            vargdouble = (w_fp64_t)wind_va_arg(args,w_fp64_t);
            /*wind_va_arg(ap,type),if type is narrow type (char,short,float) an error is given in strict ANSI
            mode,or a warning otherwise.In non-strict ANSI mode,'type' is allowed to be any expression. */
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
            num = wind_va_arg(args,w_uint32_t);
        else if (qualifier == 'h') {
            num = (unsigned short)wind_va_arg(args,w_int32_t);
            if (flags & SIGN)
                num = (short)num;
        }else if (flags & SIGN)
            num = wind_va_arg(args,w_int32_t);
        else
            num = wind_va_arg(args,w_uint32_t);
        str = wind_number(str,num,base,field_width,precision,flags);
    }
    *str = '\0';
    return str - buf;
}
 
w_int32_t wind_printf(const char *fmt,...)
{
    static char buff[512];
    w_int32_t count;
#ifdef USE_SYS_VSPRINTF
    va_list args;
#else
	wind_va_list args;
#endif

    wind_disable_switch();
    wind_memset(buff,0,sizeof(buff));
#ifdef USE_SYS_VSPRINTF
	va_start(args,fmt);
	count = vsprintf(buff,fmt,args);
	va_end(args);
#else
    wind_va_start(args,fmt);
	count = wind_vsprintf(buff,fmt,args);
    wind_va_end(args);
#endif
	wind_std_output((w_uint8_t *)buff,count);
    wind_enable_switch();
    return count;
}

w_int32_t wind_sprintf(char *buff,const char *fmt,...)
{
    w_int32_t count;
#ifdef USE_SYS_VSPRINTF
    va_list args;
    va_start(args,fmt);
	count = vsprintf(buff,fmt,args);
    va_end(args);
#else
    wind_va_list args;
    wind_va_start(args,fmt);
	count = wind_vsprintf(buff,fmt,args);
    wind_va_end(args);
#endif
    return count;
}

void wind_print_space(w_int32_t space8_cnt)
{
    w_int32_t i;
    for(i = 0;i < space8_cnt;i ++)
        wind_printf("--------");
    wind_printf("\r\n");
}

#ifndef HUGE_VAL
#define HUGE_VAL (99.e99)
#endif /* HUGE_VAL */

enum _WVSCANF__flag
{
    WVSCANF_Suppress = 0x2U,/*!< Suppress Flag. */
    WVSCANF_DestMask = 0x7cU,/*!< Destination Mask. */
    WVSCANF_DestChar = 0x4U,/*!< Destination Char Flag. */
    WVSCANF_DestString = 0x8U,/*!< Destination String FLag. */
    WVSCANF_DestSet = 0x10U,/*!< Destination Set Flag. */
    WVSCANF_DestInt = 0x20U,/*!< Destination w_int32_t Flag. */
    WVSCANF_DestFloat = 0x30U,/*!< Destination Float Flag. */
    WVSCANF_LengthMask = 0x1f00U,/*!< Length Mask Flag. */
    WVSCANF_LengthChar = 0x100U,/*!< Length Char Flag. */
    WVSCANF_LengthShortInt = 0x200U,/*!< Length ShortInt Flag. */
    WVSCANF_LengthLongInt = 0x400U,/*!< Length LongInt Flag. */
    WVSCANF_LengthLongLongInt = 0x800U,/*!< Length LongLongInt Flag. */
    WVSCANF_LengthLongLongDouble = 0x1000U,/*!< Length LongLongDuoble Flag. */
    WVSCANF_TypeSinged = 0x2000U,/*!< TypeSinged Flag. */
};


static w_uint32_t skip_space(const char **s)
{
    w_uint8_t count = 0;
    w_uint8_t c;
    c = **s;
    while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\v') || (c == '\f'))
    {
        count++;
        (*s)++;
        c = **s;
    }
    return count;
}


w_int32_t wind_vsscanf(const char *str,const char *format,wind_va_list args)
{
    static w_fp64_t fnum = 0.0;
    w_uint8_t base;
    w_int8_t neg;
    const char *c = format;
    char temp;
    char *buf;
    w_uint16_t flag = 0;
    w_uint32_t field_width;
    w_uint32_t nassigned = 0;
    w_uint32_t n_decode = 0;
    w_int32_t val;
    const char *s;
    const char *p = str;

    if (*p == '\0')
    {
        return -1;
    }

    /* Decode directives. */
    while ((*c) && (*p))
    {
        /* Ignore all white-spaces in the format strings. */
        if (skip_space((const char **)&c))
        {
            n_decode += skip_space(&p);
        }
        else if ((*c != '%') || ((*c == '%') && (*(c + 1) == '%')))
        {
            /* Ordinary characters. */
            c++;
            if (*p == *c)
            {
                n_decode++;
                p++;
                c++;
            }
            else
            {
                /* Match failure. Misalignment with C99,the unmatched characters need to be pushed back to stream.
                 * However,it is deserted now. */
                break;
            }
        }
        else
        {
            /* convernsion specification */
            c++;
            /* Reset. */
            flag = 0;
            field_width = 0;
            base = 0;

            /* Loop to get full conversion specification. */
            while ((*c) && (!(flag & WVSCANF_DestMask)))
            {
                switch (*c)
                {

                    case '*':
                        if (flag & WVSCANF_Suppress)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        flag |= WVSCANF_Suppress;
                        c++;
                        break;
                    case 'h':
                        if (flag & WVSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }

                        if (c[1] == 'h')
                        {
                            flag |= WVSCANF_LengthChar;
                            c++;
                        }
                        else
                        {
                            flag |= WVSCANF_LengthShortInt;
                        }
                        c++;
                        break;
                    case 'l':
                        if (flag & WVSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }

                        if (c[1] == 'l')
                        {
                            flag |= WVSCANF_LengthLongLongInt;
                            c++;
                        }
                        else
                        {
                            flag |= WVSCANF_LengthLongInt;
                        }
                        c++;
                        break;

                    case 'L':
                        if (flag & WVSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        flag |= WVSCANF_LengthLongLongDouble;
                        c++;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (field_width)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        do
                        {
                            field_width = field_width * 10 + *c - '0';
                            c++;
                        }while ((*c >= '0') && (*c <= '9'));
                        break;
                    case 'd':
                        base = 10;
                        flag |= WVSCANF_TypeSinged;
                        flag |= WVSCANF_DestInt;
                        c++;
                        break;
                    case 'u':
                        base = 10;
                        flag |= WVSCANF_DestInt;
                        c++;
                        break;
                    case 'o':
                        base = 8;
                        flag |= WVSCANF_DestInt;
                        c++;
                        break;
                    case 'x':
                    case 'X':
                        base = 16;
                        flag |= WVSCANF_DestInt;
                        c++;
                        break;
                    case 'i':
                        base = 0;
                        flag |= WVSCANF_DestInt;
                        c++;
                        break;
                    case 'a':
                    case 'A':
                    case 'e':
                    case 'E':
                    case 'f':
                    case 'F':
                    case 'g':
                    case 'G':
                        flag |= WVSCANF_DestFloat;
                        c++;
                        break;
                    case 'c':
                        flag |= WVSCANF_DestChar;
                        if (!field_width)
                        {
                            field_width = 1;
                        }
                        c++;
                        break;
                    case 's':
                        flag |= WVSCANF_DestString;
                        c++;
                        break;
                    default:
                        return nassigned;
                }
            }

            if (!(flag & WVSCANF_DestMask))
            {
                /* Format strings are exhausted. */
                return nassigned;
            }

            if (!field_width)
            {
                /* Large than length of a line. */
                field_width = 99;
            }

            /* Matching strings in input streams and assign to argument. */
            switch (flag & WVSCANF_DestMask)
            {
                case WVSCANF_DestChar:
                    s = (const char *)p;
                    buf = wind_va_arg(args,char *);
                    while ((field_width--) && (*p))
                    {
                        if (!(flag & WVSCANF_Suppress))
                        {
                            *buf++ = *p++;
                        }
                        else
                        {
                            p++;
                        }
                        n_decode++;
                    }

                    if ((!(flag & WVSCANF_Suppress)) && (s != p))
                    {
                        nassigned++;
                    }
                    break;
                case WVSCANF_DestString:
                    n_decode += skip_space(&p);
                    s = p;
                    buf = wind_va_arg(args,char *);
                    while ((field_width--) && (*p != '\0') && (*p != ' ') && (*p != '\t') && (*p != '\n') &&
                           (*p != '\r') && (*p != '\v') && (*p != '\f'))
                    {
                        if (flag & WVSCANF_Suppress)
                        {
                            p++;
                        }
                        else
                        {
                            *buf++ = *p++;
                        }
                        n_decode++;
                    }

                    if ((!(flag & WVSCANF_Suppress)) && (s != p))
                    {
                        /* Add NULL to end of string. */
                        *buf = '\0';
                        nassigned++;
                    }
                    break;
                case WVSCANF_DestInt:
                    n_decode += skip_space(&p);
                    s = p;
                    val = 0;
                    if ((base == 0) || (base == 16))
                    {
                        if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X')))
                        {
                            base = 16;
                            if (field_width >= 1)
                            {
                                p += 2;
                                n_decode += 2;
                                field_width -= 2;
                            }
                        }
                    }

                    if (base == 0)
                    {
                        if (s[0] == '0')
                        {
                            base = 8;
                        }
                        else
                        {
                            base = 10;
                        }
                    }

                    neg = 1;
                    switch (*p)
                    {
                        case '-':
                            neg = -1;
                            n_decode++;
                            p++;
                            field_width--;
                            break;
                        case '+':
                            neg = 1;
                            n_decode++;
                            p++;
                            field_width--;
                            break;
                        default:
                            break;
                    }

                    while ((*p) && (field_width--))
                    {
                        if ((*p <= '9') && (*p >= '0'))
                        {
                            temp = *p - '0';
                        }
                        else if ((*p <= 'f') && (*p >= 'a'))
                        {
                            temp = *p - 'a' + 10;
                        }
                        else if ((*p <= 'F') && (*p >= 'A'))
                        {
                            temp = *p - 'A' + 10;
                        }
                        else
                        {
                            temp = base;
                        }

                        if (temp >= base)
                        {
                            break;
                        }
                        else
                        {
                            val = base * val + temp;
                        }
                        p++;
                        n_decode++;
                    }
                    val *= neg;
                    if (!(flag & WVSCANF_Suppress))
                    {
                        switch (flag & WVSCANF_LengthMask)
                        {
                            case WVSCANF_LengthChar:
                                if (flag & WVSCANF_TypeSinged)
                                {
                                    *wind_va_arg(args,w_int8_t *) = (w_int8_t)val;
                                }
                                else
                                {
                                    *wind_va_arg(args,w_uint8_t *) = (w_uint8_t)val;
                                }
                                break;
                            case WVSCANF_LengthShortInt:
                                if (flag & WVSCANF_TypeSinged)
                                {
                                    *wind_va_arg(args,signed short *) = (signed short)val;
                                }
                                else
                                {
                                    *wind_va_arg(args,unsigned short *) = (unsigned short)val;
                                }
                                break;
                            case WVSCANF_LengthLongInt:
                                if (flag & WVSCANF_TypeSinged)
                                {
                                    *wind_va_arg(args,w_int32_t *) = (w_int32_t)val;
                                }
                                else
                                {
                                    *wind_va_arg(args,w_uint32_t *) = (w_uint32_t)val;
                                }
                                break;
                            case WVSCANF_LengthLongLongInt:
                                if (flag & WVSCANF_TypeSinged)
                                {
                                    *wind_va_arg(args,w_int64_t *) = (w_int64_t)val;
                                }
                                else
                                {
                                    *wind_va_arg(args,w_uint64_t *) = (w_uint64_t)val;
                                }
                                break;
                            default:
                                /* The default type is the type w_int32_t. */
                                if (flag & WVSCANF_TypeSinged)
                                {
                                    *wind_va_arg(args,w_int32_t *) = (w_int32_t)val;
                                }
                                else
                                {
                                    *wind_va_arg(args,w_uint32_t *) = (w_uint32_t)val;
                                }
                                break;
                        }

                        nassigned++;
                    }
                    break;
                case WVSCANF_DestFloat:
                    n_decode += skip_space(&p);
                    fnum = wind_strtod(p,(char **)&s);

                    if ((fnum >= HUGE_VAL) || (fnum <= -HUGE_VAL))
                    {
                        break;
                    }

                    n_decode += (w_uint32_t)((w_addr_t)(s) - (w_addr_t)(p));
                    p = s;
                    if (!(flag & WVSCANF_Suppress))
                    {
                        if (flag & WVSCANF_LengthLongLongDouble)
                        {
                            *wind_va_arg(args,double *) = fnum;
                        }
                        else
                        {
                            *wind_va_arg(args,float *) = (float)fnum;
                        }
                        nassigned++;
                    }
                    break;
                default:
                    return nassigned;
            }
        }
    }
    return nassigned;
}


#if 1
static w_int32_t scan_read_line(char *buff,w_int32_t maxlen)
{
    char ch;
    w_int32_t i = 0,len = 0;
    while(1)
    {
        len = wind_std_input((w_uint8_t*)&ch,1);
        if(len <= 0)
            wind_std_wait(5);
        else
        {
            if((ch == 0x0d) || (ch == 0x0a))
            {
                buff[i] = '\0';
                return i;
            }
            else
            {
                buff[i++] = ch;
                if(i >= maxlen)
                {
                    buff[maxlen-1] = '\0';
                    return maxlen;
                }
            }
        }
    }
}

w_int32_t wind_scanf(const char *fmt,...)
{
    static char buff[512];
    wind_va_list args;
    w_int32_t count;
    wind_disable_switch();
    count = scan_read_line(buff,sizeof(buff));
    wind_va_start(args,fmt);
    count = wind_vsscanf(buff,fmt,args);
    wind_va_end(args);
    wind_enable_switch();
    return count;
}
#endif

w_int32_t wind_sscanf(const char *buff,const char *fmt,...)
{
    wind_va_list args;
    w_int32_t count;
    wind_va_start(args,fmt);
    count = wind_vsscanf(buff,fmt,args);
    wind_va_end(args);
    return count;
}


#endif
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus

