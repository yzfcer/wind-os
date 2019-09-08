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
#if WIND_LISTFS_SUPPORT
#define LISTFS_BITMAP_MAGIC 0x37D65A82
#define BITMAP_FREE (0x00 << 0)
#define BITMAP_USED (0x01 << 0)
#define BITMAP_BAD  (0x01 << 1)
typedef struct
{
    w_uint32_t magic;
    w_addr_t addr1;         //主位图块起始地址
    w_addr_t addr2;         //备份位图块起始地址
    w_int32_t bmblk_cnt;    //位图块数量
    w_addr_t data_addr;      //数据块起始位置
    w_int32_t cur_blkidx;   //空闲位图块位置
    w_int32_t cur_byteidx;  //空闲位图字节位置
    w_blkdev_s *blkdev;      
    w_uint8_t *blk;          //数据块
}lfs_bitmap_s;

w_err_t listfs_bitmap_init(lfs_bitmap_s *bm,w_addr_t start_addr,w_int32_t count,w_blkdev_s *blkdev);

w_err_t listfs_bitmap_deinit(lfs_bitmap_s *bm);


//w_err_t bm_update_freeidx(lfs_bitmap_s *bm);

//w_err_t listfs_bitmap_set(lfs_bitmap_s *bm,w_addr_t addr,w_uint8_t bitflag);

w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t addr_cnt);

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bm,w_addr_t *addr,w_int32_t addr_cnt);

w_err_t listfs_bitmap_clear(lfs_bitmap_s *bm);

w_int32_t listfs_bitmap_calc_usedblk(lfs_bitmap_s *bm);


#endif
#endif
