#ifndef WIND_LISTFS_H__
#define WIND_LISTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "listfs_fileinfo.h"

#if WIND_FS_SUPPORT
//文件操作模式
#define LF_FMODE_R   0x01
#define LF_FMODE_W   0x02
#define LF_FMODE_RW  0x03
#define LF_FMODE_CRT 0x04
#define LF_FMODE_A   0x08


#define LISTFS_DIR_LAYCNT 32 //目录深度
//#define LISTFS_BLK_SIZE 512  //块大小

//文件属性
#define LFILE_ATTR_DIR    (0x01 << 0)
#define LFILE_ATTR_READ   (0x01 << 1)
#define LFILE_ATTR_WRITE  (0x01 << 2)
#define LFILE_ATTR_HIDE   (0x01 << 3)
#define LFILE_ATTR_VERIFY (0x01 << 4)
#define LFILE_ATTR_COMMAN (LFILE_ATTR_READ | LFILE_ATTR_WRITE)



//程序关联的文件系统信息
typedef struct __listfs_s
{
    lfs_info_s lfs_info;
    w_blkdev_s *blkdev;
    w_int32_t  file_ref;
    w_int32_t  free_blkidx;
    w_int32_t  free_byteidx;
}listfs_s;

//程序关联的文件信息
typedef struct __listfile_s
{
    lfile_info_s info;
    listfs_s *lfs;
    w_uint8_t mode;
    w_int32_t offset;
    w_int32_t filelen;
    lfile_blkinfo_s *blkinfo;
}listfile_s;


void *lfs_malloc(w_int32_t size);
w_err_t lfs_free(void *ptr);

w_err_t listfs_format(listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_mount(listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_unmount(listfs_s *lfs);

w_bool_t listfile_existing(listfs_s *lfs,const char *path);
listfile_s* listfile_open(listfs_s *lfs,const char *path,w_uint16_t mode);
w_err_t listfile_set_attr(listfile_s* file,w_uint8_t attr);
w_err_t listfile_get_attr(listfile_s* file,w_uint8_t *attr);
w_err_t listfile_close(listfile_s* file);
w_err_t listfile_remove(listfile_s *file);
w_err_t listfile_seek(listfile_s* file,w_int32_t offset);
w_int32_t listfile_ftell(listfile_s* file);
w_int32_t listfile_read(listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff, w_int32_t size);
listfile_s *listfile_readdir(listfile_s* file,w_int32_t index);
w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen);
w_err_t listfile_fputs(listfile_s* file,char *buff);

#endif
#endif
