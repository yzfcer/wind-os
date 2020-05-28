/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : treefs_def.h
** Author      : Jason Zhou
** Last Date   : 2019.04.05
** Description : The definition of data structure of trees file system
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
#ifndef TREEFS_DEF_H__
#define TREEFS_DEF_H__
#include "wind_config.h"
#include "wind_type.h"
#include "wind_obj.h"
#include "wind_tree.h"
#if WIND_TREEFS_SUPPORT

#define TF_FMODE_R  0x01
#define TF_FMODE_W  0x02
#define TF_FMODE_RW  0x03
#define TF_FMODE_CRT 0x04
#define TF_FMODE_A  0x08


#define TREEFILE_MAGIC 0x48A97D26
#define TREEFS_MAGIC 0x26F4AD56
#define TREEFS_DIR_LAYCNT 32

#define TREEFS_BLK_SIZE 128

#define F_TREEFILE_DIR (0x01 << 0) //标记treefile对象是否是一个目录
#define IS_F_TREEFILE_DIR(treefile) ((treefile->attr & F_TREEFILE_DIR) == F_TREEFILE_DIR)
#define SET_F_TREEFILE_DIR(treefile) (treefile->attr |= F_TREEFILE_DIR)
#define CLR_F_TREEFILE_DIR(treefile) (treefile->attr &= (~F_TREEFILE_DIR))

typedef struct __treefs_s w_treefs_s;
typedef struct __treefile_s w_treefile_s;

struct __treefile_s
{
    w_uint32_t magic;
    w_tree_s tree;
    char *filename;
    w_treefs_s *tfs;
    w_uint8_t isdir:1;
    w_uint8_t mode;
    w_uint32_t offset;
    w_uint32_t filelen;
    w_uint32_t bufflen;
    w_dlist_s datalist;
};

struct __treefs_s
{
    w_obj_s obj;
    w_treefile_s *root;
    w_int32_t fs_size;
};



#endif //#if WIND_MODULE_VFS_SUPPORT
#endif //#ifndef TREEFS_DEF_H__
