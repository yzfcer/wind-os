/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_cjson.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : cjson模块入口
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2015/1/24 20:24:37
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_CJSON_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/

extern int cjson_enable;

MODULE_INIT(cjson)
{
    cjson_enable = 1;
	return W_ERR_OK;
}

MODULE_EXIT(cjson)
{
    cjson_enable = 0;
    return W_ERR_OK;
}

MODULE_DEF(cjson, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
