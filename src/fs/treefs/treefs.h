#ifndef WIND_TREEFS_H__
#define WIND_TREEFS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_tree.h"
#include "wind_file.h"
#if WIND_FS_SUPPORT
#define TREEFILE_MAGIC 0x48A97D26
#define TREEFS_DIR_LAYCNT 32

#define TREEFS_BLK_SIZE 128

#if  0
#define TREEFS_SET_ATTR_READONLY(attr,v) ((attr.vattr.readonly) = v?1:0)
#define TREEFS_SET_ATTR_ISDIR(attr,v) ((attr.vattr.isdir) = v?1:0)
#define TREEFS_SET_ATTR_MODE(attr,v) ((attr.vattr.mode) = (w_uint16_t)(v))

#define TREEFS_GET_ATTR_READONLY(attr,v) (attr.vattr.readonly)
#define TREEFS_GET_ATTR_ISDIR(attr,v) (attr.vattr.isdir)
#define TREEFS_GET_ATTR_MODE(attr,v) (attr.vattr.mode)
#endif

typedef struct 
{
    w_uint16_t o_x:1;
    w_uint16_t o_w:1;
    w_uint16_t o_r:1;
    w_uint16_t g_x:1;
    w_uint16_t g_w:1;
    w_uint16_t g_r:1;
    w_uint16_t u_x:1;
    w_uint16_t u_w:1;
    w_uint16_t u_r:1;
    w_uint16_t isdir:1;
}tf_attr_s;

typedef union 
{
    w_uint16_t vattr;
    tf_attr_s sattr;
}attr_u;

typedef struct treefs_s
{
    w_uint32_t magic;
    tree_s tree;
    char *filename;
    w_uint16_t attr;
    w_uint8_t mode;
    w_uint32_t offset;
    w_uint32_t filelen;
    w_uint32_t bufflen;
    dlist_s datalist;
}treefile_s;


w_err_t treefs_format(void);
treefile_s *treefs_mk_file(const char *path);
w_err_t treefs_rm_file(treefile_s *file);

w_err_t treefile_get_attr(treefile_s *file,tf_attr_s *attr);
w_err_t treefile_set_attr(treefile_s *file,tf_attr_s *attr);


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
