/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_cache.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块设备的操作接口
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.04.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.04.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#define LISTFS_CACHE_MAGIC 09x357D26AC

#define F_LFSCACHE_DIRTY (0x01 << 0) //标记缓存的数据是否是脏的
#define IS_F_LFSCACHE_DIRTY(cache) ((cache->obj.flag & F_LFSCACHE_DIRTY) == F_LFSCACHE_DIRTY)
#define SET_F_LFSCACHE_DIRTY(cache) (cache->obj.flag |= F_LFSCACHE_DIRTY)
#define CLR_F_LFSCACHE_DIRTY(cache) (cache->obj.flag &= (~F_LFSCACHE_DIRTY))

typedef struct __lcache_item_s
{
    w_addr_t  addr;
    w_int32_t blksize;
    w_dnode_s itemnode;
    w_int8_t  *blk;
}lcache_item_s;

//缓存对象信息
typedef struct __lfs_cache_s
{
    w_obj_s obj;
    w_uint32_t itemcount;     //缓存元素数量
}lfs_cache_s;

w_err_t lfs_cache_init(lfs_cache_s *cache);

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_flush(lfs_cache_s *cache,w_blkdev_s *blkdev);

w_err_t lfs_cache_clear(lfs_cache_s *cache,w_blkdev_s *blkdev);

#endif

