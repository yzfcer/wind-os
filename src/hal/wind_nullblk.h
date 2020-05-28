/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : wind_nullblk.h
** Author      : Jason Zhou
** Last Date   : 
** Description : Create an interface for an null block device
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
w_err_t wind_nullblk_create(w_blkdev_s *blkdev,char *name);
w_err_t wind_nullblk_destroy(w_blkdev_s *blkdev);
#endif

#ifdef __cplusplus
}
#endif


#endif

