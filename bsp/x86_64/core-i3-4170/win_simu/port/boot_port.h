/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_port.c
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
#ifndef BOOT_PORT_H__
#define BOOT_PORT_H__
#include "wind_type.h"
#include "boot_part.h"
#include <stdio.h>

w_err_t    boot_enter_main_hook(void);
w_err_t    boot_run_system_hook(void);
w_int32_t  boot_receive_img(w_part_s *part);

w_uint32_t boot_get_sys_ms(void);
void       boot_jump_to_system(void);

w_err_t    boot_parts_create(void);

void       boot_feed_watchdog(void);
w_bool_t   boot_is_chip_lock();
void       boot_set_chip_lock(w_uint8_t lockflag);

#endif
