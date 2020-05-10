/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_xml.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: xml功能测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2020/03/25 16:29:55
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
/*********************************************header file***********************************************/
#include "wind_cut.h"
#include "wind_string.h"
#include "wind_xml.h"
#include "wind_xml_fsm.h"
#include "wind_core.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_XML_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************internal variables**********************************************/
static w_xmlfsm_s s_xfsm;




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
    xattr = wind_xmlattr_crate("name","root");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(root,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    xattr = wind_xmlattr_crate("lenth","324");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(root,xattr);
    EXPECT_EQ(err,W_ERR_OK);

    tmp = wind_xmlnode_create("child1");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_crate("name","level1");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);
    
    tmp = wind_xmlnode_create("child2");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_crate("name","level2");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);

    root = tmp;
    tmp = wind_xmlnode_create("child3");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_crate("name","level3");
    EXPECT_NE(xattr,W_NULL);
    err = wind_xmlnode_insert_attr(tmp,xattr);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xmlnode_insert(root,tmp);
    EXPECT_EQ(err,W_ERR_OK);

    root = tmp;
    tmp = wind_xmlnode_create("child4");
    EXPECT_NE(tmp,W_NULL);
    xattr = wind_xmlattr_crate("name","level4");
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
    char *xstr = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\
    <!-- xml test here   -->\
    <note>\
    <to>  George  </to>\
    <from>John</from>\
    <!-- xml version=\"1.0\" encoding=\"ISO-8859-1\"   -->\
    <heading >Reminder   </heading >\
    <heading1 Reminder =   	\"uhsdksd\\\"jidj\\\"\" />\
    <body>Don't forget the meeting!</body>\
    </note>";
    err = wind_xml_fsm_init(&s_xfsm,"xml");
    EXPECT_EQ(err,W_ERR_OK);
	err = wind_xml_fsm_input(&s_xfsm,xstr,wind_strlen(xstr));
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_xml_fsm_deinit(&s_xfsm);
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
