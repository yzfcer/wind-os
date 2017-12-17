/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_conv.c
**创   建   人: Jason Zhou
**最后修改日期: 2017.12.10
**描        述: 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2017.12.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2017.12.10
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_string.h"
//把字符串转换成带符号整数
w_bool_t wind_atoi(char *str,w_int32_t *value)
{
    w_uint8_t sign;
    w_uint8_t i = 0;
    w_int32_t num = 0;
    
    sign = (str[0] == '-')?1:0;
    if(str[i]=='+'||str[i]==' -')
        i++;
    for(;;i ++)
    {
        if(str[i] <= '9' && str[i] >= '0')
            num = num *10 + str[i] - '0';
        else if(str[i] != 0)
            return B_FALSE;
        else
            break;
    }
    *value = sign?-num:num;
    return B_TRUE;
}

void wind_itoa(char *str,w_int32_t value)
{
    char *beg = str;  
    int sign;  
    char *end;
    if ((sign = value) < 0) 
        value = -value;  
    do
    {  
        *str++ = '0' + value % 10;  
    }while((value /= 10) > 0);  
  
    if (sign < 0) *str++ = '-';  
  
    *str = '\0';  
  
    end = str - 1;  
    while(beg < end)
    {  
        char tmp = *beg;  
        *beg++ = *end;  
        *end-- = tmp;  
    }  
}

//把字符串转换成无符号整数
w_bool_t wind_atoui(char *str,w_uint32_t *value)
{
    w_uint32_t num = 0;
    w_uint8_t i = 0;
    if(str[i]=='+'||str[i]==' -')
        i++;
    for(;;i ++)
    {
        if(str[i] <= '9' && str[i] >= '0')
            num = num *10 + str[i] - '0';
        else if(str[i] != 0)
            return B_FALSE;
        else
            break;
    }
    *value = num;
    return B_TRUE;
}

void wind_uitoa(char *str,w_uint32_t value)
{
    char *beg = str;
    char *end;
    char tmp;
    do
    {  
        *str++ = '0' + value % 10;  
    }while((value /= 10) > 0);  
    *str = '\0';  
  
    end = str - 1;  
    while(beg < end)
    {  
        tmp = *beg;  
        *beg++ = *end;  
        *end-- = tmp;  
    }  
}

static w_int32_t c2i(char ch)  
{  
    if((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    if((ch >= 'A') && (ch <= 'Z'))
        return ch - 0x40;
    if((ch >= 'a') && (ch <= 'z'))
        return ch - 0x60;
    return -1;
}

static char i2c(w_uint8_t va)
{
    if(va < 10)
        return va + 0x30;
    return va - 10 + 0x60;
}


w_bool_t wind_htoi(char *hex,w_uint32_t *value)  
{  
    w_uint32_t num = 0;  
    w_int32_t len;  
    w_int32_t temp;  
    w_int32_t bits;  
    w_int32_t i;  

    len = wind_strlen(hex);  
    for (i=0, temp=0; i<len; i++, temp=0)  
    {  
        temp = c2i( *(hex + i));
        if(temp < 0)
            return B_FALSE;
        bits = (len - i - 1) *4;  
        temp = temp << bits;  
        num = num | temp;  
    }
    *value = num;
    return B_TRUE;  
}  

void wind_itoh(char *hex,w_uint32_t value)
{
    char *beg = hex;
    char *end;
    char tmp;
    do
    {  
        *hex++ = i2c(value&0x0f);  
    }while((value >> 4) > 0);  
    *hex = '\0';  
  
    end = hex - 1;  
    while(beg < end)
    {  
        tmp = *beg;  
        *beg++ = *end;  
        *end-- = tmp;  
    }
}
