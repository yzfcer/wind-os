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
#include "boot_media.h"
#include "wind_string.h"
#include "wind_debug.h"
//硬件RAM数量和基地址和空间大小定义
#define RAM1_BASE 0x20000000
#define RAM1_SIZE 0x100000
#define RAM1_BLKSIZE 512
static w_uint8_t g_ram1[RAM1_SIZE];


static w_err_t ram1_init(w_media_s *media)
{
    wind_memset(g_ram1,0,RAM1_SIZE);
    media->offset = 0;
    return W_ERR_OK;
}
static w_int32_t ram1_read_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    wind_memcpy(data,&g_ram1[addr],blkcnt * RAM1_BLKSIZE);
    return blkcnt;
}
static w_int32_t ram1_write_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    wind_memcpy(&g_ram1[addr],data,blkcnt * RAM1_BLKSIZE);
    return blkcnt;
}

static w_int32_t ram1_erase_blk(w_media_s *media,w_uint32_t addr,w_int32_t blkcnt)
{
    wind_memset(&g_ram1[addr],0,blkcnt * RAM1_BLKSIZE);
    return blkcnt;
}
static w_err_t ram1_erase_all(w_media_s *media)
{
    wind_memset(g_ram1,0,RAM1_SIZE);
    return W_ERR_OK;
}
static w_media_ops_s ops = 
{
    ram1_init,
    ram1_read_blk,
    ram1_write_blk,
    ram1_erase_blk,
    ram1_erase_all
};

w_media_s media_ram1 = 
{
    "ram1",
    MEDIA_TYPE_RAM,
    0,
    RAM1_SIZE,
    RAM1_BLKSIZE,
    0,
    &ops
};



