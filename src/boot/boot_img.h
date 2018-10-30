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
#ifndef BOOT_IMG_H__
#define BOOT_IMG_H__
#include "wind_type.h"
#include "boot_part.h"

#ifdef __cplusplus
extern "C" {
#endif
#define IMG_MAGIC 0x4528AB6D
typedef struct __img_head_s img_head_s;
struct __img_head_s
{
    w_uint32_t magic;
    w_uint32_t img_len;
    w_uint32_t head_len;
    w_uint32_t head_ver;
    w_uint32_t bin_ver;
    w_uint32_t bin_crc;
    w_uint32_t bin_offset;
    w_uint32_t encrypt_type;
    char       img_name[64];
    char       board_name[32];
    char       arch_name[32];
    char       cpu_name[32];
    w_uint32_t head_crc;
};

w_part_s *get_old_part(void);
w_part_s *boot_img_get_new_normal_img(void);

w_err_t boot_img_download(void);
w_err_t boot_img_flush_cache(void);
w_err_t boot_img_flush_cache_to_part(w_part_s **part,w_int32_t count);

w_err_t boot_img_clear_all(void);
w_err_t boot_img_check(void);

#ifdef __cplusplus
}
#endif

#endif
