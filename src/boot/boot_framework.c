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
#include "boot_framework.h"
#include "boot_param.h"
#include "boot_menu.h"
#include "boot_port.h"
#include "boot_share_param.h"
#include "boot_part.h"
#include "boot_media.h"
#include "wind_debug.h"
#include "boot_img.h"
#include "boot_hw_if.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif

static volatile w_int32_t g_boot_status = BOOT_INIT;
void boot_status_set(boot_status_e status)
{
    g_boot_status = status;
}

w_int32_t boot_status_get(void)
{
    return g_boot_status;
}
void boot_status_go_next(void)
{
    g_boot_status ++;
}

void print_boot_info(void)
{
    wind_printf("\r\n+---------------------------------------------+\r\n");
    wind_printf("               wind-boot %d.%d.%d\r\n",(w_uint8_t)(BOOT_VERSION >> 16),
                (w_uint8_t)(BOOT_VERSION >> 8),(w_uint8_t)(BOOT_VERSION));
    wind_printf("      *** To Make Ease For Developing ***");
    wind_printf("\r\n+---------------------------------------------+\r\n");
    wind_printf("Build: %s %s\r\n",__DATE__,__TIME__);
    wind_printf("Arch : %s\r\n",ARCH_NAME);
    wind_printf("CPU  : %s\r\n",CPU_NAME);
    wind_printf("Borad: %s\r\n\r\n",BOARD_NAME);
}

static w_err_t boot_init(void)
{
    boot_param_init();
    boot_media_init();
    boot_part_init();
    share_param_init();
    boot_status_go_next();
    return W_ERR_OK;
}

static w_err_t boot_debug_mode_check(void)
{
    boot_param_s *bp = boot_param_get();
    
    if(bp->debug_mode)
    {
        wind_notice("bootloader mode:DEBUG");
        boot_status_set(BOOT_WAIT_KEY_PRESS);
    }
    else
    {
        wind_notice("bootloader mode:NORMAL");
        boot_status_go_next();
    }
    return W_ERR_OK;
}


static w_err_t boot_first_check(void)
{
    w_err_t err;
    boot_param_s *bp;
    bp = boot_param_from_rom();
    if(W_NULL != bp)
    {
        boot_status_go_next();
        wind_notice("find it is NOT the first running time.");
        return W_ERR_OK;
    }
    wind_notice("NO valid boot params found.");
    boot_param_reset();
    err = boot_param_flush();
    if(0 != err)
    {
        wind_error("write boot params failed.");
        boot_status_set(BOOT_ERROR);
        return W_ERR_FAIL;
    }
    boot_status_go_next();
    wind_notice("check first running time OK.");
    return W_ERR_OK;
    
}

static w_err_t boot_chip_lock_check(void)
{
    if(is_chip_lock())
    {
        wind_notice("MCU chip is locked.");
    }
    else
    {
        wind_warn("MCU chip is unlocked.");
    }
    boot_status_go_next();
    return W_ERR_OK;
}



static w_err_t boot_img_valid_check(void)
{
    w_err_t err;
    err = boot_img_check();
    boot_status_go_next();
    return err;
}



static w_err_t  boot_upgrade_check(void)
{
    w_err_t err;
    share_param_s *sp = share_param_get();
    
    if(!sp || !sp->upgrade_flag)
    {
        wind_notice("upgrade flags is invalid,need NOT update image.");
        boot_status_go_next();
        return W_ERR_OK;
    }

    sp->upgrade_flag = 0;
    share_param_flush(sp);
    wind_notice("handling upgrade event,please wait");
    
    err = boot_img_flush_cache();
    if(0 != err)
    {
        wind_warn("flush upgrade img failed.");
        boot_status_set(BOOT_ERROR);
        return W_ERR_FAIL;
    }
    
    err = boot_param_flush();
    if(0 != err)
    {
        wind_error("update params failed.");
        boot_status_set(BOOT_ERROR);
        return W_ERR_FAIL-1;
    }
    boot_status_go_next();
    return W_ERR_OK;
    
}


static w_err_t boot_wait_key_press(void)
{
    char ch = 0;

    boot_param_s *bp = (boot_param_s *)boot_param_get();
    wind_printf("press any key to enter menu list:");
    if(W_ERR_OK == wait_for_key_input(bp->wait_sec,&ch,1))
    {
        boot_status_go_next();
        wind_printf("\r\n");
        return W_ERR_OK;
    }
    boot_status_set(BOOT_LOAD_IMG);
    wind_printf("\r\n");
    return W_ERR_OK;
}

static w_err_t boot_enter_menu(void)
{
    w_err_t err;
    err = run_menu();
    if(err == W_ERR_OK)
        boot_status_go_next();
    else
        boot_status_set(BOOT_INIT);
    return W_ERR_OK;
}

