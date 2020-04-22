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

static w_int32_t is_valid_prefix(int c) 
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

static void xmlfsm_clear_buff(xml_fsm_s *xmlfsm)
{
    xmlfsm->buffidx = 0;
    wind_memset(xmlfsm->buff,0,XML_FSM_BUFLEN);
}

static void xmlfsm_change_step(w_fsm_s *fsm,w_int32_t new_step)
{
    fsm->sub_step = 0;
    wind_fsm_change_step(fsm,new_step);
}


static void xmlfsm_new_node(xml_fsm_s *xmlfsm)
{
    if(xmlfsm->ver_flag)
    {
        xmlfsm->version = xmlfsm->newnode;
        xmlfsm->ver_flag = 0;
        xmlfsm->newnode = W_NULL;
    }
    else
    {
        if(xmlfsm->cur == W_NULL)
            xmlfsm->cur = xmlfsm->newnode;
        else
            wind_xmlnode_insert(xmlfsm->cur,xmlfsm->newnode);
        xmlfsm->newnode = W_NULL;
    }
    xmlfsm_change_step(xmlfsm->fsm,XML_STEP_NODE_VALUE);
}


static w_err_t xml_find_char(w_fsm_s *fsm,char ch)
{
    char *buff;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    xmlfsm = (xml_fsm_s*)fsm;
    buff = fsm->arg;
    for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
    {
        if(buff[i] == ch)
        {
            xmlfsm->argidx = i;
            return W_ERR_OK;
        }
    }
    xmlfsm->argidx = i;
    return W_ERR_FAIL;
}

static w_err_t xml_skip_space(w_fsm_s *fsm)
{
    char *buff;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    xmlfsm = (xml_fsm_s*)fsm;
    buff = fsm->arg;
    for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
    {
        if(!is_separation_character(buff[i]))
        {
            xmlfsm->argidx = i;
            return W_ERR_OK;
        }
    }
    xmlfsm->argidx = i;
    return W_ERR_FAIL;
}


static w_err_t xml_handle_idle(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t idx;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xml_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                fsm->sub_step ++;
                break;
            case 1://skip '<'
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '<',W_ERR_FAIL);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 2://goto next step depengding on the next character
                idx = xmlfsm->argidx;
                if(buff[idx] == '?')
                {
                    xmlfsm->ver_flag = 1;
                    xmlfsm_change_step(fsm,XML_STEP_NODE_NAME);
                }
                else if(buff[idx+1] == '!')
                    xmlfsm_change_step(fsm,XML_STEP_NOTE);
                else if(buff[idx+1] == '/')
                    xmlfsm_change_step(fsm,XML_STEP_NODE_TAIL);
                else
                    xmlfsm_change_step(fsm,XML_STEP_NODE_NAME);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    //if(fsm->cur_step == XML_STEP_IDLE)
    wind_fsm_wait(fsm);
    return W_ERR_OK;
}



static w_err_t xml_handle_note(w_fsm_s *fsm)
{
    w_err_t err;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip '!'
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '!',W_ERR_FAIL);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 1:
            case 2:
            case 4://skip '-'
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '-',W_ERR_FAIL);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 3://find the end of character '-'
                err = xml_find_char(fsm,'-');
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 5://skip '>' and goto idle step
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '>',W_ERR_FAIL);
                xmlfsm->argidx ++;
                xmlfsm_change_step(fsm,XML_STEP_IDLE);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_node_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xml_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1://copy node name string
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if(!is_separation_character(buff[i]))
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = buff[i];
                        xmlfsm->argidx ++;
                        xmlfsm->buffidx ++;
                    }
                    else
                    {
                        fsm->sub_step ++;
                        break;
                    }
                }
                break;
            case 2://create xml node
                xmlfsm->buff[xmlfsm->buffidx] = 0;
                xmlfsm->newnode = wind_xmlnode_create(xmlfsm->buff);
                WIND_ASSERT_RETURN(xmlfsm->newnode != W_NULL,W_ERR_MEM);
                xmlfsm->argidx ++;
                xmlfsm_change_step(fsm,XML_STEP_ATTR_VALUE);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_attr_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xml_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1:
                if(buff[xmlfsm->argidx] == '>')
                {
                    xmlfsm_new_node(xmlfsm);
                    return W_ERR_OK;
                }
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if(!is_separation_character(buff[i]) && (buff[i] != '='))
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = buff[i];
                        xmlfsm->argidx ++;
                        xmlfsm->buffidx ++;
                    }
                    else
                    {
                        fsm->sub_step ++;
                        break;
                    }
                }
                break;
            case 2:
                xmlfsm->buff[xmlfsm->buffidx] = 0;
                xmlfsm->newattr = wind_xmlattr_crate(xmlfsm->buff,"");
                WIND_ASSERT_RETURN(xmlfsm->newattr != W_NULL,W_ERR_FAIL);
                xmlfsm_change_step(fsm,XML_STEP_ATTR_NAME);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_attr_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0:
                err = xml_find_char(fsm,'\"');
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1:
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    if(xmlfsm->buffidx >= XML_FSM_BUFLEN)
                        return W_ERR_FAIL;
                    if((buff[i] != '\"'))
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = buff[i];
                        xmlfsm->argidx ++;
                        xmlfsm->buffidx ++;
                    }
                    else
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = 0;
                        WIND_ASSERT_RETURN(xmlfsm->newattr != W_NULL,W_ERR_MEM);
                        wind_xmlattr_modify(xmlfsm->newattr,xmlfsm->buff);
                        wind_xmlnode_insert_attr(xmlfsm->newnode,xmlfsm->newattr);
                        xmlfsm_change_step(fsm,XML_STEP_ATTR_NAME);
                        return W_ERR_OK;
                    }
                }
            default:
                return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_node_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    xml_fsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = fsm->arg;
    xmlfsm = (xml_fsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xml_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1://copy node name string
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if(buff[i] != '<')
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = buff[i];
                        xmlfsm->argidx ++;
                        xmlfsm->buffidx ++;
                    }
                    else
                    {
                        fsm->sub_step ++;
                        break;
                    }
                }
                break;
            case 2://create xml node
                xmlfsm->buff[xmlfsm->buffidx] = 0;
                for(i = xmlfsm->buffidx - 1;i > 0;i --)
                    if(is_separation_character(xmlfsm->buff[i]))
                        xmlfsm->buff[i] = 0;
                WIND_ASSERT_RETURN(xmlfsm->newnode != W_NULL,W_ERR_FAIL);
                wind_xmlnode_set_value(xmlfsm->newnode,xmlfsm->buff);
                //xmlfsm->argidx ++;
                xmlfsm_change_step(fsm,XML_STEP_ATTR_VALUE);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_node_tail(w_fsm_s *fsm)
{
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
    
}
static w_err_t xml_handle_end(w_fsm_s *fsm)
{
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
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
FSM_STEP(XML_STEP_IDLE,      xml_handle_idle)
FSM_STEP(XML_STEP_NOTE     , xml_handle_note)
FSM_STEP(XML_STEP_NODE_NAME, xml_handle_node_name)
FSM_STEP(XML_STEP_ATTR_NAME, xml_handle_attr_name)
FSM_STEP(XML_STEP_ATTR_VALUE,xml_handle_attr_value)
FSM_STEP(XML_STEP_NODE_VALUE,xml_handle_node_value)
FSM_STEP(XML_STEP_NODE_TAIL, xml_handle_node_tail)
FSM_STEP(XML_STEP_END,       xml_handle_end)
FSM_STEP_END
FSM_MODEL_DEF(xml)




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

