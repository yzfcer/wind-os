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
#include "boot_menu.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "boot_param.h"
#include "wind_debug.h"

#include "boot_share_param.h"
#include "boot_img.h"
#include "boot_test.h"
#include "boot_hw_if.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif

static char get_key(w_int32_t index)
{
    WIND_ASSERT_RETURN(index > 0,0);
    if(index < 10)
        return (char)(index + 0x30);
    else 
        return (char)(index - 10 + 0x60);
}

static w_int32_t get_index(char key)
{
    if(key > 0x30 && key < 0x3a)
        return (char)(key - 0x30);
    else if(key > 0x60 && key < 0x7a)
        return (char)(key - 0x60);
    return -1;
}

static w_int32_t is_string_equal(char *str1,char *str2,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(str1[i] != str2[i])
            return 0;
    }
    return 1;
}
//退出菜单标记，如果只为有效，回到菜单时将退出菜单
static w_uint8_t exit_menu_flag = 0;
//高级权限标志，拥有高级权限可以使用更多功能
static w_int32_t super_prio_flag = 0;
//退出菜单的的运行方向，如果不为0，则向下执行，否则重新初始化
static w_int32_t g_go_ahead = 0;
static w_err_t exit_menu(void);
static w_bool_t make_sure_input(char *info)
{
    char ch;
    while(1)
    {
        wind_printf("%s?[y/n]\r\n",info);
        if(W_ERR_OK != read_char_blocking(&ch))
        {
            exit_menu();
            return W_FALSE;
        }
        if('y' == ch)
            return W_TRUE;
        else if('n' == ch)
            return W_FALSE;
    }
}

w_int32_t get_menu_go_direction(void)
{
    return g_go_ahead;
}




static w_err_t download_to_img_part(void)
{
    w_err_t err;
    err = boot_img_download();
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    return boot_img_flush_cache();
}

static w_err_t download_to_fs_part(void)
{
    w_err_t err;
    w_part_s *part = boot_part_get(PART_FS);
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NOT_SUPPORT);
    err = boot_img_download();
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    return boot_img_flush_cache_to_part(&part,1);
}

static w_err_t download_to_sysrun(void)
{
    w_err_t err;
    w_part_s *part = boot_part_get(PART_SYSRUN);
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NOT_SUPPORT);
    err = boot_img_download();
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    err = boot_img_flush_cache_to_part(&part,1);
    if(err == W_ERR_OK)
    {
        g_go_ahead = 1;
        exit_menu_flag = 1;
    }
        
    return err;
}


static w_err_t download_to_any_part(void)
{
    char ch;
    w_int32_t i,index = 1;
    w_err_t err;
    w_part_s *part;
    part = boot_part_get_list();
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_FAIL);
    
    wind_printf("choose part to download:\r\n");
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(part[i].used)
            wind_printf("[%c] %s\r\n",get_key(i+1),part[i].name);
    }
    err = wait_for_key_input(20,&ch,0);
    if(err != 0)
    {
        wind_notice("wait for input timeout.\r\n");
        return W_ERR_TIMEOUT;
    }
    
    index = get_index(ch);
    if(index <= 0)
        return W_ERR_FAIL;
    wind_printf("now download to part:%s\r\n",part[index-1].name);
    part = boot_part_get(part[index-1].name);
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NOT_SUPPORT);
    err = boot_img_download();
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    return boot_img_flush_cache_to_part(&part,1);
}

static w_err_t set_debug_mode(void)
{
    w_int32_t i;
    char ch;
    char *mode[] = 
    {
        "Normal",
        "Debug",
    };
    boot_param_s *bp = (boot_param_s*)boot_param_get();
    while(1)
    {
        wind_printf("set debug mode options:\r\n");
        for(i = 0;i < sizeof(mode)/sizeof(char*);i ++)
        {
            wind_printf("[%d] %s\r\n",i+1,mode[i]);
        }
        if(W_ERR_OK != read_char_blocking(&ch))
        {
            exit_menu();
            return W_ERR_FAIL;
        }
        if(ch >= '1' && ch <= '0' + sizeof(mode)/sizeof(char*))
        {
            if(ch == '1')
                bp->debug_mode = 0;
            else if(ch == '2')
                bp->debug_mode = 1;
            break;
        }
    }
    return W_ERR_OK;
}


static w_err_t show_media_map(void)
{
    boot_media_print();
    return W_ERR_OK;
}

static w_err_t lock_mcu(void)
{
    if(is_chip_lock())
    {
        wind_notice("MCU has been locked before.");
        return W_ERR_FAIL;
    }
    set_chip_lock(1);
    exit_menu();
    return W_ERR_OK;
}

static w_err_t unlock_mcu(void)
{
    if(!is_chip_lock())
    {
        wind_notice("MCU has NOT been locked before.");
        return W_ERR_FAIL;
    }
    set_chip_lock(0);
    exit_menu();
    return W_ERR_OK;
}

