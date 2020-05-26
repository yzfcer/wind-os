/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName    :    wind_xml_fsm.c
  *** Author      :      Jason Zhou
  *Version:     1.0
  *Date:        2020/03/24
  *Description: XML parsing finite state machine
  *Others:  
  *History:  
     1.Date:
       ** Author      :
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
#define IS_XML_BORDER(c) ((C) == '<' || (c) == '>')
#define SEP_CHLIST "\r\n\t "

#if 0
static w_int32_t is_valid_prefix(int c) 
{ 
    return (c >= 'A' && c <= 'Z') || 
        (c >= 'a' && c <= 'z') || 
        (c == '_');
}
#endif


static void xmlfsm_change_step(w_xmlfsm_s *xfsm,w_int32_t new_step)
{
    xfsm->fsm.sub_step = 0;
    xfsm->buffidx = 0;
    if(xfsm->buff[0] != 0)
        wind_memset(xfsm->buff,0,XML_FSM_BUFLEN);
    wind_fsm_change_step(&xfsm->fsm,new_step);
}

static w_bool_t xmlfsm_check_chlist(char *chlist,char ch)
{
    w_int32_t i;
    w_int32_t len = wind_strlen(chlist);
    for(i = 0;i < len;i ++)
        if(chlist[i] == ch)
            return W_TRUE;
    return W_FALSE;
}

