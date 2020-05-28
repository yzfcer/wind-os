/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_md5.h
** Author      : Jason Zhou
** Last Date   : 2019.2.18
** Description : MD5 algorithm function
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2019.2.18
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2019.2.18
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_MD5_H__
#define WIND_MD5_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

typedef struct { 
    w_uint32_t state[4];
    w_uint32_t bits[2];
    w_uint8_t  buffer[64];
}w_md5_ctx_s; 
   
void wind_md5_init   (w_md5_ctx_s *ctx); 
void wind_md5_update (w_md5_ctx_s *ctx,w_uint8_t *data,w_uint32_t len); 
void wind_md5_final  (w_md5_ctx_s *ctx,w_uint8_t digest[16]); 

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
#endif //#ifndef WIND_MD5_H__