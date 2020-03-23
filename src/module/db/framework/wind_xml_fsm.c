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


static void xmlfsm_update_buff(xml_fsm_s *xmlfsm,char *buff)
{
    w_int32_t diff;
    diff = PTR_OFFSET(xmlfsm->buff,buff);
    xmlfsm->bufflen -= diff;
    xmlfsm->buff = buff;
}


static w_err_t xml_handle_idle(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    w_int32_t idx;
    xml_fsm_s *xmlfsm;
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
    char *buff = (char*)arg;
    xmlfsm = (xml_fsm_s*)fsm;
    buff = skip_prefix_space(buff);
    WIND_ASSERT_RETURN(buff[0] == '<',W_ERR_INVALID);
    xmlfsm_update_buff(fsm,buff);
    if(buff[1] == '/')
        wind_fsm_change_step(fsm,XML_STAT_NODE_TAIL);
    else if(is_valid_prefix(buff[1])
        wind_fsm_change_step(fsm,XML_STAT_NODE_NAME);
    else
    {
        wind_error("xml format error");
        wind_fsm_stop(fsm);
        return W_ERR_FAIL;
    }
    return W_ERR_OK;
}

static w_err_t xml_handle_node_name(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
}

static w_err_t xml_handle_attr_name(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);

}

static w_err_t xml_handle_attr_value(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);

}
static w_err_t xml_handle_value(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);

}

static w_err_t xml_handle_node_tail(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);

}
static w_err_t xml_handle_end(w_fsm_s *fsm,void *arg,w_int32_t arglen)
{
    WIND_CHECK_RETURN(arg != W_NULL,W_ERR_PTR_NULL);
    WIND_CHECK_RETURN(arglen > 0,W_ERR_INVALID);
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
