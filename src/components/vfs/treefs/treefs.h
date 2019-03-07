#ifndef WIND_TREEFS_H__
#define WIND_TREEFS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_tree.h"
//#include "wind_file.h"

#if WIND_FS_SUPPORT

#define TF_FMODE_R  0x01
#define TF_FMODE_W  0x02
#define TF_FMODE_RW  0x03
#define TF_FMODE_CRT 0x04
#define TF_FMODE_A  0x08


#define TREEFILE_MAGIC 0x48A97D26
#define TREEFS_DIR_LAYCNT 32

#define TREEFS_BLK_SIZE 128


typedef struct treefs_s
{
    w_uint32_t magic;
    w_tree_s tree;
    char *filename;
    w_uint16_t isdir:1;
    w_uint8_t mode;
    w_uint32_t offset;
    w_uint32_t filelen;
    w_uint32_t bufflen;
    w_dlist_s datalist;
}treefile_s;
#define NODE_TO_TREEFILE(node) (treefile_s*)(((w_uint8_t*)(node))-((w_uint32_t)&(((treefile_s*)0)->tree.treenode)))

void *treefs_malloc(w_int32_t size);
w_err_t treefs_free(void *ptr);

w_err_t treefs_format(void);


treefile_s *treefile_create(const char *path);
w_err_t treefile_rm(treefile_s *file);


treefile_s* treefile_open(const char *path,w_uint16_t mode);

w_err_t treefile_close(treefile_s* file);

w_bool_t treefile_existing(const char *path);

w_err_t treefile_seek(treefile_s* file,w_int32_t offset);

w_int32_t treefile_ftell(treefile_s* file);

w_int32_t treefile_read(treefile_s* file,w_uint8_t *buff, w_int32_t size);

w_int32_t treefile_write(treefile_s* file,w_uint8_t *buff, w_int32_t size);

treefile_s *treefile_readdir(treefile_s* file,w_int32_t index);

w_err_t treefile_fgets(treefile_s* file,char *buff, w_int32_t maxlen);

w_err_t treefile_fputs(treefile_s* file,char *buff);

#endif
#endif
