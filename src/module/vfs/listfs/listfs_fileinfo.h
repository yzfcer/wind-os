/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : listfs_fileinfo.h
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Operation interface of file information
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
#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"
#if WIND_LISTFS_SUPPORT

#define LISTFILE_MAGIC 0x7D5349AC

#define LFILE_NAME_LEN 64    //File name length

//Get blkinfo from FileInfo data block
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]

//#define BLKINFO_HAS_OFFSET(info,ofst,blksize) ((ofst >= info->offset)&&(ofst < info->offset + info->blkused * blksize))




//Solidified file header information
typedef struct __lfile_info_s
{
    w_uint32_t   magic;                //Magic word
    char         name[LFILE_NAME_LEN]; //file name
    w_int32_t    filesize;             //file size
    w_int32_t    spacesize;            //File space size
    w_uint32_t   parent_addr;          //Parent file address
    w_uint32_t   self_addr;            //Current address
    w_uint32_t   last_addr;            //Last block information address
    w_uint32_t   prevfile_addr;        //Next file address
    w_uint32_t   nextfile_addr;        //Next file address
    w_int32_t    children_cnt;         //Number of sub files
    w_uint32_t   headchild_addr;       //First subfile address
    w_uint32_t   tailchild_addr;       //Last subfile address
    w_uint8_t    attr;                 //Directory, readable, writable, hidden, and verified
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
