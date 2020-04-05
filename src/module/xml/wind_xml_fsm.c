/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml_fsm.c
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
#include "wind_type.h"
#include "wind_string.h"
#include "wind_xml_fsm.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
#define PTR_OFFSET(ptr1,ptr2) (w_int32_t)(w_addr_t)(ptr2 - ptr1)
static w_bool_t is_separation_character(char c) 
{ 
    switch(c)
    { 
        case 0x20: 
        case 0xD: 
        case 0xA: 
        case 0x9: 
            return W_TRUE;
    }
    return W_FALSE;
}

static int is_valid_prefix(int c) 
{ 
    return (c >= 'A' && c <= 'Z') || 
        (c >= 'a' && c <= 'z') || 
        (c == '_');
}

char *skip_prefix_space(char *buff) 
{ 
    char *p;
    for(p = buff;is_separation_character(*p);p ++);
    return (p);
}


static void xmlfsm_update_buff(xml_fsm_s *xmlfsm,w_uint8_t *arg,w_int32_t arglen)
{
    w_int32_t len;
    if(xmlfsm->buffidx >= xmlfsm->bufflen)
        xmlfsm->buffidx = 0;
    if(xmlfsm->buffidx > 0)
    {
        wind_memcpy(xmlfsm->buff,&xmlfsm->buff[xmlfsm->buffidx],xmlfsm->bufflen - xmlfsm->buffidx);
        xmlfsm->bufflen = xmlfsm->bufflen - xmlfsm->buffidx;
        xmlfsm->buffidx = 0;
    }
    if(xmlfsm->argidx < arglen)
    {
        len = XML_FSM_BUFLEN - xmlfsm->bufflen;
        len = len < arglen - xmlfsm->argidx?len:arglen - xmlfsm->argidx;
        wind_memcpy(&xmlfsm->buff[xmlfsm->bufflen],&arg[xmlfsm->argidx],len);
        xmlfsm->argidx += len;
    }
}


static w_err_t xml_handle_idle(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    w_err_t err;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    do
    {
        err = W_ERR_OK;
        xmlfsm = (xml_fsm_s*)fsm;
        while(xmlfsm->argidx < arglen)
        {
            xmlfsm_update_buff(xmlfsm,arg,arglen);
            
            buff = (char*)arg;
            buff = skip_prefix_space(buff);
            WIND_ASSERT_RETURN(buff[0] == '<',W_ERR_INVALID);
			xmlfsm_update_buff(xmlfsm,arg,arglen);
            if(buff[1] == '/')
                wind_fsm_change_step(fsm,XML_STAT_NODE_TAIL);
            else if(is_valid_prefix(buff[1]))
                wind_fsm_change_step(fsm,XML_STAT_NODE_NAME);
            else
            {
                wind_error("xml format error");
                wind_fsm_stop(fsm);
                return W_ERR_FAIL;
            }
        }
    }while(0);

    return W_ERR_OK;
}

static w_err_t xml_handle_node_name(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}

static w_err_t xml_handle_attr_name(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}

static w_err_t xml_handle_attr_value(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}

static w_err_t xml_handle_value(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}

static w_err_t xml_handle_node_tail(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}
static w_err_t xml_handle_end(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}


w_err_t wind_xml_fsm_init(xml_fsm_s *xfsm,char *name)
{
    static w_int32_t xid = 0;
    wind_memset(xfsm,0,sizeof(xml_fsm_s));
    xfsm->fsm = wind_fsm_create(name,xid,"xml");
    xid ++;
    if(xid < 0)
        xid = 0;
    WIND_ASSERT_RETURN(xfsm->fsm != W_NULL,W_ERR_FAIL);
    return W_ERR_OK;
}

w_err_t wind_xml_fsm_deinit(xml_fsm_s *xfsm)
{
    if(xfsm->fsm)
        wind_fsm_destroy(xfsm->fsm);
    if(xfsm->root)
        wind_xmlnode_destroy(xfsm->root);
    if(xfsm->newnode)
        wind_xmlnode_destroy(xfsm->newnode);
    if(xfsm->newattr)
        wind_xmlattr_destroy(xfsm->newattr);
    return W_ERR_OK;
}


FSM_STEP_START(xml)
FSM_STEP(XML_STAT_IDLE,      xml_handle_idle)
FSM_STEP(XML_STAT_NODE_NAME, xml_handle_node_name)
FSM_STEP(XML_STAT_ATTR_NAME, xml_handle_attr_name)
FSM_STEP(XML_STAT_ATTR_VALUE,xml_handle_attr_value)
FSM_STEP(XML_STAT_VALUE,     xml_handle_value)
FSM_STEP(XML_STAT_NODE_TAIL, xml_handle_node_tail)
FSM_STEP(XML_STAT_END,       xml_handle_end)
FSM_STEP_END
FSM_MODEL_DEF(xml)




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

