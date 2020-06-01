/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : treefs.c
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Treefs file system infomation operation interface
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.04.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.04.05
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_TREEFS_H__
#define WIND_TREEFS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_tree.h"
#include "treefs_def.h"
#if WIND_TREEFS_SUPPORT

#define F_TREEFS_POOL (0x01 << 0) //Mark whether the trees object is allocated through the memory pool
#define IS_F_TREEFS_POOL(sem) ((sem->obj.flag & F_TREEFS_POOL) == F_TREEFS_POOL)
#define SET_F_TREEFS_POOL(sem) (sem->obj.flag |= F_TREEFS_POOL)
#define CLR_F_TREEFS_POOL(sem) (sem->obj.flag &= (~F_TREEFS_POOL))

w_err_t _wind_treefs_mod_init(void);

void *tfs_mem_malloc(w_int32_t size);
w_err_t tfs_mem_free(void *ptr);

w_err_t wind_treefs_format(w_treefs_s *tfs);

w_treefs_s *wind_treefs_get(const char *name);

w_treefs_s *wind_treefs_create(char *name);

w_err_t wind_treefs_destroy(w_treefs_s *treefs);

#endif //#if WIND_MODULE_VFS_SUPPORT
#endif //#ifndef TREEFS_H__
