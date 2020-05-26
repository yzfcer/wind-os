/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_memblk.h
** Author      : Jason Zhou
** Last Date   : 
** Description : 创建内存块设备的接口
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef WIND_MEMBLK_H__
#define WIND_MEMBLK_H__

#include "wind_config.h"
#include "wind_type.h"
#include "wind_blkdev.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WIND_BLKDEV_SUPPORT
w_err_t wind_memblk_create(w_blkdev_s *blkdev,char *name,void *mem,w_int32_t size,w_int32_t blksize);
w_err_t wind_memblk_destroy(w_blkdev_s *blkdev);
#endif

#ifdef __cplusplus
}
#endif


#endif

