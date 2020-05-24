/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : tb_model_file.c
** Author      : Jason Zhou
** Last Date   : 2020.3.9
** Description : 数据表模型框架序列化功能函数即
**              
**--------------History---------------------------------------------------------------------------------
Author: 
** Version     : v1.0
** Date        : 2020.3.9
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2020.3.9
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_type.h"
#include "db_if.h"
#include "wind_debug.h"
#ifndef TB_MODEL_FILE_H__
#define TB_MODEL_FILE_H__

w_err_t tbmodel_file_init(void);
w_err_t tbmodel_file_load(char *filepath);
w_err_t tbmodel_file_load_all(char *dirpath);

w_err_t tbmodel_file_save(char *filepath);
w_err_t tbmodel_file_save_all(char *dirpath);

#endif

