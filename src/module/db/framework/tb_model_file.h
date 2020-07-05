/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : tb_model_file.c
** Author      : Jason Zhou
** Last Date   : 2020.3.9
** Description : Datasheet model framework serialization function set header
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
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
#ifndef TB_MODEL_FILE_H__
#define TB_MODEL_FILE_H__
#include "wind_type.h"
#include "db_if.h"
#include "wind_debug.h"
#if WIND_MODULE_DB_SUPPORT

w_err_t tbmodel_file_init(void);
w_err_t tbmodel_file_load(char *filepath);
w_err_t tbmodel_file_load_all(char *dirpath);

w_err_t tbmodel_file_save(char *filepath);
w_err_t tbmodel_file_save_all(char *dirpath);

#endif //#if WIND_MODULE_DB_SUPPORT
#endif

