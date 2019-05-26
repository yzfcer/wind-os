/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: listfs文件系统主体功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.04.05
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.04.05
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_LISTFS_H__
#define WIND_LISTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "listfs_fileinfo.h"
#include "listfs_blkinfo.h"
#include "listfs_bitmap.h"

#if WIND_FS_SUPPORT
//文件操作模式
#define LFMODE_R   0x01
#define LFMODE_W   0x02
#define LFMODE_RW  0x03
#define LFMODE_CRT 0x04
#define LFMODE_A   0x08


#define LISTFS_DIR_LAYCNT 32 //目录深度
//#define LISTFS_BLK_SIZE 512  //块大小

#define LISTFS_MAX_FILE_SIZE 0x7fffffff //文件长度限制，2GB

//文件属性
#define LFILE_ATTR_DIR    (0x01 << 0)
#define LFILE_ATTR_READ   (0x01 << 1)
#define LFILE_ATTR_WRITE  (0x01 << 2)
#define LFILE_ATTR_HIDE   (0x01 << 3)
#define LFILE_ATTR_VERIFY (0x01 << 4)
#define LFILE_ATTR_COMMAN (LFILE_ATTR_READ | LFILE_ATTR_WRITE)

#define LFILE_IS_DIR(attr) (attr & LFILE_ATTR_DIR)

//程序关联的文件系统信息
typedef struct __listfs_s
{
    lfs_info_s lfs_info;  //文件系统信息
    w_blkdev_s *blkdev;   //关联的块设备
    lfs_bitmap_s bitmap;  //块使用位图
    
    lfile_info_s root;    //根目录信息
    w_int32_t  file_ref;  //打开的文件数量
    w_uint32_t blkused;   //已经使用的块数量

}listfs_s;

//程序关联的文件信息
typedef struct __listfile_s
{
    lfile_info_s info;
    listfs_s *lfs;
    w_uint8_t mode;
    w_int32_t offset;
    lfile_info_s *subinfo;
    lfile_blkinfo_s *blkinfo;
}listfile_s;


void *lfs_malloc(w_int32_t size);
w_err_t lfs_free(void *ptr);

w_err_t listfs_format(listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_init(listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_deinit(listfs_s *lfs);
w_err_t listfile_destroy(listfile_s* file);

w_bool_t listfile_existing(listfs_s *lfs,const char *path);
listfile_s* listfile_open(listfs_s *lfs,const char *path,w_uint16_t mode);
w_err_t listfile_set_attr(listfile_s* file,w_uint8_t attr);
w_err_t listfile_get_attr(listfile_s* file,w_uint8_t *attr);
w_err_t listfile_close(listfile_s* file);
w_err_t listfile_remove(listfs_s *lfs,const char *path);
w_err_t listfile_seek(listfile_s* file,w_int32_t offset);
w_int32_t listfile_ftell(listfile_s* file);
w_int32_t listfile_read(listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_int32_t listfile_write(listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_err_t listfile_fgets(listfile_s* file,char *buff, w_int32_t maxlen);
w_err_t listfile_fputs(listfile_s* file,char *buff);

listfile_s *listfile_readdir(listfile_s* file,w_int32_t index);


#endif
#endif
