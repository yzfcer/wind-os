/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: module_xml.c
** **Author: Jason Zhou
** Last Date: 2015/1/24 20:24:37
** Description: xml模块入口
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2020/05/11 00:24:37
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_module.h"
#include "wind_cmd.h"
#include "wind_xml_fsm.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************header file***********************************************/

#if (WIND_MODULE_XML_SUPPORT)
FSM_MODEL_DELCARE(xml);
/********************************************internal variables**********************************************/




/********************************************internal functions**********************************************/



/********************************************global variables**********************************************/



/********************************************global functions**********************************************/


MODULE_INIT(xml)
{
    w_err_t err;
    err = wind_fsm_model_register(FSM_MODEL(xml));
    return err;//_wind_xml_mod_init();
}

MODULE_EXIT(xml)
{
    w_err_t err;
    err = wind_fsm_model_unregister(FSM_MODEL(xml));
    return err;//_wind_xml_mod_init();
}

MODULE_DEF(xml, 0x0100,"");

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
