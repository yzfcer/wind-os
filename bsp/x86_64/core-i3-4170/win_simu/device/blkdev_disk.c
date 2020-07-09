/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : Blkdev_disk.c
** Author      : Jason Zhou
** Last Date   : 2018.03.26
** Description : Block device registration file for file virtualization
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2018.03.26  
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2018.03.26
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#include <stdio.h>
#if WIND_BLKDEV_SUPPORT
#define MEM_SEC_COUNT 20480
#define MEM_SEC_SIZE 512
#define FILE_NAME "disk.bin"
FILE *file = (FILE*)W_NULL;

static w_uint8_t buffer[MEM_SEC_SIZE];
static w_mutex_s diskmtx;
static w_int32_t open_cnt = 0;

w_err_t diskmtx_init(void)
{
    w_err_t err = W_ERR_OK;
    if(diskmtx.obj.magic != WIND_MUTEX_MAGIC)
    {
        err = wind_mutex_init(&diskmtx,"diskmtx");
    }
    return err;
}

w_err_t   erase_virtual_disk(w_blkdev_s *dev,w_int32_t blkcnt)
{
    w_int32_t i;
    w_int32_t start;
    w_int32_t size,len;
    errno_t errno;
    WIND_CHECK_RETURN(file == W_NULL,W_ERR_OK);
    
    start = 0;
    size = blkcnt * dev->blksize;
    errno = fopen_s(&file,FILE_NAME,"wb+");
    WIND_ASSERT_RETURN(errno == 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_FAIL);
    wind_memset(buffer,0,sizeof(buffer));
    fseek(file,start,SEEK_SET); 
    for(i = 0;i < blkcnt;i ++)
        len = fwrite(buffer,1,sizeof(buffer),file);
    fclose(file);
    file = (FILE*)W_NULL;
    return W_ERR_OK;
}

w_err_t   make_virtual_disk(w_blkdev_s *dev,w_int32_t blkcnt)
{
    errno_t errno;
    w_int32_t start;
    w_int32_t size;
    //FILE *file;
    WIND_CHECK_RETURN(file == W_NULL,W_ERR_HARDFAULT);
    start = 0;
    size = blkcnt * dev->blksize;

    errno = fopen_s(&file,FILE_NAME,"r");
    if(file != W_NULL)
    {
        fclose(file);
        file = (FILE*)W_NULL;
        return W_ERR_OK;
    }
    return erase_virtual_disk(dev,blkcnt);
}

w_err_t   disk_init(w_blkdev_s *dev)
{
    w_err_t err = W_ERR_OK;
    diskmtx_init();
    wind_mutex_lock(&diskmtx);
    err = make_virtual_disk(dev,MEM_SEC_COUNT);
    wind_mutex_unlock(&diskmtx);
    return err;
}

w_err_t   disk_open(w_blkdev_s *dev)
{
    w_err_t err = W_ERR_OK;
    errno_t errno;
    WIND_CHECK_RETURN(!IS_F_BLKDEV_OPEN(dev),W_ERR_OK);
    wind_mutex_lock(&diskmtx);
    do
    {
        open_cnt ++;
        WIND_CHECK_BREAK(file == W_NULL,W_ERR_OK);
        errno = fopen_s(&file,FILE_NAME,"rb+");
        WIND_ASSERT_BREAK(errno == 0,W_ERR_FAIL,"");
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"");
    }while(0);
    wind_mutex_unlock(&diskmtx);
    return W_ERR_OK;
}

w_err_t   disk_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_err_t err = W_ERR_OK;
    diskmtx_init();
    wind_mutex_lock(&diskmtx);
    err = erase_virtual_disk(dev,MEM_SEC_COUNT);
    wind_mutex_unlock(&diskmtx);
    return err;
}

w_err_t   disk_eraseall(w_blkdev_s *dev)
{
    return disk_erase(dev,0,MEM_SEC_COUNT);
}


w_int32_t disk_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t start;
    w_int32_t size,len;
    WIND_CHECK_RETURN(file != W_NULL,W_ERR_HARDFAULT);
    wind_debug("read addr 0x%08x,cnt %d",addr,blkcnt);
    start = (w_int32_t)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    WIND_ASSERT_RETURN(file != W_NULL,0);
    wind_memset(buffer,0,sizeof(buffer));
    
    wind_mutex_lock(&diskmtx);
    fseek(file,start,SEEK_SET); 
    len = fread(buf,1,size,file);
    wind_mutex_unlock(&diskmtx);
    
    if(len > 0)
        return len / dev->blksize;
    return -1;
}

w_int32_t disk_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t start;
    w_int32_t size,len;
    WIND_CHECK_RETURN(file != W_NULL,W_ERR_HARDFAULT);
    wind_debug("write addr 0x%08x,cnt %d",addr,blkcnt);
    start = (w_int32_t)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    
    WIND_ASSERT_RETURN(file != W_NULL,0);
    wind_memset(buffer,0,sizeof(buffer));
    wind_mutex_lock(&diskmtx);
    fseek(file,start,SEEK_SET); 
    wind_debug("write offset : 0x%08x",start);
    len = fwrite(buf,1,size,file);
    wind_mutex_unlock(&diskmtx);
    if(len > 0)
        return len / dev->blksize;
    return -1;
}

w_err_t   disk_close(w_blkdev_s *dev)
{
    w_err_t err = W_ERR_OK;
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_HARDFAULT);
    WIND_ASSERT_RETURN(file != W_NULL, W_ERR_FAIL);
    wind_mutex_lock(&diskmtx);
    do
    {
        WIND_CHECK_BREAK(open_cnt > 0,W_ERR_OK);
        open_cnt --;
        if(open_cnt <= 0)
        {
            fclose(file);
            file = (FILE*)W_NULL;
        }
    }while(0);
    wind_mutex_unlock(&diskmtx);
    return W_ERR_OK;
}

const w_blkdev_ops_s disk_ops = 
{
    disk_init,
    W_NULL,
    disk_open,
    disk_erase,
    disk_eraseall,
    disk_read,
    disk_write,
    disk_close
};

w_blkdev_s disk_dev[7] = 
{
    WIND_BLKDEV_DEF("boot", BLKDEV_ROM,0,0,    64,  MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("img0",  BLKDEV_ROM,1,64,   4096,MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("img1",  BLKDEV_ROM,2,4160, 4096,MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("param0",BLKDEV_ROM,3,8256, 16,  MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("param1",BLKDEV_ROM,4,8272, 16,  MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("imgprm",BLKDEV_ROM,4,8288, 16,  MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("rootfs",BLKDEV_ROM,4,8304, 8192,MEM_SEC_SIZE,&disk_ops),
};

#endif

