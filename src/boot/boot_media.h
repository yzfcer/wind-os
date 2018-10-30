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
#ifndef BOOT_MEDIA_H__
#define BOOT_MEDIA_H__
#include "wind_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEDIA_NAME_LEN 12
typedef enum 
{
    MEDIA_TYPE_RAM = 0,
    MEDIA_TYPE_ROM = 1,
}w_media_type_e;

typedef struct __w_media_s w_media_s;
typedef struct __w_media_ops_s w_media_ops_s;


struct __w_media_ops_s
{
    w_err_t (*init)(w_media_s *media);
    w_int32_t (*read_blk)(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt);
    w_int32_t (*write_blk)(w_media_s *media,w_uint32_t addr,w_uint8_t *data,w_int32_t blkcnt);
    w_int32_t (*erase_blk)(w_media_s *media,w_uint32_t addr,w_int32_t blkcnt);
    w_err_t (*erase_all)(w_media_s *media);
};

struct __w_media_s
{
    char name[MEDIA_NAME_LEN];
    w_uint8_t mtype;
    w_uint32_t base;
    w_uint32_t size;
    w_uint32_t blksize;
    w_uint32_t offset;
    w_media_ops_s *ops;
};

w_err_t boot_media_init(void);
w_err_t boot_media_register(w_media_s *media);
w_err_t boot_media_unregister(w_media_s *media);
w_media_s *boot_media_get(char *name);
void boot_media_print(void);



#ifdef __cplusplus
}
#endif

#endif

