#ifndef WIND_FILE_H__
#define WIND_FILE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
//#include "wind_fs.h"

#if WIND_FS_SUPPORT
#define WIND_FS_MAGIC 0x235C79A5
#define WIND_FILE_MAGIC 0x275972D5
#define FS_NAME_LEN 12
#define FS_MOUNT_PATH_LEN 64
#define FS_CUR_PATH "/"
typedef struct __fs_ops_s fs_ops_s;
typedef struct __fs_s fs_s;
typedef struct __file_s file_s;
typedef enum 
{
    FSTYPE_TREEFS = 0x01,
    FSTYPE_UNDEF = 0xff,
}fstype_e;


struct __fs_s
{
    w_uint32_t magic;
    char *name;
    char *mount_path;
    dnode_s fsnode;
    fstype_e fstype;
    blkdev_s *blkdev;
    fs_ops_s *ops;
};


typedef enum
{
    FMODE_R = 0x01,
    FMODE_W = 0x02,
    FMODE_RW = 0x03,
    FMODE_CRT = 0x04,
    FMODE_A = 0x08,
}fmode_e;

typedef enum 
{
    FTYPE_DIR = 0x01,
    FTYPE_FILE = 0x02,
}ftype_e;

struct __fs_ops_s
{
    //w_err_t (*mount)(fs_s *fs);
    w_err_t (*init)(fs_s *fs);
    w_err_t (*format)(fs_s *fs);
    //w_err_t (*mkfile)(const char *path);
    w_err_t (*open)(file_s *file,fmode_e fmode);
    w_err_t (*close)(file_s* file);
    w_err_t (*rmfile)(file_s* file);
    file_s *(*subfile)(file_s* file,w_int32_t index);
    w_err_t (*seek)(file_s* file,w_int32_t offset);
    w_err_t (*rename)(file_s* file,char *newname);
    w_int32_t (*ftell)(file_s* file);
    w_int32_t (*read)(file_s* file,w_uint8_t *buff, w_int32_t size);
    w_int32_t (*write)(file_s* file,w_uint8_t *buff, w_int32_t size);
    w_err_t (*fgets)(file_s* file,char *buff, w_int32_t maxlen);
    w_err_t (*fputs)(file_s* file,char *buff);
};




struct __file_s
{
    //char name[32];
    char *path;
    dnode_s filenode;//链表节点
    w_uint32_t ftype:8;//文件系统类型
    w_uint32_t fmode:16;//操作模式
    w_uint32_t isdir:1;
    fs_s *fs;
    void *fileobj;//文件对象
    w_int32_t offset;//偏移量
    mutex_s *mutex;//文件操作变量
    fs_ops_s *ops;//操作函数集
};

#define FS_OPS_DEF(fs) \
static fs_ops_s fs_ops = {\
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

#define WIND_FS_DEF(name,type,ops) \
{WIND_FS_MAGIC,#name,NULL,{NULL,NULL},type,NULL,&ops}

w_err_t _wind_fs_init(void);
fs_s *wind_fs_get(char *name);
w_err_t wind_fs_mount(char *fsname,char *devname,char *path);
w_err_t wind_fs_unmount(char *fsname);
w_err_t wind_fs_format(fs_s *fs);

char *wind_file_get_full_path(char *oldpath,char *newpath,w_uint16_t isdir);
void wind_file_set_current_path(char *path);
char *wind_file_get_current_path(void);


w_bool_t wind_file_existing(const char *path);
file_s* wind_file_open(const char *path,fmode_e fmode);
w_err_t wind_file_close(file_s *file);
w_err_t wind_file_remove(file_s *file);
file_s* wind_file_subfile(file_s *file,w_int32_t index);

w_err_t wind_file_seek(file_s *file,w_int32_t offset);
w_err_t wind_file_rename(file_s *file,char *newname);
w_int32_t wind_file_tell(file_s *file);
w_int32_t wind_file_read(file_s *file,w_uint8_t *buff, w_int32_t size);
w_int32_t wind_file_write(file_s *file,w_uint8_t *buff, w_int32_t size);
w_err_t wind_file_gets(file_s *file,char *buff, w_int32_t maxlen);
w_err_t wind_file_puts(file_s *file,char *buff);
#endif
#endif

