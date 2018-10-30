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
#ifndef BOOT_SHARE_PARAM_H__
#define BOOT_SHARE_PARAM_H__
#include "wind_config.h"
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SHARE_PARAM_MAGIC 0x52d360a5
#define SHARE_BASE 0x2002F000
#define SHARE_LENTH 0x1000
#define SHARE_VERSION 0x0001

typedef struct
{
    w_uint32_t magic;
    w_uint32_t lenth;
    w_uint32_t share_version;
    w_uint32_t upgrade_flag;

    w_part_s part[PART_COUNT];        
}share_param_s;

w_err_t share_param_init(void);
w_err_t share_param_reset(void);

share_param_s *share_param_get(void);

w_err_t share_param_flush(share_param_s *sp);



#ifdef __cplusplus
}
#endif

#endif

