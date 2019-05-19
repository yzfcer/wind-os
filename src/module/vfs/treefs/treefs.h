#ifndef TREEFS_H__
#define TREEFS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_tree.h"
#include "treefs_def.h"
#if WIND_FS_SUPPORT

#define F_TREEFS_POOL (0x01 << 0) //标记treefs对象是否通过内存池分�?
#define IS_F_TREEFS_POOL(sem) ((sem->obj.flag & F_TREEFS_POOL) == F_TREEFS_POOL)
#define SET_F_TREEFS_POOL(sem) (sem->obj.flag |= F_TREEFS_POOL)
#define CLR_F_TREEFS_POOL(sem) (sem->obj.flag &= (~F_TREEFS_POOL))

w_err_t _treefs_mod_init(void);

void *tfs_malloc(w_int32_t size);
w_err_t tfs_free(void *ptr);

w_err_t wind_treefs_format(void);

w_treefs_s *wind_treefs_create(char *name);


#endif //#if WIND_FS_SUPPORT
#endif //#ifndef TREEFS_H__
