/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : listfs_blkinfo.h
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Operation interface of file block information
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019.04.05
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2019.04.05
** Description : 
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

#define LFILE_NAME_LEN 64    //File name lenth

#define LFILE_LBLK_CNT 64    //Block address count in one fileinfo


#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]
//#define BLKINFO_HAS_OFFSET(info,offset) 
//((offset >= info->offset)&&(offset < info->offset + info->blkused * info->blksize))
#define BLKINFO_HAS_OFFSET(info,offset1,blksize) ((offset1 >= info->offset)&&(offset1 < info->offset + info->blkused * blksize))


typedef struct __lfile_blkinfo_s
{
    w_uint32_t magic;        //
    w_uint32_t self_addr;    //
    w_uint32_t prevblk_addr; //
    w_uint32_t nextblk_addr; //
    w_uint32_t  blksize;      //
    w_uint32_t  offset;       //
    w_uint32_t  blkused;      //
    w_uint32_t  byteused;     //
    w_uint32_t dataaddr[LFILE_LBLK_CNT];  //
}lfile_blkinfo_s;

void blkinfo_be2le(lfile_blkinfo_s *info);

w_int32_t blkinfo_get_used(lfile_blkinfo_s *info);

w_int32_t blkinfo_get_space(lfile_blkinfo_s *info);

w_err_t blkinfo_init(lfile_blkinfo_s *info,w_uint32_t self_addr,w_uint32_t prev_addr,w_int32_t offset,w_int32_t blksize);

w_err_t blkinfo_read(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_uint32_t addr);

w_err_t blkinfo_write(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_link(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_uint32_t *addr,w_int32_t count);

w_err_t blkinfo_unlink(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_tail(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_byoffset(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_uint32_t offset);

w_err_t blkinfo_update_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_alloc(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t count);

w_err_t blkinfo_free(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t count);

w_err_t blkinfo_add_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_uint32_t *addr,w_int32_t count);

w_err_t blkinfo_del_dataaddr(lfile_blkinfo_s *info,w_blkdev_s *blkdev,w_int32_t addridx,w_int32_t count);

w_uint32_t blkinfo_get_addr(lfile_blkinfo_s *info,w_int32_t offset);


#endif
#endif

