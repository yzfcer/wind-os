/****************************************Copyright (c)**************************************************
**                                       God's harbor
**FileName: tb_demo.h
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
#ifndef DB_DEMO_H__
#define DB_DEMO_H__
#include "wind_type.h"
#include "db_if.h"
typedef struct 
{
	w_int32_t key;
	w_int32_t ademo[2];
	w_int8_t bdemo;
	w_int16_t cdemo;
}tb_demo;

TBMODEL_DECLARE(demo);

#endif


