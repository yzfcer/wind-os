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
