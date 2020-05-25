/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_crc32.c
** Author      : Jason Zhou
** Last Date   : 2017.12.10
** Description : 32 bit CRC verification method
**              
**--------------History---------------------------------------------------------------------------------
Author: 
** Version     : v1.0
** Date        : 2017.12.10
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2017.12.10
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

static w_uint32_t crc32_table[256];
void wind_crc32_init(void)
{
    w_uint32_t c;
    w_uint16_t i = 0;
    w_uint8_t bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (w_uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c >> 1)^(0xEDB88320);
            }
            else
            {
                c =c >> 1;
            }
        }
        crc32_table[i] = c;
    }
}

w_uint32_t wind_crc32(w_uint8_t *data,w_uint32_t size,w_uint32_t crc)
{
    static w_int32_t init_flag = 0;
    if(!init_flag)
    {
        wind_crc32_init();
        init_flag = 1;
    }
    while(size--)
        crc = (crc >> 8)^(crc32_table[(crc ^ *data++)&0xff]);
    return crc;
}
#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
