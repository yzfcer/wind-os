#ifndef WIND_FS_H__
#define WIND_FS_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#if WIND_FS_SUPPORT
#define FS_NAME_LEN 12
#define FS_MOUNT_PATH_LEN 64
typedef struct __fs_s fs_s;
typedef struct __fs_ops_s fs_ops_s;

typedef enum 
{
    FSTYPE_TREEFS = 0x01,
    FSTYPE_UNDEF = 0xff,
}fstype_e;

struct __fs_ops_s
{
    w_err_t (*mount)(fs_s *fs);
    w_err_t (*unmount)(fs_s *fs);
    w_err_t (*format)(fs_s *fs);
};

struct __fs_s
{
    char *name;
    char *path;
    dnode_s fsnode;
    fstype_e fstype;
    blkdev_s *dev;
    fs_ops_s *ops;
};

w_err_t _wind_fs_init(void);
fs_s *wind_fs_get(char *name);
w_err_t wind_fs_mount(char *fsname,char *blkname,char *path);
w_err_t wind_fs_unmount(char *fsname);
w_err_t wind_fs_format(char *fsname);

#endif
#endif

