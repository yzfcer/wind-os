/*********************************************************************************
*Copyright(C),2017-2020,yzfcer@163.com
*FileName    :    
*** Author      :      Jason Zhou
*Version:     1.0
*Date:        2017/04/08
*Description:  
*Others:  
*History:  
 1.Date:
   ** Author      :
   Modification:
**********************************************************************************/
#ifndef BOOT_IMG_H__
#define BOOT_IMG_H__
#include "wind_type.h"
#include "boot_part.h"

#ifdef __cplusplus
extern "C" {
#endif


w_part_s *boot_img_get_old_part(void);
w_part_s *boot_img_get_new_normal_img(void);

w_err_t boot_img_download(void);
w_err_t boot_img_flush_cache_to_part(w_part_s **part,w_int32_t count);

w_err_t boot_img_clear_all(void);
w_err_t boot_img_check(void);

#ifdef __cplusplus
}
#endif

#endif
