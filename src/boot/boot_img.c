/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    :  
** Author      :      Jason Zhou
** Version     :     1.0
** Date        :        2017/04/08
** Description : 
** Others      :  
** History     :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#include "wind_config.h"
#include "boot_menu.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "boot_param.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "boot_share_param.h"
#include "boot_img.h"
#include "boot_part.h"
#include "boot_hw_if.h"
#include "wind_encrypt.h"
#include "wind_string.h"
#include "wind_conv.h"
#include "wind_imghead.h"
#ifdef __cplusplus
extern "C" {
#endif
w_img_head_s img_head;
static w_encypt_ctx_s ctx;
static w_uint8_t keys[] = ENCRYPT_KEY;



w_part_s * boot_img_get_old_part(void)
{
    w_part_s *part[2];
    part[0] = boot_part_get(PART_IMG1);
    part[1] = boot_part_get(PART_IMG2);
    if(!part[0] && !part[1])
        return (w_part_s *)W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
        {
            part[0]->time_mark ++;
            return part[0];
        }
            
        if(part[1] && (part[1]->status == MEM_NORMAL))
        {
            part[1]->time_mark ++;
            return part[1];
        }
        return (w_part_s *)W_NULL;
    }
    if((part[0]->status != MEM_NORMAL) && (part[1]->status == MEM_NORMAL))
    {
        part[0]->time_mark = part[1]->time_mark + 1;
        return part[0];
        
    }
    if((part[1]->status != MEM_NORMAL) && (part[0]->status == MEM_NORMAL))
    {
        part[1]->time_mark = part[0]->time_mark + 1;
        return part[1];
    }
    if(part[0]->time_mark <= part[1]->time_mark)
    {
        part[0]->time_mark = part[1]->time_mark + 1;
        return part[0];
    }
    else
    {
        part[1]->time_mark = part[0]->time_mark + 1;
        return part[1];
    }
}


w_part_s *boot_img_get_new_normal_img(void)
{
    w_part_s *part[2];
    part[0] = boot_part_get(PART_IMG1);
    part[1] = boot_part_get(PART_IMG2);
    if(!part[0] && !part[1])
        return (w_part_s *)W_NULL;
    if(!(part[0] && part[1]))
    {
        if(part[0] && (part[0]->status == MEM_NORMAL))
            return part[0];
        if(part[1] && (part[1]->status == MEM_NORMAL))
            return part[1];
        return (w_part_s *)W_NULL;
    }
    if((part[0]->status != MEM_NORMAL) && (part[1]->status == MEM_NORMAL))
        return part[1];
    if((part[1]->status != MEM_NORMAL) && (part[0]->status == MEM_NORMAL))
        return part[0];
    if(part[0]->time_mark <= part[1]->time_mark)
    {
        part[0]->time_mark = part[1]->time_mark + 1;
        return part[0];
    }
    else
    {
        part[1]->time_mark = part[0]->time_mark + 1;
        return part[1];
    }
   
}

static w_err_t check_img_hwinfo(w_img_head_s *head)
{
    if(0 != wind_memcmp(head->board_name,BOARD_NAME,wind_strlen(BOARD_NAME)))
    {
        wind_notice("dest board name:%s",BOARD_NAME);
        return W_ERR_INVALID;
    }
    if(0 != wind_memcmp(head->arch_name,ARCH_NAME,wind_strlen(ARCH_NAME)))
    {
        wind_notice("dest arch name:%s",ARCH_NAME);
        return W_ERR_INVALID;
    }
    if(0 != wind_memcmp(head->cpu_name,CPU_NAME,wind_strlen(CPU_NAME)))
    {
        wind_notice("dest cpu model:%s",CPU_NAME);
        return W_ERR_INVALID;
    }
    return W_ERR_OK;    
}

