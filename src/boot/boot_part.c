/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_part.c
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

static char *get_mtype_name(w_uint8_t mtype)
{
    if(mtype == BLKDEV_RAM)
        return "RAM";
    else
        return "ROM";
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
    wind_notice("boot part init");
    wind_memset(g_part,0,sizeof(g_part));
    err = boot_parts_create();
    return W_ERR_OK;
}

static void part_info_init(w_part_s *part)
{
    part->used = 1;
    part->status = MEM_NULL;
    //part->time_mark = 0;
    part->datalen = 0;
    part->crc = 0;
}

w_bool_t  boot_part_create(const char *name,w_uint8_t encrypt)
{
    w_part_s *part;
    w_blkdev_s *blkdev;
    wind_notice("create part: %s",name);
    WIND_ASSERT_RETURN(name != W_NULL,W_FALSE);
    WIND_ASSERT_RETURN(wind_strlen(name) < PART_NAME_LEN,W_FALSE);
    blkdev = wind_blkdev_get(name);
    WIND_ASSERT_RETURN(blkdev != W_NULL,W_FALSE);
    part = get_null_part();
    WIND_ASSERT_RETURN(part != W_NULL,W_FALSE);

    wind_memcpy(&part->blkdev,blkdev,sizeof(w_blkdev_s));
    wind_strcpy(part->name,blkdev->obj.name);
    part->mtype = (w_uint8_t)blkdev->devtype;
    part->used = 1;
    part->encrypt = encrypt?1:0;
    part->status = MEM_NULL;
    part->time_mark = 0;
    part->size = blkdev->blkcnt * blkdev->blksize;
    part->datalen = 0;
    part->crc = 0;
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


w_err_t boot_part_calc_crc(w_part_s *part,w_int32_t offset,w_int32_t len,w_bool_t set)
{
    w_int32_t blkcnt;
    w_int32_t size;    
    w_uint8_t *buff;
    w_uint32_t crc = 0xffffffff;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(part->datalen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(offset >= 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN((offset & (part->blkdev.blksize - 1)) == 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(len >= 0,W_ERR_INVALID);
    if(len == 0)
        len = part->datalen;
    
    buff = get_common_buffer();
    blkcnt = COMMBUF_SIZE / part->blkdev.blksize;
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    size = blkcnt * part->blkdev.blksize;
    while(offset < part->datalen)
    {
        size = boot_part_read(part,offset,buff,COMMBUF_SIZE,W_FALSE);
        WIND_ASSERT_RETURN(size > 0,W_ERR_FAIL);
        crc = wind_crc32(buff,size,crc);
        offset += size;
    }
    if(set)
    {
        part->crc = crc;
        return W_ERR_OK;
    }
    wind_debug("part %s crc:0x%x,calc crc 0x%x",part->name,part->crc,crc);
    wind_notice("part %s calc crc %s",part->name,part->crc == crc?"OK":"ERROR");
    return part->crc == crc?W_ERR_OK:W_ERR_FAIL;
}



w_int32_t boot_part_read(w_part_s *part,w_int32_t offset,w_uint8_t *data,w_uint32_t datalen,w_bool_t read_space)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_err_t err;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NULL_PTR);
    blkdev = &part->blkdev;
    WIND_ASSERT_RETURN(offset < part->size,W_ERR_INVALID);
    if(!read_space)
        WIND_ASSERT_RETURN(offset < part->datalen,W_ERR_INVALID);
    else
        WIND_ASSERT_RETURN(offset < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(datalen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(0 == (offset & (blkdev->blksize-1)),W_ERR_INVALID);
    size = datalen;
    if(size + offset > part->size)
        size = part->datalen - offset;
    blkcnt = (size + blkdev->blksize - 1)/ blkdev->blksize;
    if(!IS_F_BLKDEV_OPEN(blkdev))
    {
        err = wind_blkdev_open(blkdev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    }
    blkcnt = wind_blkdev_read(blkdev,offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    if(!read_space)
        return offset + size > part->datalen?part->datalen - offset:size;
    return size;
}

w_int32_t boot_part_write(w_part_s *part,w_int32_t offset,w_uint8_t *data,w_uint32_t datalen)
{
    w_uint32_t blkcnt;
    w_uint32_t size;
    w_err_t err;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(part != W_NULL,W_ERR_NULL_PTR);
    blkdev = &part->blkdev;
    WIND_ASSERT_RETURN(offset <= part->datalen,W_ERR_INVALID);
    WIND_ASSERT_RETURN(data != W_NULL,W_ERR_NULL_PTR);
    WIND_ASSERT_RETURN(datalen > 0,W_ERR_INVALID);
    WIND_ASSERT_RETURN(datalen < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(datalen + offset < part->size,W_ERR_INVALID);
    WIND_ASSERT_RETURN(0 == (offset & (blkdev->blksize-1)),W_ERR_INVALID);
    size = datalen;
    blkcnt = (size + blkdev->blksize - 1)/ blkdev->blksize;
    if(!IS_F_BLKDEV_OPEN(blkdev))
    {
        err = wind_blkdev_open(blkdev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    }
    blkcnt = wind_blkdev_write(blkdev,offset,data,blkcnt);
    WIND_ASSERT_RETURN(blkcnt > 0,W_ERR_FAIL);
    if(offset + size >= part->datalen)
        part->datalen = offset + size;
    return size;
}

w_err_t boot_part_erase(w_part_s *part)
{
    w_err_t err;
    w_blkdev_s *blkdev;
    WIND_ASSERT_RETURN(part != NULL,W_ERR_NULL_PTR);
    blkdev = &part->blkdev;    
    wind_notice("erase part:%s",wind_obj_name(&blkdev->obj));
    if(!IS_F_BLKDEV_OPEN(blkdev))
    {
        err = wind_blkdev_open(blkdev);
        WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    }
    err = wind_blkdev_eraseall(blkdev);
    WIND_ASSERT_RETURN(err == W_ERR_OK,W_ERR_FAIL);
    part_info_init(part);
    return W_ERR_OK;
}

void boot_part_reset_ram(void)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(g_part[i].used && (g_part[i].mtype == BLKDEV_RAM))
        {
            g_part[i].datalen = 0;
            g_part[i].crc = 0;
        }
    }
}


void boot_part_print(void)
{
    w_int32_t i;
    w_blkdev_s *blkdev;
    w_int32_t count = boot_part_get_count();
    w_part_s *part = boot_part_get_list();
    wind_printf("system part infomation:\r\n");
    wind_print_space(9);
    wind_printf("%-12s %-8s %-10s %-10s %-10s %-8s\r\n","part","type","addr","size","datalen","usage");
    wind_print_space(9);
    for(i = 0;i < count;i ++)
    {
        blkdev = &part[i].blkdev;
        wind_printf("%-12s %-8s 0x%-8x 0x%-8x 0x%-8x %4d%%\r\n",wind_obj_name(&blkdev->obj),get_mtype_name(part[i].mtype),blkdev->blkaddr,blkdev->blksize,\
                part[i].datalen,blkdev->blkcnt?(part[i].datalen*100)/(blkdev->blkcnt * blkdev->blksize):0);
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

w_err_t boot_part_update_rom(w_part_s *ptlist)
{
    w_int32_t i;
    for(i = 0;i < PART_COUNT;i ++)
    {
        if(!g_part[i].used)
            break;
        g_part[i].datalen = ptlist[i].datalen;
        g_part[i].crc = ptlist[i].crc;
        g_part[i].time_mark = ptlist[i].time_mark;
    }
    return W_ERR_OK;
}

static void print_copy_percents(w_int32_t numerator, w_int32_t denominator,w_int32_t del)
{
    #define STATGE_LEN 100
    static char stage[STATGE_LEN+3];
    w_int32_t i,cnt;
    w_int32_t persent;
    WIND_ASSERT_RETURN_VOID(denominator > 0);
    persent = numerator*100/denominator;
    if(PRINT_LEVEL <= PRINT_LV_DEBUG)
        return;
    cnt = persent * STATGE_LEN / 100;
    wind_memset(stage,' ',STATGE_LEN+3);
    stage[0] = '[';
    stage[STATGE_LEN+1] = ']';
    stage[STATGE_LEN+2] = 0;
    for(i = 0;i < cnt;i ++)
        stage[i+1] = '=';
    if(cnt < STATGE_LEN)
        stage[cnt] = '>';
    if(del)
        wind_printf("%c%c%c%c",8,8,8,8);
    wind_printf("\r%s",stage);
    wind_printf(" %3d%%",persent);
    boot_feed_watchdog();
}

w_err_t boot_part_copy_data(w_part_s *src,w_part_s *dest)
{
    w_int32_t i,len1,len2,blocks,times,offset;
    w_uint8_t *buff = get_common_buffer();

    if(0 >= src->datalen)
        return W_ERR_OK;
    if(dest->size < src->datalen)
    {
        wind_warn("space is NOT enough.");
        return W_ERR_FAIL;
    }
    boot_part_erase(dest);
    wind_notice("copy data from \"%s\" to \"%s\" lenth %d.",
                src->name,dest->name,src->datalen);
    wind_debug("source part %s,addr 0x%x,lenth %d dest part,%s,addr 0x%x,lenth %d.",
                src->name,src->blkdev.blkaddr,src->datalen,
                src->name,dest->blkdev.blkaddr,dest->size);
    offset = 0;
    blocks = (src->datalen + COMMBUF_SIZE - 1) / COMMBUF_SIZE;
    //wind_printf("complete:\r\n");
    print_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            wind_memset(buff,0,COMMBUF_SIZE);
			len1 = boot_part_read(src,offset,buff,COMMBUF_SIZE,W_FALSE);
            if(len1 > 0)
                break;            
        }
        if(times >= 3)
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return W_ERR_FAIL;
        }

        for(times = 0;times < 3;times ++)
        {
            len2 = boot_part_write(dest,offset,buff,len1);
            if(len2 > 0)
                break;
        }
        if((times >= 3)||(len1 != len2))
        {
            wind_warn("read block offset 0x%x,lenth %d failed.",offset,COMMBUF_SIZE);
            dest->status = MEM_ERROR;
            return W_ERR_FAIL;
        }
        offset += len2;
        print_copy_percents(i,blocks,1);
        boot_feed_watchdog();
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
    if(src->blkdev.devtype != dest->blkdev.devtype)
        return W_FALSE;
    if(wind_strcmp(src->blkdev.obj.name,dest->blkdev.obj.name))
        return W_FALSE;
    if(src->blkdev.blkaddr != dest->blkdev.blkaddr)
        return W_FALSE;
    if(src->blkdev.blkcnt != dest->blkdev.blkcnt)
        return W_FALSE;
    if(src->blkdev.blksize != dest->blkdev.blksize)
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

