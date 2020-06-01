/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : fs_def.h
** Author      : Jason Zhou
** Last Date   : 2019/1/24 20:24:37
** Description : VFS structure definition
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
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

#define FMODE_R    0x01
#define FMODE_W    0x02
#define FMODE_RW   0x03
#define FMODE_CRT  0x04
#define FMODE_A    0x08

#define F_VFS_POOL (0x01 << 0) //Mark whether the vfs object is enabled
#define IS_F_VFS_POOL(fs) ((fs->obj.flag & F_VFS_POOL) == F_VFS_POOL)
#define SET_F_VFS_POOL(fs) (fs->obj.flag |= F_VFS_POOL)
#define CLR_F_VFS_POOL(fs) (fs->obj.flag &= (~F_VFS_POOL))

#define F_VFS_MOUNT (0x01 << 1) //Mark whether the FS object has been mounted
#define IS_F_VFS_MOUNT(vfs) ((vfs->obj.flag & F_VFS_MOUNT) == F_VFS_MOUNT)
#define SET_F_VFS_MOUNT(vfs) (vfs->obj.flag |= F_VFS_MOUNT)
#define CLR_F_VFS_MOUNT(vfs) (vfs->obj.flag &= (~F_VFS_MOUNT))


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
    void *usr_arg;
    w_blkdev_s *blkdev;
    w_fsops_s *ops;
};

struct __w_fsops_s
{
    w_obj_s obj;
    w_err_t (*opsinit)(void);//File system operation set initialization
    void*   (*init)(w_vfs_s *fs);//File system type initialization
    w_err_t (*deinit)(w_vfs_s *fs);//File system type uninitialization
    w_err_t (*format)(w_vfs_s *fs);//File system format
    w_err_t (*matchfs)(char *devname);//Check if the block device file system matches
    
    w_err_t (*open)(w_file_s *file,w_uint8_t fmode);
    w_err_t (*close)(w_file_s* file);
    w_err_t (*remove)(w_file_s* file);
    w_err_t (*readdir)(w_file_s* dir,w_file_s* child);
    w_err_t (*seek)(w_file_s* file,w_uint32_t offset);
    w_err_t (*rename)(w_file_s* file,char *newname);
    w_int32_t (*ftell)(w_file_s* file);
    w_int32_t (*read)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
    w_int32_t (*write)(w_file_s* file,w_uint8_t *buff, w_int32_t size);
};

struct __w_file_s
{
    w_obj_s obj;
    char *fullpath;//Full file system path
    char *realpath;//Actual file system path
    w_vfs_s *vfs;//Associated file system
    w_mutex_s *mutex;//File lock
    w_file_s *childfile;//Child files, used when traversing directories
    
    void *fileobj;//Actual file object
    w_uint8_t fmode;//Open mode
    w_uint8_t isdir;//Is it a directory
    w_uint32_t offset;//File offset
};


#define FS_OPS_DEF(fs) \
w_fsops_s fs##_ops = {\
WIND_OBJ(WIND_FSTYPE_MAGIC,0,#fs),\
fs##_op_opsinit,\
fs##_op_init,\
fs##_op_deinit,\
fs##_op_format,\
fs##_op_matchfs,\
fs##_op_open,\
fs##_op_close,\
fs##_op_remove,\
fs##_op_readdir,\
fs##_op_seek,\
fs##_op_rename,\
fs##_op_ftell,\
fs##_op_read,\
fs##_op_write \
}

#endif
