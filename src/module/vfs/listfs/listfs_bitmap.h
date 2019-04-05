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


w_err_t listfs_bitmap_init(listfs_s *listfs);

w_err_t listfs_bitmap_set(listfs_s *listfs,w_int32_t unit_idx,w_uint8_t bitflag);

w_err_t listfs_bitmap_find_free(listfs_s *listfs,w_int32_t *freeidx);

w_err_t listfs_bitmap_clear(listfs_s *listfs);

#endif
#endif
