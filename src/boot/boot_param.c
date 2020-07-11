/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_param.c
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
#include "wind_config.h" 
#include "boot_param.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "boot_hw_if.h"
#include "boot_part.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif
#define BT_BUF_SIZE 1024

static boot_param_s g_bootparam;

static w_int32_t calc_param_lenth(void)
{
    return sizeof(boot_param_s)+PART_COUNT*sizeof(w_part_s);
}

static void upate_bootparam_crc(w_uint8_t *buff)
{
    w_uint32_t *crc;
    w_int32_t index = calc_param_lenth();
    crc = (w_uint32_t*)&buff[index];
    *crc = wind_crc32(buff,sizeof(boot_param_s),0xffffffff);
}

static w_err_t do_read_boot_param(w_part_s *part)
{
    w_int32_t i,len;
    w_uint8_t *buff;
    w_err_t err = W_ERR_OK;
    buff = get_common_buffer();
    for(i = 0;i < 3;i ++)
    {
        len = boot_part_read(part,0,buff,calc_param_lenth()+4,W_TRUE);
        WIND_CHECK_BREAK(len < sizeof(boot_param_s),W_ERR_OK);
    }
    WIND_ASSERT_RETURN(len >= sizeof(boot_param_s),W_ERR_FAIL);
    err = boot_param_check(buff);
    WIND_ASSERT_RETURN(W_ERR_OK == err,W_ERR_FAIL);

    wind_memcpy(&g_bootparam,buff,sizeof(boot_param_s));
    boot_part_update_var((w_part_s*)&buff[sizeof(boot_param_s)]);
    boot_part_reset_ram();
    return W_ERR_OK;
}

void  boot_param_init(void)
{
    wind_memset(&g_bootparam,0,sizeof(boot_param_s));
}


boot_param_s *boot_param_get(void)
{
    if(g_bootparam.magic == BOOT_PARAM_MAGIC)
        return &g_bootparam;
    return W_NULL;
}

boot_param_s *boot_param_from_rom(void)
{
    w_err_t err;
    err = boot_param_read();
    if(0 != err)
    {
        wind_error("get boot params failed.");
        return (boot_param_s *)W_NULL;
    }
    return &g_bootparam;
}


w_err_t boot_param_reset(void)
{   
    boot_param_s *bp = (boot_param_s*)&g_bootparam;
    wind_notice("reset boot param.");
    WIND_ASSERT_RETURN(wind_strlen(ARCH_NAME) < ARCH_NAME_LEN,W_ERR_INVALID);
    WIND_ASSERT_RETURN(wind_strlen(CPU_NAME) < CPU_NAME_LEN,W_ERR_INVALID);
    WIND_ASSERT_RETURN(wind_strlen(BOARD_NAME) < BOARD_NAME_LEN,W_ERR_INVALID);
    wind_memset(&g_bootparam,0,BT_BUF_SIZE);
    bp->magic = BOOT_PARAM_MAGIC;
    bp->lenth = calc_param_lenth();

    bp->version = BOOT_VERSION;
    bp->debug_mode = 0;
    bp->wait_sec = WAIT_KEY_SEC;
    bp->run_type = RUN_SPACE_TYPE;
    bp->encrypt_type = ENCRYPT_TYPE;
    bp->lock_en = MCU_LOCK_ENABLE;
    bp->part_cnt = PART_COUNT;
    wind_strcpy(bp->arch_name,ARCH_NAME);
    wind_strcpy(bp->cpu_name,CPU_NAME);
    wind_strcpy(bp->board_name,BOARD_NAME);
    wind_strcpy(bp->run_part,PART_SYSRUN);
    boot_part_init();
    bp->part = boot_part_get_list();
    return W_ERR_OK;
}


//Check whether the parameter is valid, valid return 1, invalid return 0
w_err_t boot_param_check(w_uint8_t *buff)
{
    w_int32_t index;
    w_uint32_t *crc;
    boot_param_s *bp = (boot_param_s *)buff;
    index = calc_param_lenth();
    crc = (w_uint32_t*)&buff[index];
    if(bp->magic != BOOT_PARAM_MAGIC)
    {
        wind_warn("param block is invalid.");
        return W_ERR_FAIL;
    }
    if(bp->lenth != calc_param_lenth())
    {
        wind_warn("param block lenth is invalid.");
        return W_ERR_FAIL;
    }
    if(*crc != wind_crc32((w_uint8_t*)bp,sizeof(boot_param_s),0xffffffff))
    {
        wind_warn("param block crc is invalid.");
        return W_ERR_CRC;
    }
    if(bp->version > BOOT_VERSION)
    {
        wind_warn("param block version is not matched.");
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}



w_err_t boot_param_read(void)
{
    w_err_t err = W_ERR_OK;
    w_int32_t i;
    w_part_s *part[2];
    
    part[0] = boot_part_get(PART_PARAM1);
    part[1] = boot_part_get(PART_PARAM2);
    
    for(i = 0;i < 2;i ++)
    {
        err = do_read_boot_param(part[1]);
        if(W_ERR_OK == err)
            break;
    }
    WIND_ASSERT_RETURN(i < 2,W_ERR_FAIL);
    return W_ERR_OK;
}

w_err_t boot_param_flush(void)
{
    w_int32_t i,j,len,err = 0;
    w_part_s *part[2],*pt;
    w_uint8_t *buff;
    part[0] = boot_part_get(PART_PARAM1);
    part[1] = boot_part_get(PART_PARAM2);
    buff = get_common_buffer();
    wind_memset(buff,0,COMMBUF_SIZE);
    wind_memcpy(buff,&g_bootparam,sizeof(boot_param_s));
    
    part[0]->datalen = calc_param_lenth()+4;
    part[1]->datalen = calc_param_lenth()+4;
    pt = boot_part_get_list();
    wind_memcpy(&buff[sizeof(boot_param_s)],(void*)pt,PART_COUNT*sizeof(w_part_s));
    
    upate_bootparam_crc(buff);
    for(i = 0;i < 2;i ++)
    {
        for(j = 0;j < 3;j ++)
        {
            len = boot_part_write(part[i],0,buff,calc_param_lenth()+4);
            if(len >=  sizeof(boot_param_s))
                break;
        }
        if(j >= 3)
        {
            
            wind_warn("flush param %d fail.",i + 1);
            err ++;
        }
        wind_debug("flush param %d OK.",i + 1);
    }
    if(err >= 2)
    {
        wind_warn("flush both params failed.");
        part[0]->datalen = 0;
        part[1]->datalen = 0;
        return W_ERR_FAIL;
    }
    wind_notice("flush boot param OK.");
    return W_ERR_OK;
}


#ifdef __cplusplus
}
#endif

