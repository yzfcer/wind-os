#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_BLK_DRVFRAME_SUPPORT
static w_uint8_t memblk[8*64];

w_err_t   memblk_open(blkdev_s *dev)
{
    dev->blkaddr = (w_addr_t)memblk;
    dev->blksize = 64;
    dev->blkcnt = 8;
    return ERR_OK;
}

w_err_t   memblk_erase(blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return ERR_OK;
}

w_err_t   memblk_eraseall(blkdev_s *dev)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr * dev->blksize);
    size = dev->blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return ERR_OK;
}


w_int32_t memblk_read(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = dev->blkcnt * dev->blksize;
    wind_memcpy(buf,start,size);
    return blkcnt;
}

w_int32_t memblk_write(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = dev->blkcnt * dev->blksize;
    wind_memcpy(start,buf,size);
    return blkcnt;
}

w_err_t   memblk_close(blkdev_s *dev)
{
    return ERR_OK;
}

const blkdev_ops_s memblk_ops = 
{
    memblk_open,
    memblk_erase,
    memblk_eraseall,
    memblk_read,
    memblk_write,
    memblk_close
};

blkdev_s memblk_dev = 
{
    WIND_BLKDEV_MAGIC,
    {NULL,NULL},
    "memblk",
    B_FALSE,
    (w_addr_t)memblk,8,64,
    NULL,
    &memblk_ops
};

#endif

