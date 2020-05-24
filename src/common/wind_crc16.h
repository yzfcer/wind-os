/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_crc16.h
** Author      : Jason Zhou
** Last Date   : 2017.12.10
** Description : 16位CRC校验方法
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
#ifndef WIND_CRC16_H__
#define WIND_CRC16_H__
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

w_uint16_t wind_crc16(const unsigned char *data,int len,w_uint16_t crc);


#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_CRC16_H__
