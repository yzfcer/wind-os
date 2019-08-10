/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: fs_def.h
** 创   建   人: Jason Zhou
** 最后修改日期: 2019/1/24 20:24:37
** 描        述: vfs结构定义
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019/1/24 20:24:37
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef FS_DEF_H__
#define FS_DEF_H__

#include "wind_type.h"
#include "wind_obj.h"

#define WIND_VFS_MAGIC 0x235C79A5
#define WIND_FILE_MAGIC 0x275972D5
#define WIND_FSTYPE_MAGIC 0x3582A6B3
#define WFS_NAME_LEN 12
#define WFILE_NAME_LEN 20
#define FS_MOUNT_PATH_LEN 64
#define FS_CUR_PATH "/"

#define FMODE_R  0x01
#define FMODE_W  0x02
#define FMODE_RW  0x03
#define FMODE_CRT  0x04
#define FMODE_A  0x08

#define F_VFS_POOL (0x01 << 0) //标记fs对象是否通过内存池分配
#define IS_F_VFS_POOL(fs) ((fs->obj.flag & F_VFS_POOL) == F_VFS_POOL)
#define SET_F_VFS_POOL(fs) (fs->obj.flag |= F_VFS_POOL)
#define CLR_F_VFS_POOL(fs) (fs->obj.flag &= (~F_VFS_POOL))

#define F_VFS_MOUNT (0x01 << 1) //标记fs对象是否是否已被挂载
#define IS_F_VFS_MOUNT(vfs) ((vfs->obj.flag & F_VFS_MOUNT) == F_VFS_MOUNT)
#define SET_F_VFS_MOUNT(vfs) (vfs->obj.flag |= F_VFS_MOUNT)
#define CLR_F_VFS_MOUNT(vfs) (vfs->obj.flag &= (~F_VFS_MOUNT))

#if 0
typedef enum 
{
    FSTYPE_NONE   = 0x00,
    FSTYPE_TREEFS,
    FSTYPE_LISTFS,
    FSTYPE_FAT12,
    FSTYPE_FAT16,
    FSTYPE_FAT32,
    FSTYPE_EXT2,
    FSTYPE_EXT3,
    FSTYPE_EXT4,
    FSTYPE_ROMFS,
    FSTYPE_RAMFS,
    FSTYPE_NFS,
    FSTYPE_JFFS,
    FSTYPE_JFFS2,
    FSTYPE_YAFFS,
    FSTYPE_YAFFS2,
}w_fstype_e;
#endif

typedef enum 
{
    FTYPE_DIR = 0x01,
    FTYPE_FILE = 0x02,
}w_ftype_e;

typedef struct __w_fsops_s w_fsops_s;
typedef struct __w_vfs_s w_vfs_s;
typedef struct __w_file_s w_file_s;

struct __w_vfs_s
{
    w_obj_s obj;
    char *mount_path;
    char *fstype;
    void *fsobj;
    w_blkdev_s *blkdev;
    w_fsops_s *ops;
};

struct __w_fsops_s
{
    w_obj_s obj;
    void* (*init)(w_vfs_s *fs);//文件系统类型初始化
    w_err_t (*format)(w_vfs_s *fs);//格式化
    w_err_t (*matchfs)(char *devname);//检测块设备文件系统是否匹配
    
    w_err_t (*open)(w_file_s *file,w_uint16_t fmode);
    w_err_t (*close)(w_file_s* file);
    w_err_t (*remove)(w_file_s* file);
    w_err_t (*readdir)(w_file_s* dir,w_file_s* child);
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
    w_obj_s obj;
    char *fullpath;//完整的文件系统路径
    char *realpath;//实际的文件系统路径
    w_vfs_s *vfs;//关联的文件系统
    w_mutex_s *mutex;//文件锁
    w_file_s *childfile;//子文件,在需要遍历目录时使用
    w_uint16_t fmode;//打开模式
    
    void *fileobj;//实际文件对象
    w_uint8_t isdir;//是否是目录
    w_int32_t offset;//文件偏移位置
};


#define FS_OPS_DEF(fs) \
w_fsops_s fs##_ops = {\
{WIND_FSTYPE_MAGIC,#fs,{W_NULL,W_NULL},0,0},\
fs##_op_init,\
fs##_op_format,\
W_NULL,\
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
