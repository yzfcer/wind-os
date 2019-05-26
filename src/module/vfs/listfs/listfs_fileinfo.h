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

#define LISTFS_MAGIC 0x49AC7D5349AC7D64
#define LISTFILE_MAGIC 0x7D5349AC
#define LISTFILE_BLK_MAGIC 0x725A4967

#define LFILE_NAME_LEN 64    //文件名长度

//从fileinfo数据块中取出blkinfo
#define FILEINFO_BLKINFO(blk) (lfile_blkinfo_s*)&blk[sizeof(lfile_info_s)]

#define BLKINFO_HAS_OFFSET(info,ofst,blksize) ((ofst >= info->offset)&&(ofst < info->offset + info->blkused * blksize))

//固化文件系统信息
typedef struct __lfs_info_s
{
    w_uint64_t magic;        //魔术字
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


//固化文件头部信息
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //魔术字
    char       name[LFILE_NAME_LEN]; //文件名
    w_int32_t  filesize;             //文件大小
    w_int32_t  spacesize;            //文件空间大小
    w_addr_t   parent_addr;          //父地址
    w_addr_t   self_addr;            //当前地址
    w_addr_t   last_addr;            //最后一个块信息地址
    w_addr_t   prevfile_addr;        //下一个文件地址
    w_addr_t   nextfile_addr;        //下一个文件地址
    w_addr_t   headchild_addr;       //第一个子文件地址
    w_addr_t   tailchild_addr;       //最后一个文件地址
    w_uint8_t  attr;                 //是否目录，可读，可写，隐藏，校验
}lfile_info_s;


w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr);

w_err_t fileinfo_read(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

w_err_t fileinfo_write(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_next(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_get_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

#endif

