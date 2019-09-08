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
#ifndef LISTFS_BLKINFO_H__
#define LISTFS_BLKINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"
#if WIND_LISTFS_SUPPORT

#define LISTFILE_BLK_MAGIC 0x725A4967

#define LFILE_NAME_LEN 64    //文件名长度

#define LFILE_LBLK_CNT 64    //每个块允许记录的关联块数量

//从fileinfo数据块中取出blkinfo
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]
//#define BLKINFO_HAS_OFFSET(info,offset) \
//((offset >= info->offset)&&(offset < info->offset + info->blkused * info->blksize))
#define BLKINFO_HAS_OFFSET(info,offset1,blksize) ((offset1 >= info->offset)&&(offset1 < info->offset + info->blkused * blksize))

//文件数据块头部信息
typedef struct __lfile_blkinfo_s
{
    w_uint32_t magic;        //块标记魔术字
    w_addr_t   self_addr;    //当前地址
    w_addr_t   prevblk_addr; //上块地址
    w_addr_t   nextblk_addr; //下块地址
    w_int32_t  blksize;      //块大小
    w_int32_t  offset;       //当前块对应的文件的起始偏移量
    w_int32_t  blkused;      //当前块已经使用的数量
    w_int32_t  byteused;     //当前块已经使用的字节数量
    w_addr_t   dataaddr[LFILE_LBLK_CNT];  //数据块信息
}lfile_blkinfo_s;

void blkinfo_be2le(lfile_blkinfo_s *info);

w_int32_t blkinfo_get_used(lfile_blkinfo_s *info);

w_int32_t blkinfo_get_space(lfile_blkinfo_s *info);

w_err_t blkinfo_init(lfile_blkinfo_s *info,w_addr_t self_addr,w_addr_t prev_addr,w_int32_t offset,w_int32_t blksize);

w_err_t blkinfo_read(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t addr);

w_err_t blkinfo_write(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_link(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count);

w_err_t blkinfo_unlink(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_tail(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_byoffset(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t offset);

w_err_t blkinfo_update_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_alloc(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t count);

w_err_t blkinfo_free(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t count);

w_err_t blkinfo_add_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_addr_t *addr,w_int32_t count);

w_err_t blkinfo_del_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t addridx,w_int32_t count);

w_addr_t blkinfo_get_addr(lfile_blkinfo_s *info,w_int32_t offset);


#endif
#endif

