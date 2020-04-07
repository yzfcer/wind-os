/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml_fsm.h
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2020/03/24
  *Description: 
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef WIND_XML_FSM_H__
#define WIND_XML_FSM_H__
#include "wind_fsm.h"
#include "wind_xml.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define XML_FSM_BUFLEN 512
typedef struct __xml_fsm_s xml_fsm_s;
typedef enum
{
    XML_STEP_IDLE,
    XML_STEP_VERSION,
    XML_STEP_NOTE,
    XML_STEP_NODE_NAME,
    XML_STEP_ATTR_NAME,
    XML_STEP_ATTR_VALUE,
    XML_STEP_NODE_VALUE,
    XML_STEP_NODE_TAIL,
    XML_STEP_END
}xml_state_e;

struct __xml_fsm_s
{
    w_fsm_s *fsm;
    w_xmlnode_s *root;
    w_xmlnode_s *cur;
    w_xmlnode_s *newnode;
    w_xmlattr_s *newattr;
    char buff[XML_FSM_BUFLEN];
    w_int32_t bufflen;
    w_int32_t buffidx;
    w_int32_t argidx;
    
};
w_err_t wind_xml_fsm_init(xml_fsm_s *xfsm,char *name);
w_err_t wind_xml_fsm_deinit(xml_fsm_s *xfsm);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_XML_H__
