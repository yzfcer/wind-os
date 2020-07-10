/*********************************************************************************
** Copyright(C),2017-2020,yzfcer@163.com
** FileName    : boot_part.h
** Author      : Jason Zhou
** Version     : 1.0
** Date        : 2017/04/08
** Description : 
** Others      : 
** Modify      :  
** Date        :
** Author      :
** Modify      :
**********************************************************************************/
#ifndef BOOT_PART_H__
#define BOOT_PART_H__
#include "wind_type.h"
#include "wind_blkdev.h"
//#include "boot_media.h"

#define PART_NAME_LEN 12
#define INVALID_REAL_ADDR 0xffffffff

//Standard partition name
#define PART_BOOT     "boot"
#define PART_PARAM1   "param0"
#define PART_PARAM2   "param1"
#define PART_IMG1     "img0"
#define PART_IMG2     "img1"
#define PART_SHARE    "share"
#define PART_FS       "rootfs"
#define PART_SYSRUN   "sysrun"
#define PART_IMGPARA  "imgprm"
#define PART_CACHE    "cache"


typedef enum
{
    MEM_NULL = 0,
    MEM_NORMAL=1,
    MEM_ERROR=2,
}w_mem_status_e;

//Spatial partition table
typedef struct 
{
    w_blkdev_s blkdev;
    char name[PART_NAME_LEN];
    w_uint8_t used:1;
    w_uint8_t encrypt:1;
    w_uint8_t status:2;
    w_uint8_t mtype;
    w_uint16_t time_mark;
    w_uint32_t size;
    w_uint32_t datalen;
    w_uint32_t crc;
}w_part_s;
w_uint8_t *get_common_buffer(void);

w_err_t boot_part_init(void);
w_bool_t  boot_part_create(const char *name,w_uint8_t encrypt);
w_part_s *boot_part_get(const char *name);
w_err_t boot_part_update_rom(w_part_s *ptlist);
w_err_t boot_part_calc_crc(w_part_s *part,w_int32_t offset,w_int32_t len,w_bool_t set);
w_int32_t boot_part_read(w_part_s *part,w_int32_t offset,w_uint8_t *data,w_uint32_t datalen,w_bool_t read_space);
w_int32_t boot_part_write(w_part_s *part,w_int32_t offset,w_uint8_t *data,w_uint32_t datalen);
w_err_t boot_part_erase(w_part_s *part);
void    boot_part_reset_ram(void);

void boot_part_print(void);
w_int32_t boot_part_get_count(void);
w_part_s *boot_part_get_list(void);
w_err_t boot_part_copy_data(w_part_s *src,w_part_s *dest);
w_bool_t boot_part_equal(w_part_s *src,w_part_s *dest);
w_bool_t boot_part_check(w_part_s *part);


 
#endif
