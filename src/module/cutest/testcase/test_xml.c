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
#include "wind_xml.h"
#include "wind_core.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_XML_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/********************************************internal variables**********************************************/





/********************************************internal functions**********************************************/


/********************************************global variables**********************************************/



/********************************************global functions**********************************************/



CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    w_xmlnode_s *root;
    w_xmlnode_s *tmp;
    w_xmlattr_s *xattr;
    root = wind_xmlnode_create("testroot");
    EXPECT_NE(root,W_NULL);
    xattr = wind_xmlattr_crate("name","root");
    EXPECT_NE(xattr,W_NULL);
    wind_xmlnode_insert_attr(root,xattr);
    xattr = wind_xmlattr_crate("lenth","324");
    EXPECT_NE(xattr,W_NULL);
    wind_xmlnode_insert_attr(root,xattr);
    wind_xml_print(root);
}



SUITE_SETUP(xml) FUNC_EMPTY
SUITE_TEARDOWN(xml) FUNC_EMPTY

TEST_CASES_START(xml)
TEST_CASE(func)
TEST_CASES_END
TEST_SUITE(xml)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
