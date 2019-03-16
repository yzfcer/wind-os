/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**文   件   名: tb_demo1.h
**创   建   人: Jason Zhou
**最后修改日期: 2017/11/23
**描        述: 
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2017/11/23
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
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


