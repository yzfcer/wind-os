/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:    wind_xml_fsm.c
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
#include "wind_type.h"
#include "wind_string.h"
#include "wind_heap.h"
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

static void xmlfsm_clear_buff(w_xmlfsm_s *xmlfsm)
{
    xmlfsm->buffidx = 0;
    wind_memset(xmlfsm->buff,0,XML_FSM_BUFLEN);
}

static void xmlfsm_change_step(w_fsm_s *fsm,w_int32_t new_step)
{
    fsm->sub_step = 0;
    wind_fsm_change_step(fsm,new_step);
}


static w_err_t xmlfsm_new_node(w_xmlfsm_s *xmlfsm)
{
    if(xmlfsm->xhead_flag)
    {
        WIND_ASSERT_RETURN(xmlfsm->xhead == W_NULL,W_ERR_FAIL);
        xmlfsm->xhead_flag = 0;
        xmlfsm->xhead = xmlfsm->newnode;
		xmlfsm->newnode = (w_xmlnode_s*)W_NULL;
    }
    else
    {
        if(xmlfsm->parent == W_NULL)
            xmlfsm->parent = xmlfsm->newnode;
        else
            wind_xmlnode_insert(xmlfsm->parent,xmlfsm->newnode);
        xmlfsm->newnode = (w_xmlnode_s*)W_NULL;
    }
    xmlfsm_change_step(&xmlfsm->fsm,XML_STEP_NODE_VALUE);
    return W_ERR_OK;
}


static w_err_t xmlfsm_find_char(w_fsm_s *fsm,char ch)
{
    char *buff;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    xmlfsm = (w_xmlfsm_s*)fsm;
    buff = (char*)fsm->arg;
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

static w_err_t xmlfsm_skip_space(w_fsm_s *fsm)
{
    char *buff;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    xmlfsm = (w_xmlfsm_s*)fsm;
    buff = (char*)fsm->arg;
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

static void xmlfsm_parse_arg_end(w_xmlfsm_s *xfsm)
{
    w_fsm_s *fsm = &xfsm->fsm;
    xfsm->argidx = 0;
    fsm->arg = W_NULL;
    fsm->arglen = 0;
    wind_fsm_wait(fsm);
}

static void xmlfsm_parse_fail(w_xmlfsm_s *xfsm)
{
    wind_xml_fsm_deinit(xfsm);
    
}


static w_err_t xml_handle_idle(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t idx;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters and '<'
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '<',W_ERR_FAIL);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                //fsm->sub_step ++;
                break;
            case 1://goto next step depengding on the next character
                idx = xmlfsm->argidx;
                if(buff[idx] == '?')
                {
                    xmlfsm->xhead_flag = 1;
                    xmlfsm->argidx ++;
                    xmlfsm_change_step(fsm,XML_STEP_NODE_NAME);
                }
                else if(buff[idx+1] == '!')
                {
                    xmlfsm->argidx ++;
                    xmlfsm_change_step(fsm,XML_STEP_NOTE);
                }
                else
                    xmlfsm_change_step(fsm,XML_STEP_NODE_NAME);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}



static w_err_t xml_handle_note(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            //case 0://skip '!'
            //    WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '!',W_ERR_FAIL);
            //    xmlfsm->argidx ++;
            //    fsm->sub_step ++;
            //    break;
            case 1:
            case 2:
            case 4://skip '-'
                WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '-',W_ERR_FAIL);
                xmlfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 3://find the end of character '-'
                err = xmlfsm_find_char(fsm,'-');
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
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}

static w_err_t xml_handle_node_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1://copy node name string
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if(!is_separation_character(buff[i]) || buff[i] == '=')
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
                xmlfsm_change_step(fsm,XML_STEP_ATTR_NAME);
                xmlfsm->argidx ++;
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}

static w_err_t xml_handle_attr_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1://check if has attribution
                if(buff[xmlfsm->argidx] == '>')
                {
                    xmlfsm->argidx ++;
                    xmlfsm_change_step(fsm,XML_STEP_NODE_VALUE);
                    return W_ERR_OK;
                }
                else if(buff[xmlfsm->argidx] == '/')
                {
                    xmlfsm->argidx ++;
                    xmlfsm_change_step(fsm,XML_STEP_NODE_TAIL);
                    return W_ERR_OK;
                }
                else
                {
                    fsm->sub_step ++;
                    break;
                }
            case 2:
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
            case 3:
                xmlfsm->buff[xmlfsm->buffidx] = 0;
                xmlfsm->newattr = wind_xmlattr_crate(xmlfsm->buff,"");
                WIND_ASSERT_RETURN(xmlfsm->newattr != W_NULL,W_ERR_FAIL);
                xmlfsm_change_step(fsm,XML_STEP_ATTR_VALUE);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}

