/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation--------------------------------------------------------------------------------
**FileName: listfs_bitmap.h
**Author: Jason Zhou
**Last Date: 2019.04.05
**Description: 文件系统对块位图的操作接口
**              
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2019.04.05
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: Jason Zhou
** Date: 2019.04.05
** Description: 
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
    w_uint32_t addr1;         //主位图块起始地址
    w_uint32_t addr2;         //备份位图块起始地址
    w_int32_t bmblk_cnt;    //位图块数量
    w_uint32_t data_addr;      //数据块起始位置
    w_int32_t cur_blkidx;   //空闲位图块位置
    w_int32_t cur_byteidx;  //空闲位图字节位置
    w_blkdev_s *blkdev;      
    w_uint8_t *blk;          //数据块
}lfs_bitmap_s;

w_err_t listfs_bitmap_init(lfs_bitmap_s *bm,w_uint32_t start_addr,w_int32_t count,w_blkdev_s *blkdev);

w_err_t listfs_bitmap_deinit(lfs_bitmap_s *bm);


//w_err_t bm_update_freeidx(lfs_bitmap_s *bm);

//w_err_t listfs_bitmap_set(lfs_bitmap_s *bm,w_uint32_t addr,w_uint8_t bitflag);

w_err_t listfs_bitmap_alloc_blk(lfs_bitmap_s *bm,w_uint32_t *addr,w_int32_t addr_cnt);

w_err_t listfs_bitmap_free_blk(lfs_bitmap_s *bm,w_uint32_t *addr,w_int32_t addr_cnt);

w_err_t listfs_bitmap_clear(lfs_bitmap_s *bm);

w_int32_t listfs_bitmap_calc_usedblk(lfs_bitmap_s *bm);


#endif
#endif
