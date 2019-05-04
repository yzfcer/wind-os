/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_bitmap.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块位图的操作接口
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
#ifndef WIND_LISTFS_BITMAP_H__
#define WIND_LISTFS_BITMAP_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "listfs.h"
#include "wind_blkdev.h"
#if WIND_FS_SUPPORT
#define BITMAP_USED (0x01 << 0)
#define BITMAP_BAD  (0x01 << 1)
typedef struct
{
    w_addr_t addr1;         //主位图块起始地址
    w_addr_t addr2;         //备份位图块起始地址
    w_int32_t free_blkidx;   //空闲位图块位置
    w_int32_t addr_cnt;      //位图块数量
    w_int32_t free_byteidx;  //空闲位图字节位置
    w_blkdev_s *blkdev;
}lfs_bitmap_s;

w_err_t listfs_bitmap_init(lfs_bitmap_s *bp,w_addr_t start_addr,w_int32_t count,w_blkdev_s *blkdev);

w_err_t listfs_bitmap_update(lfs_bitmap_s *bp);

w_err_t listfs_bitmap_set(lfs_bitmap_s *bp,w_addr_t addr,w_uint8_t bitflag);

w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t cnt);

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bp,w_addr_t *addr,w_int32_t count);

w_err_t listfs_bitmap_clear(lfs_bitmap_s *bp);

w_uint32_t listfs_bitmap_get_usedblk(lfs_bitmap_s *bp);


#endif
#endif