static w_err_t boot_load_img(void)
{
    w_mem_status_e mem_stat = MEM_ERROR;
    w_part_s *part = W_NULL,*tmp;
    boot_param_s *bp = W_NULL; 

    wind_notice("load image to running space...");
    bp = (boot_param_s *)boot_param_get();
    part = boot_part_get(PART_SYSRUN);
    
    if(W_NULL == bp)
    {
        wind_error("get boot param failed.");
        boot_status_set(BOOT_ERROR);
        return W_ERR_FAIL;
    }
    
    // debug模式不需要检查，直接调入image
    if(bp->debug_mode)
    {
        boot_status_go_next();
        return W_ERR_OK;
    }
    tmp = boot_part_get(PART_IMG1);
    if(tmp->datalen <= 0)
    {
        wind_notice("program is NOT existing.");
        boot_status_set(BOOT_MENU_LIST);
        return W_ERR_FAIL;
    }
    tmp = boot_part_get(PART_SYSRUN);
    if(MEDIA_TYPE_ROM == tmp->mtype)
    {
        if(MEM_NORMAL == tmp->status)
        {
            mem_stat = MEM_NORMAL;
        }        
    }
    else 
    {
        tmp = boot_part_get(PART_IMG1);
        if(MEM_NORMAL == tmp->status)
        {
            mem_stat = MEM_NORMAL;
        }
    }
    if(MEM_NORMAL != mem_stat)
    {
        wind_warn("program has some ERRORs.");
        boot_status_set(BOOT_MENU_LIST);
        return W_ERR_FAIL;
    }

    if(MEDIA_TYPE_ROM == part->mtype)
    {
        wind_notice("need not load image to ROM.");
        boot_status_set(BOOT_SET_SHARE_PARAM);
        return W_ERR_OK;
    }
    else
    {
        wind_error("running memory type is not supported.");
        boot_status_set(BOOT_ERROR);
        return W_ERR_OK;
    }
    
}

static w_err_t boot_set_system_param(void)
{
    wind_notice("set share params for system");
    share_param_reset();
    boot_status_set(BOOT_RUN_SYSTEM);
    return W_ERR_OK;
}


static w_err_t boot_error_handle(void)
{
    char ch;
    w_err_t err;
    while(1)
    {
        err = read_char_blocking(&ch);
        if((err == W_ERR_OK) && (ch == '\n'))
            wind_error("Some error occured in bootloader and system boot failed.");
    }
    return W_ERR_OK;
}

static w_err_t boot_run_system(void)
{
    boot_exit_hook();
    wind_notice("jump to system running space\r\n\r\n\r\n");
    boot_jump_to_system();
    return W_ERR_OK;
}


boot_step_s g_status_handTB[] = 
{
    {BOOT_INIT,"boot init",boot_init},
    {BOOT_FIRST_CHECK,"first run check",boot_first_check},   
    {BOOT_DEBUG_MODE_CHECK,"debug mode check",boot_debug_mode_check},

    {BOOT_CHIP_LOCK_CHECK,"chip lock status check",boot_chip_lock_check},
    {BOOT_IMG_VALID_CHECK,"image valid check",boot_img_valid_check},
    {BOOT_UPGRADE_CHECK,"upgrade status check",boot_upgrade_check},
    
    {BOOT_WAIT_KEY_PRESS,"wait for any key press",boot_wait_key_press},
    {BOOT_MENU_LIST,"enter menu list",boot_enter_menu},
    {BOOT_LOAD_IMG,"load image",boot_load_img},
    {BOOT_SET_SHARE_PARAM,"set share param",boot_set_system_param},
    {BOOT_RUN_SYSTEM,"run system",boot_run_system},
    {BOOT_ERROR,"error",boot_error_handle},
};



void boot_loop(void)
{
    w_int32_t i;
    w_err_t err;
    boot_enter_main_hook();
    print_boot_info();
    wind_error("--------------------------part erase");
    while(1)
    {
        for(i = 0;i < sizeof(g_status_handTB)/sizeof(boot_step_s);i ++)
        {
            if(g_boot_status == g_status_handTB[i].status)
            {
                wind_printf("[step%-2d] %-48s\r\n",g_status_handTB[i].status+1,
                            g_status_handTB[i].stepname);
                err = g_status_handTB[i].handle();
                wind_notice("%-48s [%s]\r\n",g_status_handTB[i].stepname,err == W_ERR_OK?"OK":"ERROR");
                break;
            }
        }
        if(i >= sizeof(g_status_handTB)/sizeof(boot_step_s))
        {
            wind_error("unkown status %d.",g_boot_status);
            boot_status_set(BOOT_ERROR);
        }
    }
}


w_int32_t main(void)
{
    boot_loop();
    return 0;
}

#ifdef __cplusplus
}
#endif
