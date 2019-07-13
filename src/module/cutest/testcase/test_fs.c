/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_fs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2018/10/04 16:29:55
** 描        述: fs文件系统接口单元测试
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2018/10/04 16:37:55
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

#if (WIND_MODULE_CUTEST_SUPPORT && TEST_FS_SUPPORT)
#include "wind_file.h"
#include "wind_fs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

CASE_SETUP(fs_create)
{

}


CASE_TEARDOWN(fs_create)
{

}

CASE_FUNC(fs_create)
{
    w_err_t err;
    w_file_s *file;
    file = wind_fopen("/test.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    file = wind_fopen("/test1.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/test1.txt");
    EXPECT_EQ(err,W_ERR_OK);
    file = wind_fopen("/test2.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/test2.txt");
    EXPECT_EQ(err,W_ERR_OK);
}


CASE_SETUP(fs_readwrite)
{
}

CASE_TEARDOWN(fs_readwrite)
{
    
}

static w_uint8_t buff[32];
CASE_FUNC(fs_readwrite)
{
    w_int32_t len;
    w_err_t err;
    w_file_s *file;
    char *str = "this is a file test string.";
    file = wind_fopen("/test.txt",FMODE_CRT | FMODE_W);
    EXPECT_NE(file,W_NULL);
    len = wind_fwrite(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);

    wind_memset(buff,0,32);
    file = wind_fopen("/test.txt",FMODE_R);
    EXPECT_NE(file,W_NULL);
    len = wind_fread(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    
}

SUITE_SETUP(fs)
{
    w_vfs_s *fs;
    fs = wind_vfs_get("treefs");
    if(fs != W_NULL)
        wind_vfs_format(fs);
}

SUITE_TEARDOWN(fs)
{

}



TEST_CASES_START(fs)
TEST_CASE(fs_create)
TEST_CASE(fs_readwrite)
TEST_CASES_END
TEST_SUITE(fs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
