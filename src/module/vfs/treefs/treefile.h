/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : treefile.h
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Treefs file operation interface
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
#ifndef WIND_TREEFILE_H__
#define WIND_TREEFILE_H__
#include "treefs_def.h"

#if WIND_TREEFS_SUPPORT
#define NODE_TO_TREEFILE(node) (w_treefile_s*)(((w_uint8_t*)(node))-((w_addr_t)&(((w_treefile_s*)0)->tree.treenode)))
w_treefile_s *treefs_mk_subnode(w_treefile_s *parent,char *nodename,w_uint8_t isdir);

w_treefile_s *treefile_create(w_treefs_s *tfs,const char *path);
w_err_t treefile_remove(w_treefile_s *file);


w_treefile_s* treefile_open(w_treefs_s *tfs,const char *path,w_uint16_t mode);

w_err_t treefile_close(w_treefile_s* file);

w_bool_t treefile_existing(w_treefs_s *tfs,const char *path);

w_err_t treefile_seek(w_treefile_s* file,w_uint32_t offset);

w_int32_t treefile_ftell(w_treefile_s* file);

w_int32_t treefile_read(w_treefile_s* file,w_uint8_t *buff, w_int32_t size);

w_int32_t treefile_write(w_treefile_s* file,w_uint8_t *buff, w_int32_t size);

w_err_t treefile_readdir(w_treefile_s* dir,w_treefile_s **sub);


#endif
#endif
