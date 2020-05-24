/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: test_xml.c
** ** Author      : Jason Zhou
** Last Date: 2017/10/22 16:29:55
** Description : xml功能测试
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2020/03/25 16:29:55
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************header file***********************************************/
#include "wind_cut.h"
#include "wind_string.h"
#include "wind_xml.h"
//#include "wind_xml_fsm.h"
#include "wind_core.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_XML_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************internal variables**********************************************/
//static w_xmlfsm_s s_xfsm;




/********************************************internal functions**********************************************/


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/



CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    w_xmlnode_s *root,*root1;
    w_xmlnode_s *tmp;
    w_xmlattr_s *xattr;
    root = wind_xmlnode_create("testroot");
    EXPECT_NE(root,W_NULL);
    xattr = wind_xmlattr_create("name","root");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(root,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    xattr = wind_xmlattr_create("lenth","324");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(root,xattr);
    EXPECT_EQ(err,W_ERR_OK);

    tmp = wind_xmlnode_create("child1");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_create("name","level1");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);
    
    tmp = wind_xmlnode_create("child2");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_create("name","level2");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);

    root = tmp;
    tmp = wind_xmlnode_create("child3");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_create("name","level3");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);

    root = tmp;
    tmp = wind_xmlnode_create("child4");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_create("name","level4");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_set_value(tmp,"level4 value");
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);

    root = wind_xmlnode_get_parent(root);
    EXPECT_NE(root,W_NULL);
    root = wind_xmlnode_get_parent(root);
    EXPECT_NE(root,W_NULL);
    //wind_printf("sizeof xndoe=%d\r\n",sizeof(w_xmlnode_s));
    err = wind_xml_print(root);
    EXPECT_EQ(err,W_ERR_OK);
    root1 = wind_xmlnode_get_parent(root);
    EXPECT_EQ(root1,W_NULL);
    err = wind_xmlnode_destroy(root);
    EXPECT_EQ(err,W_ERR_OK);
}

CASE_SETUP(parse) FUNC_EMPTY
CASE_TEARDOWN(parse) FUNC_EMPTY
CASE_FUNC(parse)
{
    w_err_t err;
    w_xml_s xml;
    char *xstr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n\
    <!-- xml test here   -->\r\n\
    <note>\r\n\
    <to>  George  </to>\r\n\
    <from>John</from>\r\n\
    <!-- xml version=\"1.0\" encoding=\"ISO-8859-1\"   -->\r\n\
    <heading >Reminder   </heading >\r\n\
    <heading1 Reminder =   	\"uhsdksd\\\"jidj\\\"\" />\r\n\
    <body>Don't forget the meeting!</body>\r\n\
    </note>\r\n";
    wind_printf("xml=%s\r\n",xstr);
    //err = wind_xml_fsm_init(&s_xfsm,"xml");
    err = wind_xml_init(&xml);
    EXPECT_EQ(err,W_ERR_OK);
	//err = wind_xml_fsm_input(&s_xfsm,xstr,wind_strlen(xstr));
	err = wind_xml_parse(&xml,xstr,wind_strlen(xstr));
    EXPECT_EQ(err,W_ERR_OK);

    err = wind_xml_print(xml.xhead);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xml_print(xml.root);
    EXPECT_EQ(err,W_ERR_OK);
    
    //err = wind_xml_fsm_deinit(&xml);
    err = wind_xml_deinit(&xml);
    EXPECT_EQ(err,W_ERR_OK);
}



SUITE_SETUP(xml) FUNC_EMPTY
SUITE_TEARDOWN(xml) FUNC_EMPTY

TEST_CASES_START(xml)
TEST_CASE(func)
TEST_CASE(parse)
TEST_CASES_END
TEST_SUITE(xml)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
