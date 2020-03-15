/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: listfs_fileinfo.h
**创   建   人: Jason Zhou
**最后修改日期: 2019.04.05
**描        述: 文件系统对块设备的操作接口
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
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"
#if WIND_LISTFS_SUPPORT

#define LISTFILE_MAGIC 0x7D5349AC

#define LFILE_NAME_LEN 64    //文件名长度

//从fileinfo数据块中取出blkinfo
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]

//#define BLKINFO_HAS_OFFSET(info,ofst,blksize) ((ofst >= info->offset)&&(ofst < info->offset + info->blkused * blksize))




//固化文件头部信息
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //魔术字
    char       name[LFILE_NAME_LEN]; //文件名
    w_int32_t  filesize;             //文件大小
    w_int32_t  spacesize;            //文件空间大小
    w_uint32_t   parent_addr;          //父地址
    w_uint32_t   self_addr;            //当前地址
    w_uint32_t   last_addr;            //最后一个块信息地址
    w_uint32_t   prevfile_addr;        //下一个文件地址
    w_uint32_t   nextfile_addr;        //下一个文件地址
    w_int32_t  children_cnt;         //子文件的数量
    w_uint32_t   headchild_addr;       //第一个子文件地址
    w_uint32_t   tailchild_addr;       //最后一个文件地址
    w_uint8_t  attr;                 //是否目录，可读，可写，隐藏，校验
}lfile_info_s;

void fileinfo_be2le(lfile_info_s *info);
w_err_t fileinfo_init(lfile_info_s *info,char *name,
    w_uint32_t self_addr,w_uint32_t parent_addr,w_uint32_t prev_addr,w_uint8_t attr);

w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_uint32_t addr);

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_add_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_add_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_rm_update_next(lfile_info_s *info,w_blkdev_s *blkdev);

#endif
#endif
