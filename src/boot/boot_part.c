
#include "wind_config.h"
#include "boot_hw_if.h"
#include "boot_part.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "boot_param.h"
static w_part_s g_part[PART_COUNT];
static w_uint8_t commbuffer[COMMBUF_SIZE];

w_uint8_t *get_common_buffer(void)
{
    return commbuffer;
}

static char *get_mtype_name(w_media_type_e mtype)
{
    char *name[2] = {"RAM","ROM"};
    return name[mtype];
}

static w_part_s *get_null_part(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(!g_part[i].used)
            return &g_part[i];
    }
    return NULL;
}

w_err_t boot_part_init(void)
{
    w_err_t err;
    wind_memset(g_part,0,sizeof(g_part));
    err = boot_parts_create();
    return W_ERR_OK;
}

w_bool_t  boot_part_create(const char *name,w_media_s *media,w_uint32_t size,w_uint8_t encrypt)
{
    w_part_s *part;
    wind_notice("create part: %s",name);
    WIND_ASSERT_RETURN(name != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(wind_strlen(name) < PART_NAME_LEN,W_FALSE);
    WIND_ASSERT_RETURN(media != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(size > media->blksize,W_FALSE);
    WIND_ASSERT_RETURN(size < media->size,W_FALSE);
    WIND_ASSERT_RETURN(size + media->offset <= media->size,W_FALSE);
    part = get_null_part();
    WIND_ASSERT_RETURN(part != W_NULL,W_FALSE);
    wind_strcpy(part->name,name);
    wind_strcpy(part->media_name,media->name);
    part->mtype = media->mtype;
    part->used = 1;
    part->encrypt = encrypt?1:0;
    part->status = MEM_NULL;
    part->time_mark = 0;
    part->base = media->offset;
    part->size = size;
    part->blksize = media->blksize;
    part->datalen = 0;
    part->offset = 0;
    part->crc = 0;
    media->offset += size;
    return W_TRUE;
}


w_part_s *boot_part_get(const char *name)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(g_part[i].used && (wind_strcmp(name,g_part[i].name) == 0))
            return &g_part[i];
    }
    return NULL;
}

w_err_t boot_part_seek(w_part_s *part,w_int32_t offset)
{
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(offset < part->size,W_ERR_INVALID);
    part->offset = offset;
    return W_ERR_OK;
}

