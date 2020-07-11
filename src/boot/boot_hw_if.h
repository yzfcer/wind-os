/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_hw_if.h
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2017/04/08
** Description : 
** Others      : 
** History     :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#ifndef BOOT_INPUT_IF_H__
#define BOOT_INPUT_IF_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
w_uint32_t boot_get_sys_ms(void);
w_err_t boot_read_char_blocking(char *ch);
w_int32_t boot_read_line_blockig(char *buff,w_int32_t len);
w_err_t boot_wait_input(w_int32_t to_sec,char *ch,w_int32_t print32_t_flag);

#ifdef __cplusplus
}
#endif

#endif
