#include "wind_blkdev.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "w25qxx.h"


w_err_t   spi_flash_open(blkdev_s *dev)
{
    static w_bool_t init_flag = B_FALSE;
    if(wind_strcmp(dev->name,"spi_flash0") == 0)
    {
        dev->blkaddr = 0;
        dev->blksize = 512;
        dev->blkcnt = 16384;        
    }
    else
    {
        dev->blkaddr = 16384;
        dev->blksize = 512;
        dev->blkcnt = 16384;        
    }
    if(!init_flag)
    {
        W25QXX_Init();
        init_flag = B_TRUE;
    }
    return ERR_OK;
}

w_err_t   spi_flash_erase(blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_Erase_Sector((w_uint32_t)(start + i * dev->blksize));
    return ERR_OK;
}

w_err_t   spi_flash_eraseall(blkdev_s *dev)
{
    W25QXX_Erase_Chip();
    return ERR_OK;
}


w_int32_t spi_flash_read(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_Read(buf,(w_uint32_t)(start + i * dev->blksize),dev->blksize);
    return ERR_OK;
}

w_int32_t spi_flash_write(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{   
    w_int32_t i;
    w_uint8_t *start;
    start = (w_uint8_t *)((dev->blkaddr + addr) * dev->blksize);
    for(i = 0;i < blkcnt;i ++)
        W25QXX_SectorWrite(buf,(w_uint32_t)(start + i * dev->blksize),dev->blksize);
    return ERR_OK;
}

w_err_t   spi_flash_close(blkdev_s *dev)
{
    return ERR_OK;
}

const blkdev_ops_s spi_flash_ops = 
{
    spi_flash_open,
    spi_flash_erase,
    spi_flash_eraseall,
    spi_flash_read,
    spi_flash_write,
    spi_flash_close
};

blkdev_s spi_flash_dev[2] = 
{
    {
        WIND_BLKDEV_MAGIC,
        {NULL,NULL},
        "spi_flash0",
        B_FALSE,
        0,16384,512,
        NULL,
        &spi_flash_ops
    },
    {
        WIND_BLKDEV_MAGIC,
        {NULL,NULL},
        "spi_flash1",
        B_FALSE,
        16384,16384,512,
        NULL,
        &spi_flash_ops
    }
};




