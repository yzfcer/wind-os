/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_port.c
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2017/04/08
** Description :  
** Others      : 
** Modify      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#include "boot_port.h"
#include <stdio.h>
#include "boot_param.h"
#include "boot_part.h"
#include "wind_debug.h"

w_err_t boot_enter_main_hook(void)
{
    return W_ERR_OK;
}

w_err_t boot_run_system_hook(void)
{
    return W_ERR_OK;
}

w_err_t boot_parts_create(void)
{
    boot_part_create(PART_BOOT,0);
    boot_part_create(PART_PARAM1,0);
    boot_part_create(PART_PARAM2,0);
    boot_part_create(PART_IMGPARA,0);
    boot_part_create(PART_SYSRUN,0);
    boot_part_create(PART_FS,0);
    
    boot_part_create(PART_IMG1,1);
    boot_part_create(PART_IMG2,1);
    
    boot_part_create(PART_CACHE,0);
    boot_part_create(PART_SHARE,0);
    
    return W_ERR_OK;
}


void boot_jump_to_system(void)
{
    char ch;
    while(1)
    {
        scanf("%c",&ch);
        if((ch == '\n') || (ch == '\r'))
        wind_printf("boot can not operate in system mode.please restart the bootloader.\r\n");
    }
}

void boot_feed_watchdog(void)
{

}

w_bool_t boot_is_chip_lock()
{
    return W_FALSE;
}

void boot_set_chip_lock(w_uint8_t lockflag)
{

}












