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
#include <windows.h>
#include <winbase.h>
#include "boot_param.h"
#include "boot_part.h"
//#include "boot_media.h"
#include <conio.h>
#include "wind_debug.h"

w_err_t boot_enter_main_hook(void)
{
    return W_ERR_OK;
}

w_err_t boot_exit_hook(void)
{
    return W_ERR_OK;
}



w_int32_t boot_receive_img(w_part_s *part)
{
    FILE *file;
    w_err_t err;
    w_int32_t len,offset;
    w_uint8_t *buff;
    do
    {
        err = W_ERR_OK;
        buff = get_common_buffer();
        errno = fopen_s(&file,"imgfile.none.img","rb");
        WIND_ASSERT_RETURN(errno == 0,-1);
        WIND_ASSERT_RETURN(file != W_NULL,-1);
        offset = 0;
        while(1)
        {
            len = fread((w_uint8_t*)buff,1,COMMBUF_SIZE,file);
            if(len > 0)
            {
                len = boot_part_write(part,offset,buff,len);
                WIND_ASSERT_BREAK(len > 0,W_ERR_FAIL,"write part fail");
                offset += len;
            }
            else
                break;
        }
        fclose(file);
    }while(0);
    if(err != W_ERR_OK)
        return -1;
    return part->datalen;
}

w_uint32_t boot_get_sys_ms(void)
{
    return GetTickCount();
}


w_err_t boot_parts_create(void)
{
    boot_part_create(PART_BOOT,BOOT_SIZE,0);
    boot_part_create(PART_PARAM1,PARAM1_SIZE,0);
    boot_part_create(PART_PARAM2,PARAM2_SIZE,0);
    boot_part_create(PART_IMGPARA,IMG_PARA_SIZE,0);
    boot_part_create(PART_SYSRUN,SYSRUN_SIZE,0);
    boot_part_create(PART_FS,FS_SIZE,0);
    
    boot_part_create(PART_IMG1,IMG1_SIZE,1);
    boot_part_create(PART_IMG2,IMG2_SIZE,1);
    
    boot_part_create(PART_CACHE,CACHE_SIZE,0);
    boot_part_create(PART_SHARE,SHARE_SIZE,0);
    
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

void boot_set_chip_lock(w_uint8_t lock_enable)
{

}












