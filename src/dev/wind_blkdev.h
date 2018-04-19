/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_blkdev.h
**��   ��   ��: �ܽ���
**����޸�����: 
**��        ��: ϵͳ����������ص�ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
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
typedef struct __blkdev_s blkdev_s;
typedef struct __blkdev_ops_s blkdev_ops_s;
struct __blkdev_s
{
    w_uint32_t magic;
    dnode_s blkdevnode;
    char name[12];
    w_bool_t opened;
    w_addr_t blkaddr;
    w_int32_t blkcnt;
    w_int32_t blksize;
    mutex_s *mutex;
    const blkdev_ops_s *ops;
};

struct __blkdev_ops_s
{
    w_err_t   (*open)(blkdev_s *dev);
    w_err_t   (*erase)(blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt);
    w_err_t   (*eraseall)(blkdev_s *dev);
    w_int32_t (*read)(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt);
    w_int32_t (*write)(blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt);
    w_err_t   (*close)(blkdev_s *dev);
};
w_err_t _wind_blkdev_init(void);
w_err_t _register_blkdevs(void);

w_err_t wind_blkdev_register(blkdev_s *dev,w_int32_t count);
w_err_t wind_blkdev_unregister(blkdev_s *dev);

blkdev_s *wind_blkdev_get(char *name);
w_err_t wind_blkdev_open(blkdev_s *dev);
w_int32_t wind_blkdev_read(blkdev_s *dev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_int32_t wind_blkdev_write(blkdev_s *dev,w_addr_t blkaddr,w_uint8_t *buf,w_int32_t blkcnt);
w_err_t wind_blkdev_erase(blkdev_s *dev,w_addr_t blkaddr,w_int32_t blkcnt);
w_err_t wind_blkdev_eraseall(blkdev_s *dev);
w_err_t wind_blkdev_close(blkdev_s *dev);
w_err_t wind_blkdev_print(dlist_s *list);
#else
#define _wind_blkdev_init()
#endif

#ifdef __cplusplus
}
#endif


#endif
