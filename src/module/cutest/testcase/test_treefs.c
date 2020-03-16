/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_treefs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: treefs文件系统单元测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2017/10/22 16:29:55
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cut.h"
#include "wind_string.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_TREEFS_SUPPORT)
#include "treefs.h"
#include "treefile.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

CASE_SETUP(create) FUNC_EMPTY
CASE_TEARDOWN(create) FUNC_EMPTY
CASE_FUNC(create)
{
    w_err_t err;
    w_treefile_s *file;
    w_treefs_s *tfs;
    tfs = wind_treefs_get("tfs0");
    WIND_ASSERT_RETURN_VOID(tfs != W_NULL);
    file = treefile_open(tfs,"/test.txt",TF_FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = treefile_remove(file);
    EXPECT_EQ(err,W_ERR_OK);
    file = treefile_open(tfs,"/test1.txt",TF_FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = treefile_remove(file);
    EXPECT_EQ(err,W_ERR_OK);
    file = treefile_open(tfs,"/test2.txt",TF_FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = treefile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = treefile_remove(file);
    EXPECT_EQ(err,W_ERR_OK);
}


static w_uint8_t buff[32];
CASE_SETUP(readwrite) FUNC_EMPTY
CASE_TEARDOWN(readwrite) FUNC_EMPTY
CASE_FUNC(readwrite)
{
    w_int32_t len;
    w_err_t err;
    w_treefile_s *file;
    w_treefs_s *tfs;
    char *str = "this is a file test string.";
    tfs = wind_treefs_get("tfs0");
    WIND_ASSERT_RETURN_VOID(tfs != W_NULL);
    file = treefile_open(tfs,"/test.txt",TF_FMODE_CRT | TF_FMODE_W);
    EXPECT_NE(file,W_NULL);
    len = treefile_write(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = treefile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    wind_memset(buff,0,32);
    file = treefile_open(tfs,"/test.txt",TF_FMODE_R);
    EXPECT_NE(file,W_NULL);
    len = treefile_read(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = treefile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = treefile_remove(file);
    EXPECT_EQ(err,W_ERR_OK);
    
}

SUITE_SETUP(treefs)
{
    w_treefs_s *tfs;
    tfs = wind_treefs_get("tfs0");
    WIND_ASSERT_RETURN(tfs != W_NULL,W_ERR_FAIL);
    wind_treefs_format(tfs);
    return W_ERR_OK;
}

SUITE_TEARDOWN(treefs) FUNC_EMPTY


TEST_CASES_START(treefs)
TEST_CASE(create)
TEST_CASE(readwrite)
TEST_CASES_END
TEST_SUITE(treefs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
