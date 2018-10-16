/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/11/04
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef WIND_ENCRYPT_H__
#define WIND_ENCRYPT_H__
#include "wind_type.h"
#define ENCRYPT_PASSWD_MIN_LEN 12
typedef struct 
{
    w_uint8_t sbox[256];
}w_encypt_ctx_s;

w_err_t wind_encrypt_init(w_encypt_ctx_s *ctx,w_uint8_t *passwd, w_int32_t len);
w_int32_t wind_encrypt(w_encypt_ctx_s *ctx,w_uint8_t *data, w_int32_t len);
w_int32_t wind_decrypt(w_encypt_ctx_s *ctx,w_uint8_t *data, w_int32_t len);

#endif