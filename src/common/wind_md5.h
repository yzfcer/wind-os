/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_md5.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.2.18
**描        述: MD5算法模块
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2019.2.18
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2019.2.18
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_MD5_H__
#define WIND_MD5_H__
#include "wind_type.h"
   

typedef struct { 
    w_uint32_t state[4];        /* state (ABCD) */ 
    w_uint32_t count[2]; /* number of bits, modulo 2^64 (lsb first) */ 
    unsigned char buffer[64];      /* input buffer */ 
} w_md5_ctx_s; 
   
void MD5Init (w_md5_ctx_s *context); 
void MD5Update (w_md5_ctx_s *context, unsigned char*input, unsigned int inputLen); 
//void MD5UpdaterString(w_md5_ctx_s *context,const char *string); 
//int MD5FileUpdateFile(w_md5_ctx_s *context,char*filename); 
void MD5Final (w_md5_ctx_s *context,unsigned char digest[16]); 
//void MDString (char*string,unsigned char digest[16]); 
//int MD5File (char *filename,unsigned char digest[16]);


#endif