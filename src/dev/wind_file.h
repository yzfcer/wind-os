#ifndef WIND_FILE_H__
#define WIND_FILE_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_fs.h"
#if WIND_FS_SUPPORT
typedef struct __file_ops_s file_ops_s;
typedef struct __file_s file_s;

typedef enum
{
    FMODE_R = 0x01,
    FMODE_W = 0x02,
    FMODE_RW = 0x03,
    FMODE_CRT = 0x04,
    FMODE_A = 0x04,
}fmode_e;

typedef enum 
{
    FTYPE_DIR = 0x01,
    FTYPE_FILE = 0x02,
}ftype_e;

typedef enum 
{
    FSTYPE_TREEFS = 0x01,
    FSTYPE_UNDEF = 0xff,
}fstype_e;

typedef struct 
{
    char name[32];
    dnode_s filenode;//链表节点
    ftype_e ftype;//文件系统类型
    fmode_e fmode;//操作模式
    //w_bool_t opened;
    fs_s *fs;
    void *fileobj;//文件对象
    w_int32_t offset;//偏移量
    mutex_s *mutex;//文件操作变量
    void *fileobj;
    fs_ops_s *ops;//操作函数集
}file_s;


struct __file_ops_s
{
    w_err_t (*mkdir)(const char *path);
    file_s* (*open)(const char *path,fmode_e fmode);
    w_err_t (*close)(file_s* file);
    w_err_t (*seek)(file_s* file,w_int32_t offset);
    w_int32_t (*ftell)(file_s* file);
    w_int32_t (*read)(file_s* file,char *buff, w_int32_t size);
    w_int32_t (*write)(file_s* file,char *buff, w_int32_t size);
    w_err_t (*fgets)(file_s* file,char *buff, w_int32_t maxlen);
    w_err_t (*fputs)(file_s* file,char *buff);
};

file_s *_file_malloc(void);
w_err_t _file_free(file_s *file);

w_err_t _wind_file_init(void);
fs_s *wind_fs_get(char *name);

file_s* wind_file_open(const char *path,fmode_e fmode);
w_err_t wind_file_close(file_s *file);
w_err_t wind_file_seek(file_s *file,w_int32_t offset);
w_int32_t wind_file_tell(file_s *file);
w_int32_t wind_file_read(file_s *file,char *buff, w_int32_t size);
w_int32_t wind_file_write(file_s *file,char *buff, w_int32_t size);
w_err_t wind_file_gets(file_s *file,char *buff, w_int32_t maxlen);
w_err_t wind_file_puts(file_s *file,char *buff);

#endif
#endif

