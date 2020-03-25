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
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#define XML_FSM_BUFLEN 512
typedef enum
{
    XML_STAT_IDLE,
    XML_STAT_NODE_NAME,
    XML_STAT_ATTR_NAME,
    XML_STAT_ATTR_VALUE,
    XML_STAT_VALUE,
    XML_STAT_NODE_TAIL,
    XML_STAT_END
}xml_state_e;

typedef struct
{
    w_fsm_s *fsm;
    char *buff;//[XML_FSM_BUFLEN];
    w_int32_t bufflen;
    w_int32_t idx;
    
}xml_fsm_s;



#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif //#ifndef WIND_XML_H__
