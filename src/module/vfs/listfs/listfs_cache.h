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
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#define LISTFS_CACHE_MAGIC 09x357D26AC

#define F_LFSCACHE_DIRTY (0x01 << 0) //��ǻ���������Ƿ������
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

//���������Ϣ
typedef struct __lfs_cache_s
{
    w_obj_s obj;
    w_uint32_t itemcount;     //����Ԫ������
}lfs_cache_s;

w_err_t lfs_cache_init(lfs_cache_s *cache);

w_err_t lfs_cache_read(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_write(lfs_cache_s *cache,w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t *blk);

w_err_t lfs_cache_flush(lfs_cache_s *cache,w_blkdev_s *blkdev);

w_err_t lfs_cache_clear(lfs_cache_s *cache,w_blkdev_s *blkdev);

#endif

