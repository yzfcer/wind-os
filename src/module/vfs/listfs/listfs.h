/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : listfs.h
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : Main functions of listfs file system
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
//Open mode
#define LFMODE_R   0x01
#define LFMODE_W   0x02
#define LFMODE_RW  0x03
#define LFMODE_CRT 0x04
#define LFMODE_A   0x08

#define LISTFS_MAGIC 0x49AC7D53

#define LISTFS_DIR_LAYCNT 32 //Catalog depth
//#define LISTFS_BLK_SIZE 512  //Block size

#define LISTFS_MAX_FILE_SIZE 0x7fffffff //File length limit，2GB

//File properties
#define LFILE_ATTR_DIR    (0x01 << 0) //Directory or not
#define LFILE_ATTR_RDEN   (0x01 << 1) //Readable or not
#define LFILE_ATTR_WREN   (0x01 << 2) //Writable or not
#define LFILE_ATTR_HIDE   (0x01 << 3) //Hide or not
#define LFILE_ATTR_VERIFY (0x01 << 4) //Check or not
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



//Solidified file system information
typedef struct __lfs_info_s
{
    w_uint32_t magic;        //Magic word
    w_uint32_t blkcount;     //Number of blocks
    w_uint16_t unit_size;    //File unit size
    w_uint16_t blksize;      //Block size
    w_uint16_t reserve_blk;  //Number of reserved blocks
    w_uint16_t attr;         //File system properties
    w_uint32_t bitmap_cnt;   //Number of bitmap blocks
    w_uint32_t   bitmap1_addr; //Primary bitmap address
    w_uint32_t   bitmap2_addr; //Backup bitmap address
    w_uint32_t   root_addr;    //Root address
}lfs_info_s;

//File system information associated with the program
typedef struct __listfs_s
{
    lfs_info_s lfs_info;  //file system information
    w_blkdev_s *blkdev;   //Associated block device
    lfs_bitmap_s bitmap;  //Block usage bitmap
    w_int32_t  file_ref;  //Number of open files
    w_uint32_t blkused;   //Number of blocks used
}w_listfs_s;

//File information associated with program
typedef struct __listfile_s
{
    lfile_info_s info;        //Basic information of documents
    w_listfs_s *lfs;          //Corresponding file system
    w_uint8_t mode;           //Open mode
    w_uint32_t offset;         //File offset
    lfile_blkinfo_s *blkinfo; //Current block information
    lfile_info_s *subinfo;    //Subfile information
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

w_err_t listfile_readdir(w_listfile_s* file,w_listfile_s** sub);


#endif
#endif
