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

#if WIND_LISTFS_SUPPORT
//文件操作模式
#define LFMODE_R   0x01
#define LFMODE_W   0x02
#define LFMODE_RW  0x03
#define LFMODE_CRT 0x04
#define LFMODE_A   0x08

#define LISTFS_MAGIC 0x49AC7D53

#define LISTFS_DIR_LAYCNT 32 //目录深度
//#define LISTFS_BLK_SIZE 512  //块大小

#define LISTFS_MAX_FILE_SIZE 0x7fffffff //文件长度限制，2GB

//文件属性
#define LFILE_ATTR_DIR    (0x01 << 0) //是否目录
#define LFILE_ATTR_RDEN   (0x01 << 1) //是否可读
#define LFILE_ATTR_WREN   (0x01 << 2) //是否可写
#define LFILE_ATTR_HIDE   (0x01 << 3) //是否隐藏
#define LFILE_ATTR_VERIFY (0x01 << 4) //是否校验
#define LFILE_ATTR_COMMAN (LFILE_ATTR_RDEN | LFILE_ATTR_WREN)

#define IS_LFILE_ATTR_DIR(attr) (attr & LFILE_ATTR_DIR)
#define SET_LFILE_ATTR_DIR(attr) (attr |= LFILE_ATTR_DIR)
#define CLR_LFILE_ATTR_DIR(attr) (attr &= (~LFILE_ATTR_DIR))

#define IS_LFILE_ATTR_RDEN(attr) (attr & LFILE_ATTR_RDEN)
#define SET_LFILE_ATTR_RDEN(attr) (attr |= LFILE_ATTR_RDEN)
#define CLR_LFILE_ATTR_RDEN(attr) (attr &= (~LFILE_ATTR_RDEN))

#define IS_LFILE_ATTR_WREN(attr) (attr & LFILE_ATTR_WREN)
#define SET_LFILE_ATTR_WREN(attr) (attr |= LFILE_ATTR_WREN)
#define CLR_LFILE_ATTR_WREN(attr) (attr &= (~LFILE_ATTR_WREN))

#define IS_LFILE_ATTR_HIDE(attr) (attr & LFILE_ATTR_HIDE)
#define SET_LFILE_ATTR_HIDE(attr) (attr |= LFILE_ATTR_HIDE)
#define CLR_LFILE_ATTR_HIDE(attr) (attr &= (~LFILE_ATTR_HIDE))

#define IS_LFILE_ATTR_VERIFY(attr) (attr & LFILE_ATTR_VERIFY)
#define SET_LFILE_ATTR_VERIFY(attr) (attr |= LFILE_ATTR_VERIFY)
#define CLR_LFILE_ATTR_VERIFY(attr) (attr &= (~LFILE_ATTR_VERIFY))



//固化文件系统信息
typedef struct __lfs_info_s
{
    w_uint32_t magic;        //魔术字
    w_uint32_t blkcount;     //块数量
    w_uint16_t unit_size;    //文件单位大小
    w_uint16_t blksize;      //块大小
    w_uint16_t reserve_blk;  //保留块数
    w_uint16_t attr;         //文件系统属性
    w_uint32_t bitmap_cnt;   //位图块数
    w_addr_t   bitmap1_addr; //主位图地址
    w_addr_t   bitmap2_addr; //备份位图地址
    w_addr_t   root_addr;    //根目录位置
}lfs_info_s;

//程序关联的文件系统信息
typedef struct __listfs_s
{
    lfs_info_s lfs_info;  //文件系统信息
    w_blkdev_s *blkdev;   //关联的块设备
    lfs_bitmap_s bitmap;  //块使用位图
    w_int32_t  file_ref;  //打开的文件数量
    w_uint32_t blkused;   //已经使用的块数量
}w_listfs_s;

//程序关联的文件信息
typedef struct __listfile_s
{
    lfile_info_s info;        //文件基本信息
    w_listfs_s *lfs;          //对应的文件系统
    w_uint8_t mode;           //打开模式
    w_int32_t offset;         //文件偏移量
    lfile_blkinfo_s *blkinfo; //当前数据块信息
    lfile_info_s *subinfo;    //子文件信息
}w_listfile_s;

void lfs_info_be2le(lfs_info_s *info);

w_err_t _wind_listfs_mod_init(void);
void *listfs_mem_malloc(w_int32_t size);
w_err_t listfs_mem_free(void *ptr);
w_err_t listfs_format(w_listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_init(w_listfs_s *lfs,w_blkdev_s *blkdev);
w_err_t listfs_deinit(w_listfs_s *lfs);
w_err_t listfs_match(w_blkdev_s *blkdev);

w_bool_t listfile_existing(w_listfs_s *lfs,const char *path);
w_listfile_s* listfile_open(w_listfs_s *lfs,const char *path,w_uint8_t mode);
w_err_t listfile_set_attr(w_listfile_s* file,w_uint8_t attr);
w_err_t listfile_get_attr(w_listfile_s* file,w_uint8_t *attr);
w_err_t listfile_close(w_listfile_s* file);
w_err_t listfile_remove(w_listfs_s *lfs,const char *path);
w_err_t listfile_seek(w_listfile_s* file,w_int32_t offset);
w_int32_t listfile_ftell(w_listfile_s* file);
w_int32_t listfile_read(w_listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_int32_t listfile_write(w_listfile_s* file,w_uint8_t *buff, w_int32_t size);
w_err_t listfile_fgets(w_listfile_s* file,char *buff, w_int32_t maxlen);
w_err_t listfile_fputs(w_listfile_s* file,char *buff);

w_err_t listfile_readdir(w_listfile_s* file,w_listfile_s** sub);


#endif
#endif
