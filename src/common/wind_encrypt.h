/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
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
w_err_t wind_encrypt_init(w_uint8_t *passwd, w_int32_t len);
w_int32_t wind_encrypt(w_uint8_t *data, w_int32_t len);
w_int32_t wind_decrypt(w_uint8_t *data, w_int32_t len);

#endif