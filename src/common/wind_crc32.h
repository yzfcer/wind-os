/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_crc32.h
** Author      : Jason Zhou
** Last Date   : 2017.12.10
** Description : 32 bit CRC verification method
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
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
#ifndef WIND_CRC32_H__
#define WIND_CRC32_H__

#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

void wind_crc32_init(void);

w_uint32_t wind_crc32(w_uint8_t *data,w_uint32_t size,w_uint32_t crc);


#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_CRC32_H__
