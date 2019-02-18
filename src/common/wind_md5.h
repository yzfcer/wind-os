/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_md5.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.2.18
**��        ��: MD5�㷨ģ��
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 2019.2.18
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 2019.2.18
** �衡��: 
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