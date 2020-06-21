/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_tftp.h
** Author      : Jason Zhou
** Last Date   : 2020-06-16
** Description : 
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020-06-16
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2020-06-16
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TFTP_H__
#define WIND_TFTP_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#if WIND_NET_TFTP_SUPPORT

#define TFTP_RRQ     0x0001
#define TFTP_WRQ     0x0002
#define TFTP_DATA    0x0003
#define TFTP_ACK     0x0004
#define TFTP_ERR     0x0005
#define TFTP_OPTACK  0x0006
typedef struct
{
    char *filename;
    char *type;
    char *optname;
    char *optvalue;
}w_tftp_wrq_s;

typedef struct
{
    char *optname;
    char *optvalue;
}w_tftp_optack_s;

typedef struct
{
    w_uint16_t blknum;
    w_uint8_t buff;
}w_tftp_data_s;





#endif // #if WIND_NET_TFTP_SUPPORT
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_TFTP_H__

