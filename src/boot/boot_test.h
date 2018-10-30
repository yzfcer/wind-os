/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef BOOT_TEST_H__
#define BOOT_TEST_H__
#include "wind_type.h"
#include "wind_config.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif

#if BOOT_TEST_ENABLE
typedef enum 
{
    ERR_SFlash,
    ERR_XRam,
    ERR_PRama,
    ERR_APP_TYPE,
    ERR_CNT
}sys_error;

typedef struct __boot_stub
{
    w_uint8_t err[ERR_CNT];
}boot_stub_s;


typedef struct 
{
    char key;
    char *item_desc;
    void (*test)(void);
}boot_test_s;
void set_error(w_uint8_t err_type,w_uint8_t err);
w_uint8_t get_error(w_uint8_t err_type);
w_err_t test_entry(void);
#else
#define get_error(t) 0
#endif

#ifdef __cplusplus
}
#endif

#endif

