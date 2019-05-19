/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: fs_def.h
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: 文件系统通用结构定义
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef FS_DEF_H__
#define FS_DEF_H__

#include "wind_type.h"
#include "wind_dlist.h"

#define WIND_FS_MAGIC 0x235C79A5
#define WIND_FILE_MAGIC 0x275972D5
#define WFS_NAME_LEN 12
#define WFILE_NAME_LEN 20
#define FS_MOUNT_PATH_LEN 64
#define FS_CUR_PATH "/"

#define FMODE_R  0x01
#define FMODE_W  0x02
#define FMODE_RW  0x03
#define FMODE_CRT  0x04
#define FMODE_A  0x08

typedef enum 
{
    FSTYPE_TREEFS = 0x01,
    FSTYPE_UNDEF = 0xff,
}w_fstype_e;

typedef enum 
{
    FTYPE_DIR = 0x01,
    FTYPE_FILE = 0x02,
}w_ftype_e;

typedef struct __w_fs_ops_s w_fs_ops_s;
typedef struct __w_fs_s w_fs_s;
typedef struct __w_file_s w_file_s;

struct __w_fs_s
{
    w_uint32_t magic;
    char *name;
    char *mount_path;
    w_dnode_s fsnode;
    w_fstype_e fstype;
    void *fsobj;
    w_blkdev_s *blkdev;
    w_fs_ops_s *ops;
};
struct __w_fs_ops_s
{
    w_err_t (*init)(w_fs_s *fs);
    w_err_t (*format)(w_fs_s *fs);
    
    w_err_t (*open)(w_file_s *file,w_uint16_t fmode);
    w_err_t (*close)(w_file_s* file);
    w_err_t (*remove)(w_file_s* file);
    char *(*subfile)(w_file_s* dir,w_int32_t index);
    w_err_t (*seek)(w_file_s* file,w_int32_t offset);
    w_err_t (*rename)(w_file_s* file,char *newname);
    w_int32_t (*ftell)(w_file_s* file);
    w_int32_t (*read)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
    w_int32_t (*write)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
    w_err_t (*fgets)(w_file_s* file,char *buff, w_int32_t maxlen);
    w_err_t (*fputs)(w_file_s* file,char *buff);
};

struct __w_file_s
{
    char *path;
    char *subname;
    w_dnode_s filenode;//链表节点
    w_uint16_t fmode;//操作模式
    w_uint8_t ftype;//文件系统类型
    w_uint8_t isdir;
    w_fs_s *fs;
    void *fileobj;//文件对象
    w_int32_t offset;//偏移量
    w_mutex_s *mutex;//文件操作变量
    //w_fs_ops_s *ops;//操作函数集
};


#define FS_OPS_DEF(fs) \
static w_fs_ops_s fs_ops = {\
fs##_op_init,\
fs##_op_format,\
fs##_op_open,\
fs##_op_close,\
fs##_op_rmfile,\
fs##_op_subfile,\
fs##_op_seek,\
fs##_op_rename,\
fs##_op_ftell,\
fs##_op_read,\
fs##_op_write,\
fs##_op_fgets,\
fs##_op_fputs,\
}

#endif