w_err_t boot_part_calc_crc(w_part_s *part,w_int32_t offset,w_int32_t len,w_bool_t set)
{
    w_int32_t blkcnt;
    w_int32_t size;    
    w_uint8_t *buff;
    w_uint32_t crc = 0xffffffff;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN((offset & (part->blksize-1)) == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(len >= 0,W_ERR_INVALID);
    if(len == 0)
        len = part->datalen;
    
    buff = get_common_buffer();
    blkcnt = COMMBUF_SIZE / part->blksize;
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    size = blkcnt * part->blksize;
    boot_part_seek(part,offset);
    while(part->offset < part->datalen)
    {
        size = boot_part_read(part,buff,COMMBUF_SIZE);
        WIND_ASSERT_RETURN(size > 0,W_ERR_FAIL);
        crc = wind_crc32(buff,size,crc);
    }
    if(set)
    {
        part->crc = crc;
        return W_ERR_OK;
    }
    return part->crc == crc?W_ERR_OK:W_ERR_FAIL;
}



w_int32_t boot_part_read(w_part_s *part,w_uint8_t *data,w_uint32_t datalen)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(part->offset < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(datalen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(0 == (part->offset&(part->blksize-1)),W_ERR_INVALID);
    size = datalen;
    if(size + part->offset > part->size)
        size = part->datalen - part->offset;
    media = boot_media_get(part->media_name);
    blkcnt = (size + part->blksize - 1)/ part->blksize;
    blkcnt = media->ops->read_blk(media,part->base + part->offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    part->offset += size;
    return size;
}

w_int32_t boot_part_write(w_part_s *part,w_uint8_t *data,w_uint32_t datalen)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(part->offset < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(datalen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(datalen < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(datalen + part->offset < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(0 == (part->offset&(part->blksize-1)),W_ERR_INVALID);
    size = datalen;
    media = boot_media_get(part->media_name);
    blkcnt = (size + part->blksize - 1)/ part->blksize;
    blkcnt = media->ops->write_blk(media,part->base + part->offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    
    part->offset += size;
    part->datalen = part->offset;
    return size;
}

w_err_t boot_part_erase(w_part_s *part)
{
    w_uint32_t blkcnt;
    w_media_s *media;
    WIND_ASSERT_RETURN(part != NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(part->size > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(part->blksize > 0,W_ERR_INVALID);
    media = boot_media_get(part->media_name);
    blkcnt = part->size / part->blksize;
    blkcnt = media->ops->erase_blk(media,part->base,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    return W_ERR_OK;
}

void boot_part_reset_ram(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(g_part[i].used && (g_part[i].mtype == MEDIA_TYPE_RAM))
        {
            g_part[i].datalen = 0;
            g_part[i].crc = 0;
        }
    }
}


void boot_part_print(void)
{
    w_int32_t i;
    w_int32_t count = boot_part_get_count();
    w_part_s *part = boot_part_get_list();
    wind_printf("system part infomation:\r\n");
    wind_print_space(9);
    wind_printf("%-12s %-8s %-8s %-10s %-10s %-10s %-8s\r\n","part","media","type","addr","size","datalen","usage");
    wind_print_space(9);
    for(i = 0;i < count;i ++)
    {
        wind_printf("%-12s %-8s %-8s 0x%-8x 0x%-8x 0x%-8x %4d%%\r\n",part[i].name,part[i].media_name,get_mtype_name(part[i].mtype),part[i].base,part[i].size,\
                part[i].datalen,part[i].size?(part[i].datalen*100)/part[i].size:0);
    }
    wind_print_space(9);
}



w_int32_t boot_part_get_count(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(!g_part[i].used)
            return i;
    }
    return 0;
}

 
w_part_s *boot_part_get_list(void)
{
    return g_part;
}



static void print_copy_percents(w_int32_t numerator, w_int32_t denominator,w_int32_t del)
{
    if(del)
        wind_printf("%c%c%c%c",8,8,8,8);
    wind_printf("%3d%%",numerator*100/denominator);
    feed_watchdog();
}

w_err_t boot_part_copy_data(w_part_s *src,w_part_s *dest)
{
    w_int32_t i,len,blocks,times;
    w_uint8_t *buff = get_common_buffer();

    if(0 >= src->datalen)
        return W_ERR_OK;
    if(dest->size < src->datalen)
    {
        wind_warn("space is NOT enough.");
        return W_ERR_FAIL;
    }
    wind_notice("copy data from \"%s\" to \"%s\" lenth %d.",
                src->name,dest->name,src->datalen);
    wind_debug("source part %s,addr 0x%x,lenth %d dest part,%s,addr 0x%x,lenth %d.",
                src->name,src->base,src->datalen,
                src->name,dest->base,dest->size);
    boot_part_seek(src,0);
    boot_part_seek(dest,0);
    blocks = (src->datalen + COMMBUF_SIZE - 1) / COMMBUF_SIZE;
    wind_printf("complete:");
    print_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            wind_memset(buff,0,COMMBUF_SIZE);
            len = boot_part_read(src,buff,COMMBUF_SIZE);
            if(len > 0)
                break;            
        }
        if(times >= 3)
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",src->offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return W_ERR_FAIL;
        }

        for(times = 0;times < 3;times ++)
        {
            len = boot_part_write(dest,buff,COMMBUF_SIZE);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",dest->offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return W_ERR_FAIL;
        }
        print_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print_copy_percents(i,blocks,1);
    wind_printf("\r\n");

    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    wind_debug("copy data OK."); 
    return W_ERR_OK;    
}

w_bool_t boot_part_equal(w_part_s *src,w_part_s *dest)
{
    if(src->mtype != dest->mtype)
        return W_FALSE;
    if(wind_strcmp(src->media_name,dest->media_name))
        return W_FALSE;
    if(src->base != dest->base)
        return W_FALSE;
    if(src->size != dest->size)
        return W_FALSE;
    return W_TRUE;
}



w_bool_t boot_part_check(w_part_s *part)
{
    w_err_t err;
    WIND_ASSERT_RETURN(part != W_NULL,W_FALSE);
    if(part->status == MEM_NULL)
        return W_TRUE;
    if(part->status == MEM_ERROR)
        return W_FALSE;
    err = boot_part_calc_crc(part,0,0,W_FALSE);
    if(W_ERR_OK != err)
        return W_FALSE;
    return W_TRUE;
}