static w_err_t xmlfsm_copy_until(w_xmlfsm_s *xfsm,char *chlist)
{
    w_int32_t i;
    char *buff;
    w_fsm_s *fsm = &xfsm->fsm;
    buff = (char *)fsm->arg;
    for(i = xfsm->argidx;i < fsm->arglen;i ++)
    {
        WIND_ASSERT_RETURN(xfsm->buffidx < XML_FSM_BUFLEN,W_ERR_FAIL);
        if(buff[i] == '\\' && (!xfsm->trans_flag))
        {
            xfsm->trans_flag = 1;
            xfsm->argidx ++;
            continue;
        }
        if(!xmlfsm_check_chlist(chlist,buff[i]) || xfsm->trans_flag)
        {
            xfsm->trans_flag = 0;
            xfsm->buff[xfsm->buffidx] = buff[i];
            xfsm->argidx ++;
            xfsm->buffidx ++;
        }
        else
        {
            xfsm->buff[xfsm->buffidx] = 0;
            wind_debug("item name:%s",xfsm->buff);
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}


static w_err_t xmlfsm_open_xnode(w_xmlfsm_s *xfsm,char *name)
{
    w_err_t err;
    w_xmlnode_s *newnode = W_NULL;
    do
    {
        err = W_ERR_OK;
        wind_debug("open node name:%s",name);
        newnode = wind_xmlnode_create(xfsm->buff);
        WIND_ASSERT_BREAK(newnode != W_NULL,W_ERR_MEM,"create xnode fail:%s");
        if(xfsm->xhead_flag) //add xml head info
        {
            WIND_ASSERT_BREAK(xfsm->xhead == W_NULL,W_ERR_FAIL,"xml head has been exist");
            xfsm->xhead = newnode;
            break;
        }
        if(xfsm->root == W_NULL) // add xml root node
        {
            xfsm->root = newnode;
            break;
        }
        if(xfsm->newnode != W_NULL) // enter sub node
            xfsm->parent = xfsm->newnode;
        //else // add xml sub node
        err = wind_xmlnode_insert(xfsm->parent,newnode);
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"insert xnode fail");
    }while(0);
    if(err != W_ERR_OK)
    {
        if(newnode != W_NULL)
            wind_xmlnode_destroy(newnode);
    }
    else
        xfsm->newnode = newnode;
    return err;
}

static w_err_t xmlfsm_close_xnode(w_xmlfsm_s *xfsm,char *name)
{
    wind_debug("close node name:%s",name);
    if(xfsm->xhead_flag)
    {
        xfsm->newnode = W_NULL;
        xfsm->xhead_flag = 0;
        return W_ERR_OK;
    }
    if(xfsm->newnode && (wind_strcmp(name,xfsm->newnode->name) == 0))
    {
        xfsm->newnode = W_NULL;
        return W_ERR_OK;
    }
    else if(xfsm->parent && (wind_strcmp(name,xfsm->parent->name) == 0))
    {
        xfsm->parent = wind_xmlnode_get_parent(xfsm->parent);
        return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

static w_err_t xmlfsm_new_attr(w_xmlfsm_s *xfsm,char *attr_name)
{
    w_err_t err;
    wind_debug("attr name:%s",attr_name);
    xfsm->newattr = wind_xmlattr_create(attr_name,"");
    WIND_ASSERT_RETURN(xfsm->newattr != W_NULL,W_ERR_MEM);
    err = wind_xmlnode_insert_attr(xfsm->newnode,xfsm->newattr);
    return err;
}


static w_err_t xmlfsm_find_char(w_fsm_s *fsm,char ch)
{
    char *buff;
    w_int32_t i;
    w_xmlfsm_s *xfsm;
    xfsm = (w_xmlfsm_s*)fsm;
    buff = (char*)fsm->arg;
    for(i = xfsm->argidx;i < fsm->arglen;i ++)
    {
        if(buff[i] == ch)
        {
            xfsm->argidx = i;
            return W_ERR_OK;
        }
    }
    xfsm->argidx = i;
    return W_ERR_FAIL;
}

static w_err_t xmlfsm_skip_space(w_fsm_s *fsm)
{
    char *buff;
    w_int32_t i;
    w_xmlfsm_s *xfsm;
    xfsm = (w_xmlfsm_s*)fsm;
    buff = (char*)fsm->arg;
    for(i = xfsm->argidx;i < fsm->arglen;i ++)
    {
        if(!xmlfsm_check_chlist(SEP_CHLIST,buff[i]))
        {
            xfsm->argidx = i;
            return W_ERR_OK;
        }
    }
    xfsm->argidx = i;
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



static w_err_t xmlfsm_handle_idle(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://find and skip separate characters and '<'
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                WIND_ASSERT_RETURN(buff[xfsm->argidx] == '<',W_ERR_FAIL);
                xfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 1://goto next step depengding on current character
                if(buff[xfsm->argidx] == '?') // xml head
                {
                    xfsm->xhead_flag = 1;
                    xfsm->argidx ++;
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_NAME);
                }
                else if(buff[xfsm->argidx] == '!') // xml note
                {
                    xfsm->argidx ++;
                    xmlfsm_change_step(xfsm,XML_STEP_NOTE);
                }
                else if(buff[xfsm->argidx] == '/') // xml tail
                {
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_TAIL);
                }
                else //xml node name
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_NAME);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}



static w_err_t xmlfsm_handle_note(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0:
            case 1:// must be "--",skip '-'
                WIND_ASSERT_RETURN(buff[xfsm->argidx] == '-',W_ERR_FAIL);
                xfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 2://find the end of character '-'
                err = xmlfsm_find_char(fsm,'-');
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 3:// must be "--",otherwise turn back to sun step 2
                if(buff[xfsm->argidx] != '-')
                    fsm->sub_step = 2;
                else
                    fsm->sub_step ++;
                xfsm->argidx ++;
                break;
            case 4://skip '>' and goto idle step
                if(buff[xfsm->argidx] != '>')
                {
                    fsm->sub_step = 2;
                    xfsm->argidx ++;
                    break;
                }
                xfsm->argidx ++;
                xmlfsm_change_step(xfsm,XML_STEP_IDLE);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_node_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                fsm->sub_step ++;
                break;
            case 1://copy node name string
                err = xmlfsm_copy_until(xfsm,SEP_CHLIST">");
                if(err == W_ERR_OK)
                    fsm->sub_step ++;
                break;
            case 2://create xml node
                wind_debug("xnode name:%s",xfsm->buff);
                err = xmlfsm_open_xnode(xfsm,xfsm->buff);
                WIND_ASSERT_RETURN(err == W_ERR_OK,err);
                if(buff[xfsm->argidx] != '>')
                    xmlfsm_change_step(xfsm,XML_STEP_ATTR_NAME);
                else
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_VALUE);
                xfsm->argidx ++;
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_attr_name(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                fsm->sub_step ++;
                break;
            case 1://check if has attribution
                if(buff[xfsm->argidx] == '>')
                {
                    xfsm->argidx ++;
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_VALUE);
                    return W_ERR_OK;
                }
                else if((buff[xfsm->argidx] == '/') || (buff[xfsm->argidx] == '?'))
                {
                    xfsm->argidx ++;
                    xmlfsm_change_step(xfsm,XML_STEP_NODE_TAIL);
                    return W_ERR_OK;
                }
                else
                {
                    fsm->sub_step ++;
                    break;
                }
            case 2:
                err = xmlfsm_copy_until(xfsm,SEP_CHLIST"=");
                if(err == W_ERR_OK)
                    fsm->sub_step ++;
                break;
            case 3:
                xfsm->argidx ++;
                //wind_debug("xattr name:%s",xfsm->buff);
                err = xmlfsm_new_attr(xfsm,xfsm->buff);
                WIND_ASSERT_RETURN(err == W_ERR_OK,err);
                xmlfsm_change_step(xfsm,XML_STEP_ATTR_VALUE);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_attr_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0:
                err = xmlfsm_find_char(fsm,'\"');
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                xfsm->argidx ++;
                fsm->sub_step ++;
                break;
            case 1:
                err = xmlfsm_copy_until(xfsm,"\"");
                if(err == W_ERR_OK)
                    fsm->sub_step ++;
                break;
            case 2:
                xfsm->argidx ++;
                wind_debug("xattr value:%s",xfsm->buff);
                WIND_ASSERT_RETURN(xfsm->newattr != W_NULL,W_ERR_MEM);
                wind_xmlattr_modify(xfsm->newattr,xfsm->buff);
                xfsm->newattr = W_NULL;
                xmlfsm_change_step(xfsm,XML_STEP_ATTR_NAME);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_node_value(w_fsm_s *fsm)
{
    w_err_t err;
    w_int32_t i;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://skip separate characters
                err = xmlfsm_skip_space(fsm);
                WIND_CHECK_RETURN(err == W_ERR_OK,W_ERR_OK);
                fsm->sub_step ++;
                break;
            case 1://copy node name string
                //err = xmlfsm_copy_xnode_value(xfsm);
                err = xmlfsm_copy_until(xfsm,"<");
                if(err == W_ERR_OK)
                    fsm->sub_step ++;
                break;
            case 2://create xml node
                for(i = xfsm->buffidx - 1;i > 0;i --)
                {
                    if(xmlfsm_check_chlist(SEP_CHLIST,xfsm->buff[i]))
                        xfsm->buff[i] = 0;
                    else
                        break;
                }
                WIND_ASSERT_RETURN(xfsm->newnode != W_NULL,W_ERR_FAIL);
                wind_debug("xnode value:%s",xfsm->buff);
                if(xfsm->buff[0] != 0)
                    wind_xmlnode_set_value(xfsm->newnode,xfsm->buff);
                //xfsm->argidx ++;
                xmlfsm_change_step(xfsm,XML_STEP_IDLE);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_node_tail(w_fsm_s *fsm)
{
    w_err_t err;
    w_xmlfsm_s *xfsm;
    char *buff;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    buff = (char*)fsm->arg;
    xfsm = (w_xmlfsm_s*)fsm;
    while(xfsm->argidx < fsm->arglen)
    {
        switch(fsm->sub_step)
        {
            case 0://check tail type
                if(buff[xfsm->argidx] == '>')// tail likes "/>" or "?>"
                {
					xmlfsm_close_xnode(xfsm,xfsm->newnode->name);
                    xfsm->argidx ++;
                    xmlfsm_change_step(xfsm,XML_STEP_IDLE);
                    return W_ERR_OK;
                }
                else if(buff[xfsm->argidx] == '/')// tail likes "</"
                {
                    xfsm->argidx ++;
                    fsm->sub_step ++;
                }
                else  
                {
                    //xmlfsm_change_step(xfsm,XML_STEP_IDLE);
                    wind_error("error node tail:%c",buff[xfsm->argidx]);
                    return W_ERR_FAIL;
                }
                break;
            case 1:// copy node tail name 
                err = xmlfsm_copy_until(xfsm,SEP_CHLIST">");
                if(err == W_ERR_OK)
                    fsm->sub_step ++;
                break;
            case 2:// close xml node
                xmlfsm_close_xnode(xfsm,xfsm->buff);
                fsm->sub_step ++;
                return W_ERR_OK;
            case 3:// skip '>'
                err = xmlfsm_find_char(fsm,'>');
                if(err != W_ERR_OK)
                    return W_ERR_OK;
                xfsm->argidx ++;
                if(xfsm->parent == W_NULL)
                    xmlfsm_change_step(xfsm,XML_STEP_END);
                else
                    xmlfsm_change_step(xfsm,XML_STEP_IDLE);
                return W_ERR_OK;
            default:
                wind_error("xml unkown sub_step:%d",fsm->sub_step);
                return W_ERR_FAIL;
        }
    }
    xmlfsm_parse_arg_end(xfsm);
    return W_ERR_OK;
}

static w_err_t xmlfsm_handle_end(w_fsm_s *fsm)
{
    w_xmlfsm_s *xfsm;
    WIND_CHECK_RETURN(fsm->arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(fsm->arglen > 0,W_ERR_INVALID);
    xfsm = (w_xmlfsm_s*)fsm;
    wind_debug("xfsm parse complete");
    xfsm->complete_flag = 1;
    return W_ERR_FAIL;
}


w_err_t wind_xml_fsm_init(w_xmlfsm_s *xfsm,char *name)
{
    w_err_t err;
    char *fsmname;
    static w_int32_t xfsm_id = 0;
    WIND_ASSERT_RETURN(xfsm != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(name != W_NULL,W_ERR_PTR_NULL);
    fsmname = (char*)wind_malloc(16);
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
    
    if(xfsm->root)
        wind_xmlnode_destroy(xfsm->root);
    if(xfsm->xhead)
        wind_xmlnode_destroy(xfsm->xhead);
    if(xfsm->newattr)
        wind_xmlattr_destroy(xfsm->newattr);
    return W_ERR_OK;
}


FSM_STEP_START(xml)
FSM_STEP(XML_STEP_IDLE,      xmlfsm_handle_idle)
FSM_STEP(XML_STEP_NOTE     , xmlfsm_handle_note)
FSM_STEP(XML_STEP_NODE_NAME, xmlfsm_handle_node_name)
FSM_STEP(XML_STEP_ATTR_NAME, xmlfsm_handle_attr_name)
FSM_STEP(XML_STEP_ATTR_VALUE,xmlfsm_handle_attr_value)
FSM_STEP(XML_STEP_NODE_VALUE,xmlfsm_handle_node_value)
FSM_STEP(XML_STEP_NODE_TAIL, xmlfsm_handle_node_tail)
FSM_STEP(XML_STEP_END,       xmlfsm_handle_end)
FSM_STEP_END
FSM_MODEL_DEF(xml)




#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

