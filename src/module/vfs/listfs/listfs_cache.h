/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: listfs_cache.h
**��   ��   ��: Jason Zhou
**����޸�����: 2019.04.05
**��        ��: �ļ�ϵͳ�Կ��豸�Ĳ����ӿ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2019.04.05
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2019.04.05
** �衡��: 
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

#define F_LFSCACHE_DIRTY (0x01 << 0) //��ǻ���������Ƿ������
#define IS_F_LFSCACHE_DIRTY(cache) ((cache->obj.flag & F_LFSCACHE_DIRTY) == F_LFSCACHE_DIRTY)
#define SET_F_LFSCACHE_DIRTY(cache) (cache->obj.flag |= F_LFSCACHE_DIRTY)
#define CLR_F_LFSCACHE_DIRTY(cache) (cache->obj.flag &= (~F_LFSCACHE_DIRTY))

#define F_LFSCACHE_HIT (0x01 << 1) //��ǻ�������������е�
#define IS_F_LFSCACHE_HIT(cacheitem) ((cacheitem->flag & F_LFSCACHE_HIT) == F_LFSCACHE_HIT)
#define SET_F_LFSCACHE_HIT(cacheitem) (cacheitem->flag |= F_LFSCACHE_HIT)
#define CLR_F_LFSCACHE_HIT(cacheitem) (cacheitem->flag &= (~F_LFSCACHE_HIT))

#define F_LFSCACHE_SW (0x01 << 2) //��ǻ�����������û���
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

//���������Ϣ
typedef struct __lfs_cache_s
{
    w_obj_s obj;
    w_uint32_t r_hit;
    w_uint32_t r_miss;
    w_uint32_t w_hit;
    w_uint32_t w_miss;
    w_uint32_t itemcount;     //����Ԫ������
    //w_uint32_t itemsize;      //����Ԫ�ش�С
    w_dlist_s itemlist;       //����Ԫ���б�
    
}lfs_cache_s;

w_err_t lfs_cache_init(lfs_cache_s *cache,w_uint32_t itemcount,w_uint32_t blksize);

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_flush(lfs_cache_s *cache,w_blkdev_s *blkdev);

w_err_t lfs_cache_clear(lfs_cache_s *cache,w_blkdev_s *blkdev);

#endif
#endif

