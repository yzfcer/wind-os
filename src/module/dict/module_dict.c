/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_dict.c
** Author      : Jason Zhou
** Last Date   : 2019/11/04 20:24:37
** Description : Parameter dictionary module entry
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2019/11/04 20:24:37
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
#include "wind_dict.h"
#include "wind_debug.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_DICT_SUPPORT)

/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/


MODULE_INIT(dict)
{
    
	return _wind_dict_mod_init();
}

MODULE_EXIT(dict)
{
    return W_ERR_OK;
}

MODULE_DEF(dict, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
