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
static w_uint8_t buffer[MEM_SEC_SIZE];

w_err_t   erase_virtual_disk(w_blkdev_s *dev,w_int32_t blkcnt)
{
    w_int32_t i;
    w_int32_t start;
    //w_int32_t size;
    FILE *file;
    //errno_t errno;
    
    start = 0;
    //size = blkcnt * dev->blksize;
    file = fopen(FILE_NAME,"wb+");
    WIND_ASSERT_RETURN(file  != W_NULL,W_ERR_FAIL);
    WIND_ASSERT_RETURN(file != W_NULL,W_ERR_FAIL);

    wind_memset(buffer,0,sizeof(buffer));
    fseek(file,start,SEEK_SET); 
    for(i = 0;i < blkcnt;i ++)
        fwrite(buffer,1,sizeof(buffer),file);
    fclose(file);
    return W_ERR_OK;
}

w_err_t   make_virtual_disk(w_blkdev_s *dev,w_int32_t blkcnt)
{
    //w_int32_t start;
    //w_int32_t size;
    FILE *file;
    //errno_t errno;
    //start = 0;
    //size = blkcnt * dev->blksize;

    file = fopen(FILE_NAME,"r");
    //WIND_ASSERT_RETURN(file  != W_NULL,W_ERR_FAIL);
    //WIND_ASSERT_RETURN(file != W_NULL,W_ERR_FAIL);
    if(file != W_NULL)
    {
        fclose(file);
        return W_ERR_OK;
    }
    return erase_virtual_disk(dev,blkcnt);
}

w_err_t   disk_init(w_blkdev_s *dev)
{
    return make_virtual_disk(dev,MEM_SEC_COUNT);
}

w_err_t   disk_open(w_blkdev_s *dev)
{
    FILE *file;
    //errno_t errno;
    file = fopen(FILE_NAME,"rb+");
    WIND_ASSERT_RETURN(file  != W_NULL,W_ERR_FAIL);
    //WIND_ASSERT_RETURN(file != W_NULL,W_ERR_FAIL);
    dev->user_arg = file;
    return W_ERR_OK;
}

w_err_t   disk_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    return erase_virtual_disk(dev,MEM_SEC_COUNT);
}

w_err_t   disk_eraseall(w_blkdev_s *dev)
{
    return disk_erase(dev,0,MEM_SEC_COUNT);
}


w_int32_t disk_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t start;
    w_int32_t size,len;
    FILE *file;
    wind_debug("read addr 0x%08x,cnt %d",addr,blkcnt);
    start = (w_int32_t)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    file = dev->user_arg;
    WIND_ASSERT_RETURN(file != W_NULL,0);
    wind_memset(buffer,0,sizeof(buffer));
    fseek(file,start,SEEK_SET); 
    len = fread(buf,1,size,file);
    if(len > 0)
        return len / dev->blksize;
    return -1;
}

w_int32_t disk_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t start;
    w_int32_t size,len;
    FILE *file;
    wind_debug("write addr 0x%08x,cnt %d",addr,blkcnt);
    start = (w_int32_t)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    
    file = dev->user_arg;
    WIND_ASSERT_RETURN(file != W_NULL,0);
    wind_memset(buffer,0,sizeof(buffer));
    fseek(file,start,SEEK_SET); 
    wind_debug("write offset : 0x%08x",start);
    len = fwrite(buf,1,size,file);
    if(len > 0)
        return len / dev->blksize;
    return -1;
}

w_err_t   disk_close(w_blkdev_s *dev)
{
    FILE *file;
    file = dev->user_arg;
    WIND_ASSERT_RETURN(file != W_NULL, W_ERR_FAIL);
    fclose(file);
    dev->user_arg = W_NULL;
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

w_blkdev_s disk_dev[4] = 
{
    WIND_BLKDEV_DEF("disk0",BLKDEV_ROM,0,0,    4096,MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("disk1",BLKDEV_ROM,1,4096, 4096,MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("disk2",BLKDEV_ROM,2,8192, 4096,MEM_SEC_SIZE,&disk_ops),
    WIND_BLKDEV_DEF("disktest",BLKDEV_ROM,3,12288,8192,MEM_SEC_SIZE,&disk_ops)
};

#endif

