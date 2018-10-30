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
#include "boot_share_param.h"
#include "wind_config.h"
#include "boot_part.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "wind_string.h"

#ifdef __cplusplus
extern "C" {
#endif
 
static share_param_s g_sharedata;

static void update_share_crc(w_uint8_t *buff)
{
    w_uint32_t *crc;
    w_int32_t index = sizeof(share_param_s);
    crc = (w_uint32_t*)&buff[index];
    *crc = wind_crc32(buff,sizeof(share_param_s),0xffffffff);
}



static w_err_t check_share_param(w_uint8_t *buff)
{
    share_param_s *sp = (share_param_s *)buff;
    w_uint32_t *crc = (w_uint32_t*)&buff[sizeof(share_param_s)];
    if(sp->magic != SHARE_PARAM_MAGIC)
    {
        wind_notice("share param block is invalid.");
        return W_ERR_INVALID;
    }
    if(sp->lenth != sizeof(share_param_s))
    {
        wind_warn("share param block lenth is invalid.");
        return W_ERR_INVALID;
    }

    if(*crc != wind_crc32(buff,sizeof(share_param_s),0xffffffff))
    {
        wind_warn("share param block crc is invalid.");
        return W_ERR_INVALID;
    }

    return W_ERR_OK;
}




static share_param_s *get_share_data_from_part(void)
{
    w_err_t err;
    w_int32_t len;
    share_param_s * sp = &g_sharedata;
    w_uint8_t *buff = get_common_buffer();
    
    w_part_s *part = boot_part_get(PART_SHARE);
    WIND_ASSERT_RETURN(part != W_NULL,(share_param_s *)W_NULL);
    WIND_ASSERT_RETURN(COMMBUF_SIZE >= part->size,(share_param_s *)W_NULL);
    boot_part_seek(part,0);
    len = boot_part_read(part,buff,sizeof(share_param_s)+4);
    WIND_ASSERT_RETURN(len > 0,(share_param_s *)W_NULL);
    err = check_share_param(buff);
    if(err == W_ERR_OK)
    {
        wind_memcpy((void*)sp,(void*)buff,sizeof(share_param_s));
        return sp;
    }
    share_param_reset();
    
    return sp;
}

w_err_t share_param_flush(share_param_s *sp)
{
    w_int32_t len;
    w_uint8_t *buff;
    w_part_s *part = boot_part_get(PART_SHARE);
    buff = get_common_buffer();
    wind_notice("flush share param");
    wind_memcpy(buff,sp,sizeof(share_param_s));
    update_share_crc(buff);
    boot_part_seek(part,0);
    len = boot_part_write(part,(w_uint8_t*)sp,sizeof(share_param_s)+4);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    return W_ERR_OK;
}

w_err_t share_param_init(void)
{
    wind_memset((void *)&g_sharedata,0,sizeof(share_param_s));
    return W_ERR_OK;
}

w_err_t share_param_reset(void)
{
    w_part_s *part;
    share_param_s *sp = &g_sharedata;

    wind_memset((void *)sp,0,sizeof(share_param_s));
    sp->magic = SHARE_PARAM_MAGIC;
    sp->lenth = sizeof(share_param_s);
    sp->share_version = SHARE_VERSION;
    sp->upgrade_flag = 0;

    part = boot_part_get_list();
    wind_memcpy((void *)sp->part,part,sizeof(sp->part));
    share_param_flush(sp);
    return W_ERR_OK;
}

share_param_s *share_param_get(void)
{
    share_param_s *sp = &g_sharedata;
    if(sp->magic == SHARE_PARAM_MAGIC)
        return sp;
    sp = get_share_data_from_part();
    return sp;      
}







#ifdef __cplusplus
}
#endif

