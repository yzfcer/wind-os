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
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_encrypt.h"

static void rc4_init(w_uint8_t *sbox, w_uint8_t *key, w_int32_t len)
{
    w_int32_t i = 0, j = 0;
    w_uint8_t k[256] = { 0 };
    w_uint8_t tmp = 0;
    for (i = 0; i<256; i++)
    {
        sbox[i] = (w_uint8_t)i;
        k[i] = key[i%len];
    }
    for (i = 0; i<256; i++)
    {
        j = (j + sbox[i] + k[i]) % 256;
        tmp = sbox[i];
        sbox[i] = sbox[j];
        sbox[j] = tmp;
    }
}

static w_int32_t rc4_crypt(w_encypt_ctx_s *ctx,w_uint8_t *data, w_int32_t len)
{
    w_int32_t i = 0, j = 0, t = 0;
    w_int32_t k = 0;
    w_uint8_t tmp;
    for (k = 0; k<len; k++)
    {
        i = (i + 1) % 256;
        j = (j + ctx->sbox[i]) % 256;
        tmp = ctx->sbox[i];
        ctx->sbox[i] = ctx->sbox[j];
        ctx->sbox[j] = tmp;
        t = (ctx->sbox[i] + ctx->sbox[j]) % 256;
        data[k] ^= ctx->sbox[t];
    }
    return len;
}
 
w_err_t wind_encrypt_init(w_encypt_ctx_s *ctx,w_uint8_t *passwd, w_int32_t len)
{
    WIND_ASSERT_RETURN(passwd != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(len > ENCRYPT_PASSWD_MIN_LEN,W_ERR_INVALID);
    rc4_init(ctx->sbox,passwd,len);
    return W_ERR_OK;

}
w_int32_t wind_encrypt(w_encypt_ctx_s *ctx,w_uint8_t *data, w_int32_t len)
{
    return rc4_crypt(ctx,data,len);
}
w_int32_t wind_decrypt(w_encypt_ctx_s *ctx,w_uint8_t *data, w_int32_t len)
{
    return rc4_crypt(ctx,data,len);
}
