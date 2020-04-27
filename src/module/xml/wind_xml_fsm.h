/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml_fsm.h
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2020/03/24
  *Description: XML parsing finite state machine
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
    XML_STEP_IDLE,      //when XML FSM initiated
    XML_STEP_NOTE,      //when parse a xml note       
    XML_STEP_NODE_NAME, //when parse a xml node name
    XML_STEP_ATTR_NAME, //when parse a xml node attribution name
    XML_STEP_ATTR_VALUE,//when parse a xml node attribution value
    XML_STEP_NODE_VALUE,//when parse a xml node value
    XML_STEP_NODE_TAIL, //when parse a xml node tail
    XML_STEP_END        //when FSM going to end
}xml_state_e;

struct __xml_fsm_s
{
    w_fsm_s *fsm;              //XML FSM object
    w_xmlnode_s *root;         //XML root node
    w_xmlnode_s *xhead;        //XML head declaration
    w_xmlnode_s *parent;       //XML parent node
    w_xmlnode_s *newnode;      //XML new node
    w_xmlattr_s *newattr;      //XML new attribution
    char buff[XML_FSM_BUFLEN]; //XML FSM buffer
    w_int32_t bufflen;         //XML FSM buffer lenth
    w_int32_t buffidx;         //XML FSM buffer's current index
    w_int32_t argidx;          //XML FSM argument's current index
    w_int32_t xhead_flag;      //Flag which mark a xhead node    
};
w_err_t wind_xml_fsm_init(xml_fsm_s *xfsm,char *name);
w_err_t wind_xml_fsm_input(xml_fsm_s *xfsm,char *xstr,w_int32_t len);
w_err_t wind_xml_fsm_deinit(xml_fsm_s *xfsm);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_XML_H__