static w_err_t decrypt_img(w_part_s *img)
{
    w_int32_t offset;
    w_int32_t len;
    w_uint32_t fsize;
    w_uint8_t *buff;
    w_img_head_s *head = &img_head;
    if(head->magic != IMG_MAGIC)
        return W_ERR_FAIL;
    if(!ENCRYPT_TYPE)
        return W_ERR_OK;
    //wind_notice("decrypt part:%s",img->name);
    wind_debug("part:%s,data lenth:%d",img->name,img->datalen);
    offset = head->head_len;
    fsize = head->img_len;
    buff = get_common_buffer();
    
    wind_encrypt_init(&ctx,keys,sizeof(keys));
    while(1)
    {
        len = boot_part_read(img,offset,buff,COMMBUF_SIZE,W_FALSE);
        WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
        len = (fsize - offset) > len?len:fsize - offset;
        wind_decrypt(&ctx,buff,len);
        boot_part_write(img,offset - IMG_HEAD_LEN,buff,COMMBUF_SIZE);
        offset += len;
        if(offset >= fsize)
            break;
    }
    img->datalen -= IMG_HEAD_LEN;
    boot_part_calc_crc(img,0,img->datalen,W_TRUE);
    return W_ERR_OK;
}

static w_err_t check_img_file_crc(w_part_s *cache)
{
    //w_int32_t blkcnt;
    w_int32_t size;    
    w_uint8_t *buff;
    w_img_head_s *head;
    w_uint32_t offset;
    w_uint32_t crc = 0xffffffff;

    

    WIND_ASSERT_RETURN(cache != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(cache->datalen > 0,W_ERR_INVALID);
    buff = get_common_buffer();
    //blkcnt = COMMBUF_SIZE / cache->blksize;
    //WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    //size = blkcnt * cache->blksize;
    offset = IMG_HEAD_LEN;
    while(offset < cache->datalen)
    {
        size = boot_part_read(cache,offset,buff,COMMBUF_SIZE,W_FALSE);
        WIND_ASSERT_RETURN(size > 0,W_ERR_FAIL);
        crc = wind_crc32(buff,size,crc);
        offset += size;
    }
    
    head = &img_head;
    wind_debug("bin file crc:0x%x,calc crc 0x%x,size:%d",head->bin_crc,crc,offset);
    return head->bin_crc == crc?W_ERR_OK:W_ERR_FAIL;
}

static w_err_t check_img_valid(w_part_s *cache)
{
    w_img_head_s *head;
    w_int32_t len;
    w_uint8_t *buff;
    w_err_t err;
    boot_feed_watchdog();
    head = &img_head;
    cache = boot_part_get(PART_CACHE);
    buff = get_common_buffer();
    len = boot_part_read(cache,0,buff,COMMBUF_SIZE,W_FALSE);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    wind_memset(head,0,sizeof(w_img_head_s));
    err = wind_img_head_get(head,buff);
    WIND_ASSERT_RETURN(head->magic == IMG_MAGIC,W_ERR_INVALID);

    if(W_ERR_OK != check_img_hwinfo(head))
    {
        wind_warn("hardware is NOT matched.");
        return W_ERR_FAIL;
    }
    
    boot_feed_watchdog();
    err = check_img_file_crc(cache);
    boot_feed_watchdog();
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    wind_notice("img file verify OK.");
    return W_ERR_OK;
}

static w_err_t flush_bin_file(w_part_s **part,w_int32_t count,w_uint8_t encrypt)
{
    w_int32_t i;
    w_err_t err;
    w_part_s *cache;

    cache = boot_part_get(PART_CACHE);
    for(i = 0;i < count;i ++)
    {
        if((part[i]->encrypt && encrypt) ||
            !(part[i]->encrypt || encrypt))
        {
            wind_notice("flush bin file to part:%s",part[i]->name);
            err = boot_part_copy_data(cache,part[i]);
            if(0 != err)
            {
                wind_warn("flush data to %s failed.",cache->name);
                return W_ERR_FAIL;
            }
        }
    }
    boot_param_flush();
    return W_ERR_OK;    
}


w_err_t boot_img_flush_cache_to_part(w_part_s **part,w_int32_t count)
{
    w_err_t err;
    w_part_s *cache;
    w_img_head_s *head = &img_head;
    w_int32_t len;
    w_uint8_t *buff;

    
    cache = boot_part_get(PART_CACHE);
    buff = get_common_buffer();
    len = boot_part_read(cache,0,buff,COMMBUF_SIZE,W_FALSE);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    if(head->magic != IMG_MAGIC)
    {
        wind_memset(head,0,sizeof(w_img_head_s));
        err = wind_img_head_get(head,buff);
        wind_notice("get image file head %s",err == W_ERR_OK?"OK":"ERROR");
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    }
    
    err = flush_bin_file(part,count,1);
    wind_notice("flush encrypt image file %s",err == W_ERR_OK?"OK":"ERROR");
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    
    err = decrypt_img(cache);

    wind_notice("decrypt image file %s",err == W_ERR_OK?"OK":"ERROR");
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    err = flush_bin_file(part,count,0);
    wind_notice("flush decrypt image file %s",err == W_ERR_OK?"OK":"ERROR");
    return err;
}

w_err_t boot_img_download(void)
{
    w_err_t err;
    w_int32_t len;
    w_part_s *cache;
    boot_param_s *bp = (boot_param_s*)boot_param_get();

    if(bp->debug_mode)
    {
        wind_notice("device can NOT download in debug mode ,set it to normal mode first");
        return W_ERR_FAIL;
    }
    wind_memset(&img_head,0,sizeof(img_head));
    cache = boot_part_get(PART_CACHE);
    wind_notice("receive file data,please wait.\r\n");
    boot_part_erase(cache);
    len = boot_receive_img(cache);
    if(len <= 0)
    {
        wind_error("receive cache data failed.");
        return W_ERR_FAIL;
    }
    wind_notice("image file lenth:%d",len);
    cache->datalen = (w_uint32_t)len;
    err = check_img_valid(cache);
    wind_notice("check image file valid %s",err == W_ERR_OK?"OK":"ERROR");
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    boot_part_calc_crc(cache,0,0,W_TRUE);
    //wind_notice("cache file lenth:%d",cache->datalen);
    return W_ERR_OK;
}


w_err_t boot_img_clear_all(void)
{
    w_int32_t idx = 0;
    w_uint32_t i;
    w_part_s *code[5];
    wind_printf("clearing program ...\r\n");
    code[idx++] = boot_part_get(PART_IMG1);
    code[idx++] = boot_part_get(PART_IMG2);
    code[idx++] = boot_part_get(PART_SYSRUN);
    code[idx++] = boot_part_get(PART_PARAM1);
    code[idx++] = boot_part_get(PART_PARAM2);
    
    for(i = 0;i < 5;i ++)
    {
        if(!code[i])
            continue;
        wind_notice("erase base 0x%x,lenth %d.",code[i]->base,code[i]->datalen);
        boot_part_erase(code[i]);
    }
    wind_printf("clear program OK.\r\n");
    return W_ERR_OK;
}


//-------------------------------------------------------------------------
static w_err_t repair_program(void)
{
    w_err_t err;
    w_part_s *src,*dest;
    dest = boot_part_get(PART_SYSRUN);
    if(MEM_ERROR != dest->status)
        return W_ERR_OK;
    src = boot_img_get_new_normal_img();
    WIND_ASSERT_RETURN(src != W_NULL,W_ERR_FAIL);
    wind_notice("repair part:%s from %s",dest->name,src->name);
    err = boot_part_copy_data(src,dest);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    
    (void)boot_param_flush();
    return err;
}

w_err_t boot_img_check(void)
{
    w_err_t err = 0;
    w_part_s *code[3];
    w_int32_t error_flag = 0,i;
    w_bool_t is_ok;
    
    code[0] = boot_part_get(PART_IMG1);
    code[1] = boot_part_get(PART_IMG2);

    code[2] = boot_part_get(PART_SYSRUN);
    if(code[2]->mtype == MEDIA_TYPE_RAM)
        code[2] = W_NULL;
    for(i = 0;i < sizeof(code)/sizeof(w_part_s*);i ++)
    {
        if(!code[i])
            continue;
        is_ok = boot_part_check(code[i]);
        if(!is_ok)
        {
            error_flag = 1;
            code[i]->status = MEM_ERROR;
        }
        wind_notice("part %s status:%s",code[i]->name,is_ok?"OK":"ERROR");
    }

    if(error_flag)
    {
        (void)boot_param_flush();
        err = repair_program();
        
        if(0 != err)
        {
            wind_error("repairing program failed");
            return W_ERR_FAIL;
        }
        return err;
    }
    return err;
}

#ifdef __cplusplus
}
#endif
