/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: wind_blkdev.h
**Author: Jason Zhou
**Last Date: 
**Description: 块设备标准化API接口
**              
**--------------History----------------------------------------------------------------------------
Author: 
** Version: v1.0
** Date: 
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_BLKDEV_H__
#define WIND_BLKDEV_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"
#include "wind_dlist.h"
#include "wind_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif
#if WIND_BLKDEV_SUPPORT

#define WIND_BLKDEV_MAGIC 0xB88F3D9A
#define F_BLKDEV_OPEN (0x01 << 0) //标记blkdev对象是否已经打开
#define IS_F_BLKDEV_OPEN(chdev) ((chdev->obj.flag & F_BLKDEV_OPEN) == F_BLKDEV_OPEN)
#define SET_F_BLKDEV_OPEN(chdev) (chdev->obj.flag |= F_BLKDEV_OPEN)
#define CLR_F_BLKDEV_OPEN(chdev) (chdev->obj.flag &= (~F_BLKDEV_OPEN))
typedef enum 
{
    BLKDEV_COMMON = 0x01,
    BLKDEV_NULL,
    BLKDEV_RAM,
    BLKDEV_ROM,
    BLKDEV_NOR,
    BLKDEV_NAND,
    BLKDEV_EEPROM,
    BLKDEV_SPIFLASH,
}w_blkdev_e;

typedef struct __w_blkdev_s w_blkdev_s;
typedef struct __w_blkdev_ops_s w_blkdev_ops_s;
struct __w_blkdev_s
{
    w_obj_s obj;
    w_int16_t devtype;
    w_int16_t devid;
    w_addr_t blkaddr;
    w_uint32_t blkcnt;
    w_uint32_t blksize;
    w_mutex_s *mutex;
    void *user_arg;
    const w_blkdev_ops_s *ops;
};

#define WIND_BLKDEV_DEF(name,devtype,devid,addr,blkcnt,blksize,ops) \
{WIND_OBJ(WIND_BLKDEV_MAGIC,0,name),devtype,devid,addr,blkcnt,blksize,W_NULL,W_NULL,ops}

struct __w_blkdev_ops_s
{
    w_err_t   (*init)(w_blkdev_s *blkdev);
    w_err_t   (*deinit)(w_blkdev_s *blkdev);
    w_err_t   (*open)(w_blkdev_s *blkdev);
    w_err_t   (*erase)(w_blkdev_s *blkdev,w_addr_t addr,w_int32_t blkcnt);
    w_err_t   (*eraseall)(w_blkdev_s *blkdev);
    w_int32_t (*read)(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt);
    w_int32_t (*write)(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt);
    w_err_t   (*close)(w_blkdev_s *blkdev);
};
w_err_t _wind_blkdev_mod_init(void);
w_err_t _register_blkdevs(void);

w_err_t wind_blkdev_register(w_blkdev_s *blkdev,w_int32_t count);
w_err_t wind_blkdev_unregister(w_blkdev_s *blkdev);

w_blkdev_s *wind_blkdev_get(const char *name);
w_err_t wind_blkdev_open(w_blkdev_s *blkdev);
w_int32_t wind_blkdev_read(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_int32_t wind_blkdev_write(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_err_t wind_blkdev_erase(w_blkdev_s *blkdev,w_addr_t blkaddr,w_int32_t blkcnt);
w_err_t wind_blkdev_eraseall(w_blkdev_s *blkdev);
w_err_t wind_blkdev_close(w_blkdev_s *blkdev);
w_err_t wind_blkdev_print_list(void);
w_err_t wind_blkdev_print_detail(void);

#endif

#ifdef __cplusplus
}
#endif


#endif

