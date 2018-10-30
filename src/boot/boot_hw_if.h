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
#ifndef BOOT_INPUT_IF_H__
#define BOOT_INPUT_IF_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
void boot_delay(w_uint32_t ms);

w_err_t read_char_blocking(char *ch);
w_int32_t read_line_blockig(char *buff,w_int32_t len);
w_err_t wait_for_key_input(w_int32_t to_sec,char *ch,w_int32_t print32_t_flag);

#ifdef __cplusplus
}
#endif

#endif
