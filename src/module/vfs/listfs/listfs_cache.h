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
#ifndef LISTFS_CAVHE_H__
#define LISTFS_CAVHE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_blkdev.h"
#if WIND_LISTFS_SUPPORT

#define LISTFS_CACHE_MAGIC 0x9357D26A

#define LFS_CACHEITEM_MAX_CNT 5

#define F_LFSCACHE_DIRTY (0x01 << 0) //标记缓存的数据是否是脏的
#define IS_F_LFSCACHE_DIRTY(cache) ((cache->obj.flag & F_LFSCACHE_DIRTY) == F_LFSCACHE_DIRTY)
#define SET_F_LFSCACHE_DIRTY(cache) (cache->obj.flag |= F_LFSCACHE_DIRTY)
#define CLR_F_LFSCACHE_DIRTY(cache) (cache->obj.flag &= (~F_LFSCACHE_DIRTY))

#define F_LFSCACHE_HIT (0x01 << 1) //标记缓存的数据是命中的
#define IS_F_LFSCACHE_HIT(cacheitem) ((cacheitem->flag & F_LFSCACHE_HIT) == F_LFSCACHE_HIT)
#define SET_F_LFSCACHE_HIT(cacheitem) (cacheitem->flag |= F_LFSCACHE_HIT)
#define CLR_F_LFSCACHE_HIT(cacheitem) (cacheitem->flag &= (~F_LFSCACHE_HIT))

#define F_LFSCACHE_SW (0x01 << 2) //标记缓存的数据是置换的
#define IS_F_LFSCACHE_SW(cacheitem) ((cacheitem->flag & F_LFSCACHE_SW) == F_LFSCACHE_SW)
#define SET_F_LFSCACHE_SW(cacheitem) (cacheitem->flag |= F_LFSCACHE_SW)
#define CLR_F_LFSCACHE_SW(cacheitem) (cacheitem->flag &= (~F_LFSCACHE_SW))

typedef struct __lcache_item_s
{
    w_addr_t  addr;
    w_int16_t blksize;
    w_int16_t flag;
    w_dnode_s itemnode;
    w_int8_t  *blk;
}lcache_item_s;

//缓存对象信息
typedef struct __lfs_cache_s
{
    w_obj_s obj;
    w_uint32_t r_hit;
    w_uint32_t r_miss;
    w_uint32_t w_hit;
    w_uint32_t w_miss;
    w_uint32_t itemcount;     //缓存元素数量
    //w_uint32_t itemsize;      //缓存元素大小
    w_dlist_s itemlist;       //缓存元素列表
    
}lfs_cache_s;

w_err_t lfs_cache_init(lfs_cache_s *cache,w_uint32_t itemcount,w_uint32_t blksize);

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_flush(lfs_cache_s *cache,w_blkdev_s *blkdev);

w_err_t lfs_cache_clear(lfs_cache_s *cache,w_blkdev_s *blkdev);

#endif
#endif

