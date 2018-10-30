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
#include <stdio.h>
#include "boot_media.h"
#include "wind_string.h"
#include "wind_debug.h"
//硬件RAM数量和基地址和空间大小定义
#define ROM1_SIZE 0x80000
#define ROM1_BLKSIZE 512
static w_uint8_t g_rom1[ROM1_SIZE];

static void flush_rom_file(void)
{
    FILE *file;
    file = fopen("rom1.bin","wb+");
    fwrite(g_rom1,1,ROM1_SIZE,file);
    fclose(file);
}

static void read_rom_file(void)
{
    FILE *file;
    file = fopen("rom1.bin","rb");
    fread(g_rom1,1,ROM1_SIZE,file);
    fclose(file);
}

static w_err_t rom1_init(w_media_s *media)
{
    FILE *file;
    file = fopen("rom1.bin","rb");
    if(file == W_NULL)
    {
        file = fopen("rom1.bin","wb+");
        WIND_ASSERT_RETURN(file != 0,W_ERR_FAIL);
    }
    fclose(file);
    media->offset = 0;
    wind_memset(g_rom1,0,ROM1_SIZE);
    return W_ERR_OK;
}

static w_int32_t rom1_read_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    read_rom_file();
    wind_memcpy(data,&g_rom1[addr],blkcnt * ROM1_BLKSIZE);
    return blkcnt;
}
static w_int32_t rom1_write_blk(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt)
{
    wind_memcpy(&g_rom1[addr],data,blkcnt * ROM1_BLKSIZE);
    flush_rom_file();
    return blkcnt;
}

static w_int32_t rom1_erase_blk(w_media_s *media,w_uint32_t addr,w_int32_t blkcnt)
{
    wind_memset(&g_rom1[addr],0,blkcnt * ROM1_BLKSIZE);
    flush_rom_file();
    return blkcnt;
}

static w_err_t rom1_erase_all(w_media_s *media)
{
    wind_memset(g_rom1,0,ROM1_SIZE);
    flush_rom_file();
    return W_ERR_OK;
}
static w_media_ops_s ops = 
{
    rom1_init,
    rom1_read_blk,
    rom1_write_blk,
    rom1_erase_blk,
    rom1_erase_all
};

w_media_s media_rom1 = 
{
    "rom1",
    MEDIA_TYPE_ROM,
    0,
    ROM1_SIZE,
    ROM1_BLKSIZE,
    0,
    &ops
};




