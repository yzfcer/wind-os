/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_blkdev.h
**创   建   人: 周江村
**最后修改日期: 
**描        述: 块设备标准化API接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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
#if WIND_BLK_DRVFRAME_SUPPORT

#define WIND_BLKDEV_MAGIC 0xB88F3D9A
typedef struct __w_blkdev_s w_blkdev_s;
typedef struct __w_blkdev_ops_s w_blkdev_ops_s;
struct __w_blkdev_s
{
    w_uint32_t magic;
    char name[12];
    w_int8_t dev_id;
    w_addr_t blkaddr;
    w_int32_t blkcnt;
    w_int32_t blksize;
    w_bool_t opened;
    w_dnode_s blkdevnode;
    w_mutex_s *mutex;
    const w_blkdev_ops_s *ops;
};
#define WIND_BLKDEV_DEF(name,devid,addr,blkcnt,blksize,ops) \
{WIND_BLKDEV_MAGIC,name,devid,addr,blkcnt,blksize,W_FALSE,{W_NULL,W_NULL},W_NULL,ops}

struct __w_blkdev_ops_s
{
    w_err_t   (*init)(w_blkdev_s *blkdev);
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

w_blkdev_s *wind_blkdev_get(char *name);
w_err_t wind_blkdev_open(w_blkdev_s *blkdev);
w_int32_t wind_blkdev_read(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_int32_t wind_blkdev_write(w_blkdev_s *blkdev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_err_t wind_blkdev_erase(w_blkdev_s *blkdev,w_addr_t blkaddr,w_int32_t blkcnt);
w_err_t wind_blkdev_eraseall(w_blkdev_s *blkdev);
w_err_t wind_blkdev_close(w_blkdev_s *blkdev);
w_err_t wind_blkdev_print(w_dlist_s *list);

#endif

#ifdef __cplusplus
}
#endif


#endif

