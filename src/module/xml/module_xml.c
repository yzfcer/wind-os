/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : module_xml.c
** Author      : Jason Zhou
** Last Date   : 2015/1/24 20:24:37
** Description : XML module entry
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020/05/11 00:24:37
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
#include "wind_xml_fsm.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




#if (WIND_MODULE_XML_SUPPORT)
FSM_MODEL_DELCARE(xml);
















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
