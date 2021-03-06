#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_BLKDEV_SUPPORT
#define MEM_SEC_COUNT 10240
#define MEM_SEC_SIZE 512

static w_uint8_t memblk[MEM_SEC_COUNT*MEM_SEC_SIZE];

w_err_t   memblk_init(w_blkdev_s *dev)
{
    dev->blkaddr = (w_addr_t)memblk;
    dev->blkcnt = MEM_SEC_COUNT;
    dev->blksize = MEM_SEC_SIZE;
    return W_ERR_OK;
}

w_err_t   memblk_open(w_blkdev_s *dev)
{
    dev->blkaddr = (w_addr_t)memblk;
    dev->blksize = 64;
    dev->blkcnt = 8;
    return W_ERR_OK;
}

w_err_t   memblk_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return W_ERR_OK;
}

w_err_t   memblk_eraseall(w_blkdev_s *dev)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr * dev->blksize);
    size = dev->blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return W_ERR_OK;
}


w_int32_t memblk_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memcpy(buf,start,size);
    return blkcnt;
}

w_int32_t memblk_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memcpy(start,buf,size);
    return blkcnt;
}

w_err_t   memblk_close(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

const w_blkdev_ops_s memblk_ops = 
{
    memblk_init,
    W_NULL,
    memblk_open,
    memblk_erase,
    memblk_eraseall,
    memblk_read,
    memblk_write,
    memblk_close
};

w_blkdev_s memblk_dev[1] = 
{
    WIND_BLKDEV_DEF("memblk",BLKDEV_RAM,0,0,0,0,&memblk_ops)
};

#endif

