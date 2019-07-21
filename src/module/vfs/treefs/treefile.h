#ifndef WIND_TREEFILE_H__
#define WIND_TREEFILE_H__
#include "treefs_def.h"

#if WIND_TREEFS_SUPPORT
#define NODE_TO_TREEFILE(node) (w_treefile_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((w_treefile_s*)0)->tree.treenode)))
w_treefile_s *treefs_mk_subnode(w_treefile_s *parent,char *nodename,w_uint8_t isdir);

w_treefile_s *treefile_create(w_treefs_s *tfs,const char *path);
w_err_t treefile_rm(w_treefile_s *file);


w_treefile_s* treefile_open(w_treefs_s *tfs,const char *path,w_uint16_t mode);

w_err_t treefile_close(w_treefile_s* file);

w_bool_t treefile_existing(w_treefs_s *tfs,const char *path);

w_err_t treefile_seek(w_treefile_s* file,w_int32_t offset);

w_int32_t treefile_ftell(w_treefile_s* file);

w_int32_t treefile_read(w_treefile_s* file,w_uint8_t *buff, w_int32_t size);

w_int32_t treefile_write(w_treefile_s* file,w_uint8_t *buff, w_int32_t size);

w_treefile_s *treefile_readdir(w_treefile_s* file,w_int32_t index);

w_err_t treefile_fgets(w_treefile_s* file,char *buff, w_int32_t maxlen);

w_err_t treefile_fputs(w_treefile_s* file,char *buff);

#endif
#endif