static w_err_t xml_handle_attr_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0:
                err = xmlfsm_find_char(fsm,'\"');
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xmlfsm_clear_buff(xmlfsm);
                fsm->sub_step ++;
                break;
            case 1:
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if((buff[i] != '\"'))
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
                WIND_ASSERT_RETURN(xmlfsm->newattr != W_NULL,W_ERR_MEM);
                wind_xmlattr_modify(xmlfsm->newattr,xmlfsm->buff);
                wind_xmlnode_insert_attr(xmlfsm->newnode,xmlfsm->newattr);
                xmlfsm->newattr = W_NULL;
                xmlfsm_change_step(fsm,XML_STEP_ATTR_NAME);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}

static w_err_t xml_handle_node_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
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
                xmlfsm_change_step(fsm,XML_STEP_NODE_TAIL);
                xmlfsm->argidx ++;
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}

static w_err_t xml_handle_node_tail(w_fsm_s *fsm)
{
    w_int32_t i;
    //w_err_t err;
    w_xmlfsm_s *xmlfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xmlfsm = (w_xmlfsm_s*)fsm;
    while(xmlfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0:
                //WIND_ASSERT_RETURN(buff[xmlfsm->argidx] == '!',W_ERR_FAIL);
                xmlfsm_clear_buff(xmlfsm);
                if(buff[xmlfsm->argidx] == '>')
                {
                    xmlfsm_new_node(xmlfsm);
                    xmlfsm->argidx ++;
                    xmlfsm_change_step(fsm,XML_STEP_IDLE);
                }
                else if(buff[xmlfsm->argidx] == '/')
                {
                    xmlfsm->argidx ++;
                    fsm->sub_step ++;
                }
                else
                    return W_ERR_FAIL;
                break;
            case 1:
                for(i = xmlfsm->argidx;i < fsm->arglen;i ++)
                {
                    WIND_ASSERT_RETURN(xmlfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
                    if(buff[i] != '>')
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = buff[i];
                        xmlfsm->argidx ++;
                        xmlfsm->buffidx ++;
                    }
                    else
                    {
                        xmlfsm->buff[xmlfsm->buffidx] = 0;
                        xmlfsm->argidx ++;
                        fsm->sub_step ++;
                        break;
                    }
                }
                break;
            case 2:
                if(wind_strcmp(xmlfsm->parent->name,xmlfsm->buff) == 0)
                {
                    xmlfsm->parent = wind_xmlnode_get_parent(xmlfsm->parent);
                    if(xmlfsm->parent == W_NULL)
                        xmlfsm_change_step(fsm,XML_STEP_END);
                }
                else if(wind_strcmp(xmlfsm->newnode->name,xmlfsm->buff) != 0)
                    return W_ERR_FAIL;
                else
                    xmlfsm_change_step(fsm,XML_STEP_IDLE);
                return W_ERR_OK;
            default:
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xmlfsm);
    return W_ERR_OK;
}
static w_err_t xml_handle_end(w_fsm_s *fsm)
{
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    return W_ERR_FAIL;
}


w_err_t wind_xml_fsm_init(w_xmlfsm_s *xfsm,char *name)
{
    w_err_t err;
    static w_int32_t xfsm_id = 0;
    char *fsmname = (char*)wind_malloc(16);
    WIND_ASSERT_RETURN(fsmname != W_NULL,W_ERR_MEM);
    wind_memset(fsmname,0,16);
    wind_sprintf(fsmname,"xml%d",xfsm_id);
    wind_memset(xfsm,0,sizeof(w_xmlfsm_s));
    err = wind_fsm_init(&xfsm->fsm,fsmname,xfsm_id,name);
    xfsm_id ++;
    if(xfsm_id < 0)
        xfsm_id = 0;
    WIND_ASSERT_RETURN(err == W_ERR_OK,err);
    wind_fsm_start(&xfsm->fsm);
    wind_fsm_wait(&xfsm->fsm);
    return W_ERR_OK;
}

w_err_t wind_xml_fsm_input(w_xmlfsm_s *xfsm,char *xstr,w_int32_t len)
{
    w_err_t err;
    WIND_ASSERT_RETURN(xfsm != W_NULL,W_ERR_PTR_NULL);
    err = wind_fsm_input(&xfsm->fsm,xstr,len);
    return err;
}


w_err_t wind_xml_fsm_deinit(w_xmlfsm_s *xfsm)
{
    WIND_ASSERT_RETURN(xfsm != W_NULL,W_ERR_PTR_NULL);

    wind_free(xfsm->fsm.obj.name);
    wind_fsm_destroy(&xfsm->fsm);
    
    //xfsm->fsm = (w_fsm_s*)W_NULL;
    if(xfsm->root)
        wind_xmlnode_destroy(xfsm->root);
    if(xfsm->xhead)
        wind_xmlnode_destroy(xfsm->xhead);
    if(xfsm->parent)
        wind_xmlnode_destroy(xfsm->parent);
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

