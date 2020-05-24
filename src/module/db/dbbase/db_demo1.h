/****************************************Copyright (c)**************************************************
**                                       God's harbor
**FileName: tb_demo1.h
**Author: Jason Zhou
**Last Date: 2017/11/23
**Description: 
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2017/11/23
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef DB_demo1_H__
#define DB_demo1_H__
#include "wind_type.h"
#include "db_if.h"
typedef struct 
{
	w_int32_t key;
	w_int32_t ademo1[2];
	w_int8_t bdemo1;
	w_int16_t cdemo1;
    w_bool_t bodemo1;
    w_str64_t strdemo;
}tb_demo1;

TBMODEL_DECLARE(demo1);

#endif


