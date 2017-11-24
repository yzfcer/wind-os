/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.c
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "wind_config.h"
#include "wind_type.h"
#include "wind_debug.h"

//static const w_int8_t * const hexlist = "0123456789abcdef";
extern w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
#if 0
int wind_printf(const w_int8_t *string,...)
{
    w_uint16_t index, value, pos, cnt, base, ulNeg;
    w_int8_t *pcStr, buff[16], chfill;
    va_list vaArgP;
    
    if(string == NULL)
        return 0;
    wind_close_interrupt();
    va_start(vaArgP, string);
    while(*string)
    {
        for(index = 0; (string[index] != '%') && (string[index] != '\0');index++);
            wind_std_output((w_uint8_t *)string, index);
        string += index;
        if(*string == '%')
        {
            string++;
            cnt = 0;
            chfill = ' ';
again:
            switch(*string++)
            {
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
                {
                    if((string[-1] == '0') && (cnt == 0))
                    {
                        chfill = '0';
                    }
                    cnt *= 10;
                    cnt += string[-1] - '0';
                    goto again;
                }
            case 'c':
                {
                    value = va_arg(vaArgP, w_uint16_t);
                    wind_std_output((w_uint8_t *)(&value), 1);
                    break;
                }
            case 'd':
                {
                    value = va_arg(vaArgP, w_uint16_t);
                    pos = 0;
                    if((long)value < 0)
                    {
                        value = -(long)value;
                        ulNeg = 1;
                    }
                    else
                    {
                        ulNeg = 0;
                    }
                    base = 10;
                    goto convert;
                }
            case 's':
                {
                    pcStr = va_arg(vaArgP, w_int8_t *);
                    for(index = 0; pcStr[index] != '\0'; index++);
                    wind_std_output((w_uint8_t *)pcStr, index);
                    if(cnt > index)
                    {
                        cnt -= index;
                        while(cnt--)
                            wind_std_output((w_uint8_t *)" ", 1);
                    }
                    break;
                }
            case 'u':
                {
                    value = va_arg(vaArgP, w_uint16_t);
                    pos = 0;
                    base = 10;
                    ulNeg = 0;
                    goto convert;
                }
            case 'x':
            case 'X':
            case 'p':
                {
                    value = va_arg(vaArgP, w_uint16_t);
                    pos = 0;
                    base = 16;
                    ulNeg = 0;

convert:
                    for(index = 1;
                        (((index * base) <= value) &&
                        (((index * base) / base) == index));
                    index *= base, cnt--);
                    if(ulNeg)
                        cnt--;
                    if(ulNeg && (chfill == '0'))
                    {
                        buff[pos++] = '-';
                        ulNeg = 0;
                    }
                    if((cnt > 1) && (cnt < 16))
                    {
                        for(cnt--; cnt; cnt--)
                            buff[pos++] = chfill;
                    }
                    if(ulNeg)
                        buff[pos++] = '-';
                    for(; index; index /= base)
                        buff[pos++] = hexlist[(value / index) % base];
                    wind_std_output((w_uint8_t *)buff, pos);
                    break;
                }
            case '%':
                    wind_std_output((w_uint8_t *)string - 1, 1);
                    //wind_std_output("%",1);
                    break;
            default:
                    wind_std_output((w_uint8_t *)"ERROR", 5);
                    break;
            }
        }
    }
    wind_open_interrupt();
    return 0;
}

#endif

#if 0
#define ZEROPAD    1        /* pad with zero */ 
#define SIGN    2        /* unsigned/signed long */ 
#define PLUS    4        /* show plus */ 
#define SPACE    8        /* space if plus */ 
#define LEFT    16        /* left justified */ 
#define SPECIAL    32        /* 0x */ 
#define LARGE    64        /* use 'ABCDEF' instead of 'abcdef' */ 
int wind_vsprintf(char *buf, const char *fmt, va_list args) ;
#endif

w_int32_t wind_printf(const char *fmt, ...) 
{ 
    static char sprint_buf[1024];
    va_list args; 
    w_int32_t n;
    wind_close_interrupt();
    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    sprint_buf[n] = 0;
    va_end(args);
    wind_std_output((w_uint8_t *)sprint_buf, n);
    wind_open_interrupt();
    return n; 
}



