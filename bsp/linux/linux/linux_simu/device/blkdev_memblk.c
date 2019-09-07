#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#if WIND_BLKDEV_SUPPORT
#define MEM_SEC_COUNT 10240
#define MEM_SEC_SIZE 512

static w_uint8_t memblk[MEM_SEC_COUNT*MEM_SEC_SIZE];

w_err_t   memblk_init(w_blkdev_s *blkdev)
{
    blkdev->blkaddr = (w_addr_t)0;
    blkdev->blkcnt = MEM_SEC_COUNT;
    blkdev->blksize = MEM_SEC_SIZE;
    return W_ERR_OK;
}

w_err_t   memblk_open(w_blkdev_s *blkdev)
{
    blkdev->blkaddr = (w_addr_t)0;
    blkdev->blksize = MEM_SEC_SIZE;
    blkdev->blkcnt = MEM_SEC_COUNT;
    return W_ERR_OK;
}

w_err_t   memblk_erase(w_blkdev_s *blkdev,w_addr_t addr,w_int32_t blkcnt)
{
    w_uint32_t start;
    w_int32_t size;
    start = (w_uint32_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    wind_memset(&memblk[start],0,size);
    return W_ERR_OK;
}

w_err_t   memblk_eraseall(w_blkdev_s *blkdev)
{
    w_uint32_t start;
    w_int32_t size;
    start = (w_uint32_t)((blkdev->blkaddr) * blkdev->blksize);
    size = blkdev->blkcnt * blkdev->blksize;
    wind_memset(&memblk[start],0,size);
    return W_ERR_OK;
}


w_int32_t memblk_read(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint32_t start;
    w_int32_t size;
    start = (w_uint32_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    //wind_notice("memblk_read:0x%08x,%d",start,size);
    wind_memcpy(buf,&memblk[start],size);
    return blkcnt;
}

w_int32_t memblk_write(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint32_t start;
    w_int32_t size;
    start = (w_uint32_t)((blkdev->blkaddr + addr) * blkdev->blksize);
    size = blkcnt * blkdev->blksize;
    //wind_notice("memblk_write:0x%08x,%d",start,size);
    wind_memcpy(&memblk[start],buf,size);
    return blkcnt;
}

w_err_t   memblk_close(w_blkdev_s *blkdev)
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

