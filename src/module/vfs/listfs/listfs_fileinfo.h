#ifndef LISTFS_FILEINFO_H__
#define LISTFS_FILEINFO_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"

#define LISTFS_MAGIC 0x49AC7D5349AC7D64
#define LISTFILE_MAGIC 0x7D5349AC
#define LISTFILE_BLK_MAGIC 0x725A4967

#define LFILE_NAME_LEN 64    //文件名长度

#define LFILE_LBLK_CNT 64    //每个块允许记录的关联块数量


//固化文件系统信息
typedef struct __lfs_info_s
{
    w_uint64_t magic;      //魔术字
    w_uint32_t blkcount;   //块数量
    w_uint16_t unit_size;  //文件单位大小
    w_uint16_t blksize;    //块大小
    w_uint16_t reserve_blk;//保留块数
    w_uint16_t attr;       //文件系统属性
    w_uint32_t bitmap_cnt; //位图块数
    w_addr_t   bitmap1;    //位图块数
    w_addr_t   bitmap2;    //位图块数
    w_addr_t   root_addr;  //根目录位置
}lfs_info_s;


//文件数据块头部信息
typedef struct __lfile_blkinfo_s
{
    w_uint32_t magic;     //块标记魔术字
    w_addr_t   self_addr;    //当前地址
    w_addr_t   prevblk_addr; //上块地址
    w_addr_t   nextblk_addr; //下块地址
    w_int32_t  offset;       //当前块对应的文件的起始偏移量
    w_int32_t  blkused;      //当前块已经使用的数量
    w_int32_t  byteused;     //当前块已经使用的字节数量
}lfile_blkinfo_s;

//固化文件头部信息
typedef struct __lfile_info_s
{
    w_uint32_t magic;                //魔术字
    char       name[LFILE_NAME_LEN]; //文件名
    w_addr_t   parent_addr;          //父地址
    w_addr_t   prevfile_addr;        //下一个文件地址
    w_addr_t   nextfile_addr;        //下一个文件地址
    w_addr_t   headchild_addr;       //第一个子文件地址
    w_addr_t   tailchild_addr;       //最后一个文件地址
    w_uint8_t  attr;                 //是否目录，可读，可写，隐藏，校验
    lfile_blkinfo_s blkinfo;
}lfile_info_s;

w_err_t listfs_get_fsinfo(lfs_info_s *fsinfo,w_blkdev_s *blkdev);

w_err_t listfs_fileinfo_init(lfile_info_s *info,char *name,
    w_addr_t self_addr,w_addr_t parent_addr,w_addr_t prev_addr,w_uint8_t attr);

w_err_t listfs_read_block(w_blkdev_s *blkdev,w_addr_t addr,w_uint8_t **blk);

w_err_t listfs_get_fileinfo(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

w_err_t listfs_set_fileinfo(lfile_info_s *info,w_blkdev_s *blkdev,w_addr_t addr);

lfile_info_s *fileinfo_parent(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_next(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_headchild(lfile_info_s *info,w_blkdev_s *blkdev);

lfile_info_s *fileinfo_tailchild(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_parent(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t fileinfo_update_prev(lfile_info_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_prev(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

w_err_t blkinfo_get_next(lfile_blkinfo_s *info,w_blkdev_s *blkdev);

#endif