static w_err_t show_part_status(void)
{
    boot_part_print();
    return W_ERR_OK;
}

#if BOOT_TEST_ENABLE
w_err_t bootloader_test(void)
{
    if(0 != test_entry())
    {
        exit_menu();
    }
    return W_ERR_OK;
}
#endif

static void do_clear_flash_data(w_uint8_t unlock)
{
    boot_img_clear_all();
    boot_param_reset();
    boot_param_flush();
    if(unlock)
        set_chip_lock(0);
}

static w_err_t clear_boot_param(void)
{
    if(make_sure_input("Are you sure to clear params"))
        do_clear_flash_data(0);
    wind_printf("clear boot param complete.\r\n");
    return W_ERR_OK;
}

static w_err_t exit_and_save(void)
{
    w_err_t err;
    
    err = boot_param_flush();
    exit_menu();
    return err;
}


static w_menu_tb_s g_menu_list[] = 
{
    {'1',0,0,"download img file",download_to_img_part},
    {'2',0,0,"download file system",download_to_fs_part},
    {'3',0,0,"download to sysrun",download_to_sysrun},
    {'4',0,0,"download to any part",download_to_any_part},
    {'5',0,0,"show media map",show_media_map},
    {'6',0,0,"show part status",show_part_status},
    {'7',2,2,"set debug mode",set_debug_mode},
    {'k',0,0,"lock MCU chip",lock_mcu},
    {'r',2,2,"clear boot params",clear_boot_param},
#if BOOT_TEST_ENABLE
    {'t',0,0,"bootloader test",bootloader_test},
#endif
    {'u',2,2,"unlock MCU chip",unlock_mcu},
    
    {'x',1,1,"exit menu and save",exit_and_save},
    {'q',0,0,"exit menu",exit_menu}
};

static w_err_t exit_menu(void)
{
    w_int32_t i;
    exit_menu_flag = 1;
    if(super_prio_flag)
    {
        for(i = 0;i < sizeof(g_menu_list)/sizeof(w_menu_tb_s);i ++)
        {
            g_menu_list[i].prio = g_menu_list[i].prio_bak;
        }
    }
    return W_ERR_OK;
}

void print32_t_menu_list(void)
{
    w_int32_t i;
    wind_printf("\r\n\r\nmenu list:\r\n");
    for(i = 0;i < sizeof(g_menu_list)/sizeof(w_menu_tb_s);i ++)
    {
        if(!g_menu_list[i].prio)
        {
            wind_printf("[%c] %s\r\n",g_menu_list[i].key,g_menu_list[i].menu_item);
        }
    }
}

//开启超级权限，可以删除程序，修改重要参数等
w_err_t open_super_prio(void)
{
    w_int32_t i;
    w_int32_t len;
    w_int32_t prio = 0;
    char *prio1 = "test";
    char *prio2 = "sudo";
    w_uint8_t *buff = get_common_buffer();
    len = read_line_blockig((char*)buff,COMMBUF_SIZE);
    if(wind_strcmp((char*)buff,prio1) == 0)
    {
        prio = 1;
    }
    else if(wind_strcmp((char*)buff,prio2) == 0)
    {
        prio = 2;
    }
    else
    {
        return W_ERR_FAIL;
    }
    
    for(i = 0;i < sizeof(g_menu_list)/sizeof(w_menu_tb_s);i ++)
    {
        if(g_menu_list[i].prio <= prio)
            g_menu_list[i].prio = 0;
    }
    super_prio_flag = 1;
    return W_ERR_OK;
}

w_err_t run_menu(void)
{
    char ch;
    w_int32_t i;
    w_err_t err;
    
    exit_menu_flag = 0;
    g_go_ahead = 1;
    while(0 == exit_menu_flag)
    {
        print32_t_menu_list();
        if(W_ERR_OK != read_char_blocking(&ch))
        {
            exit_menu();
            if(get_menu_go_direction())
                return W_ERR_OK;
            else
                return W_ERR_FAIL;
        }
        for(i = 0;i < sizeof(g_menu_list)/sizeof(w_menu_tb_s);i ++)
        {
            if(ch == g_menu_list[i].key && 0 == g_menu_list[i].prio)
            {
                g_go_ahead = 0;
                g_menu_list[i].handle();
                break;
            }
        }
        if(i >= sizeof(g_menu_list)/sizeof(w_menu_tb_s))
        {
            if(ch == '0')
            {
                wind_printf("\r\n");
                err = open_super_prio();
                if(0 == err)
                {
                    wind_notice("You have opened advanced function.");
                }
            }
        }
    }
    if(get_menu_go_direction())
        return W_ERR_OK;
    else
        return W_ERR_FAIL;
}    



#ifdef __cplusplus
}
#endif
