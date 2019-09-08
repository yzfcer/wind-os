/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: hostfs.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.09.08
**描        述: hostfs宿主机文件系统主题功能
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2019.09.08
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2019.09.08
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_HOSTFS_H__
#define WIND_HOSTFS_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_dlist.h"
#include "wind_blkdev.h"
#include "hostfs_fileinfo.h"
#include "hostfs_blkinfo.h"
#include "hostfs_bitmap.h"

#if WIND_MODULE_VFS_SUPPORT
//文件操作模式
#define HMODE_R   0x01
#define HMODE_W   0x02
#define HMODE_RW  0x03
#define HMODE_CRT 0x04
#define HMODE_A   0x08

#define HOSTFS_MAGIC 0x49AC7D53

#define HOSTFS_DIR_LAYCNT 32 //目录深度
//#define HOSTFS_BLK_SIZE 512  //块大小

#define HOSTFS_MAX_FILE_SIZE 0x7fffffff //文件长度限制，2GB

//文件属性
#define HFILE_ATTR_DIR    (0x01 << 0) //是否目录
#define HFILE_ATTR_RDEN   (0x01 << 1) //是否可读
#define HFILE_ATTR_WREN   (0x01 << 2) //是否可写
#define HFILE_ATTR_HIDE   (0x01 << 3) //是否隐藏
#define HFILE_ATTR_VERIFY (0x01 << 4) //是否校验
#define HFILE_ATTR_COMMAN (HFILE_ATTR_RDEN | HFILE_ATTR_WREN)

#define IS_HFILE_ATTR_DIR(attr) (attr & HFILE_ATTR_DIR)
#define SET_HFILE_ATTR_DIR(attr) (attr |= HFILE_ATTR_DIR)
#define CLR_HFILE_ATTR_DIR(attr) (attr &= (~HFILE_ATTR_DIR))

#define IS_HFILE_ATTR_RDEN(attr) (attr & HFILE_ATTR_RDEN)
#define SET_HFILE_ATTR_RDEN(attr) (attr |= HFILE_ATTR_RDEN)
#define CLR_HFILE_ATTR_RDEN(attr) (attr &= (~HFILE_ATTR_RDEN))

#define IS_HFILE_ATTR_WREN(attr) (attr & HFILE_ATTR_WREN)
#define SET_HFILE_ATTR_WREN(attr) (attr |= HFILE_ATTR_WREN)
#define CLR_HFILE_ATTR_WREN(attr) (attr &= (~HFILE_ATTR_WREN))

#define IS_HFILE_ATTR_HIDE(attr) (attr & HFILE_ATTR_HIDE)
#define SET_HFILE_ATTR_HIDE(attr) (attr |= HFILE_ATTR_HIDE)
#define CLR_HFILE_ATTR_HIDE(attr) (attr &= (~HFILE_ATTR_HIDE))

#define IS_HFILE_ATTR_VERIFY(attr) (attr & HFILE_ATTR_VERIFY)
#define SET_HFILE_ATTR_VERIFY(attr) (attr |= HFILE_ATTR_VERIFY)
#define CLR_HFILE_ATTR_VERIFY(attr) (attr &= (~HFILE_ATTR_VERIFY))



//固化文件系统信息
typedef struct __hfs_info_s
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
}hfs_info_s;

//程序关联的文件系统信息
typedef struct __hostfs_s
{
    hfs_info_s hfs_info;  //文件系统信息
    w_blkdev_s *blkdev;   //关联的块设备
    hfs_bitmap_s bitmap;  //块使用位图
    w_int32_t  file_ref;  //打开的文件数量
    w_uint32_t blkused;   //已经使用的块数量
}w_hostfs_s;

//程序关联的文件信息
typedef struct __hostfile_s
{
    hfile_info_s info;        //文件基本信息
    w_hostfs_s *lfs;          //对应的文件系统
    w_uint8_t mode;           //打开模式
    w_int32_t offset;         //文件偏移量
    hfile_blkinfo_s *blkinfo; //当前数据块信息
    hfile_info_s *subinfo;    //子文件信息
}w_hostfile_s;

void hfs_info_be2le(hfs_info_s *info);

w_err_t _wind_hostfs_mod_init(void);
void *hostfs_mem_malloc(w_int32_t size);
w_err_t hostfs_mem_free(void *ptr);
w_err_t hostfs_format(w_hostfs_s *lfs,w_blkdev_s *blkdev);
w_err_t hostfs_init(w_hostfs_s *lfs,w_blkdev_s *blkdev);
w_err_t hostfs_deinit(w_hostfs_s *lfs);
w_err_t hostfs_match(w_blkdev_s *blkdev);

w_bool_t hostfile_existing(w_hostfs_s *lfs,const char *path);
w_hostfile_s* hostfile_open(w_hostfs_s *lfs,const char *path,w_uint8_t mode);
w_err_t hostfile_set_attr(w_hostfile_s* file,w_uint8_t attr);
w_err_t hostfile_get_attr(w_hostfile_s* file,w_uint8_t *attr);
w_err_t hostfile_close(w_hostfile_s* file);
w_err_t hostfile_remove(w_hostfs_s *lfs,const char *path);
w_err_t hostfile_seek(w_hostfile_s* file,w_int32_t offset);
w_int32_t hostfile_ftell(w_hostfile_s* file);
w_int32_t hostfile_read(w_hostfile_s* file,w_uint8_t *buff, w_int32_t size);
w_int32_t hostfile_write(w_hostfile_s* file,w_uint8_t *buff, w_int32_t size);
w_err_t hostfile_fgets(w_hostfile_s* file,char *buff, w_int32_t maxlen);
w_err_t hostfile_fputs(w_hostfile_s* file,char *buff);

w_err_t hostfile_readdir(w_hostfile_s* file,w_hostfile_s** sub);


#endif
#endif
