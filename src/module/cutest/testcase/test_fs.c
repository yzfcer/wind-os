/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName: test_fs.c
** **Author: Jason Zhou
** Last Date: 2018/10/04 16:29:55
** Description: fs文件系统接口单元测试
**  
**--------------History----------------------------------------------------------------------------
**Author: Jason Zhou
** Version: v1.0
** Date: 2018/10/04 16:37:55
** Description: First version
**
**--------------Cureent version----------------------------------------------------------------------------
** Modify: 
** Date: 
** Description: 
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

CASE_SETUP(create) FUNC_EMPTY
CASE_TEARDOWN(create) FUNC_EMPTY
CASE_FUNC(create)
{
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
    file = wind_fopen("/test.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    EXPECT_STR_EQ(file->fullpath,"/test.txt");
    EXPECT_STR_EQ(file->realpath,"/test.txt");
    EXPECT_NE(file->vfs,W_NULL);
    EXPECT_NE(file->mutex,W_NULL);
    EXPECT_EQ(file->childfile,W_NULL);
    EXPECT_EQ(file->fmode,FMODE_CRT);
    EXPECT_NE(file->fileobj,W_NULL);
    EXPECT_EQ(file->isdir,0);
    EXPECT_EQ(file->offset,0);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = wind_fopen("/test1.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    EXPECT_STR_EQ(file->fullpath,"/test1.txt");
    EXPECT_STR_EQ(file->realpath,"/test1.txt");
    EXPECT_NE(file->vfs,W_NULL);
    EXPECT_NE(file->mutex,W_NULL);
    EXPECT_EQ(file->childfile,W_NULL);
    EXPECT_EQ(file->fmode,FMODE_CRT);
    EXPECT_NE(file->fileobj,W_NULL);
    EXPECT_EQ(file->isdir,0);
    EXPECT_EQ(file->offset,0);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = wind_fopen("/test.txt",FMODE_R);
    EXPECT_NE(file,W_NULL);
    EXPECT_STR_EQ(file->fullpath,"/test.txt");
    EXPECT_STR_EQ(file->realpath,"/test.txt");
    EXPECT_NE(file->vfs,W_NULL);
    EXPECT_NE(file->mutex,W_NULL);
    EXPECT_EQ(file->childfile,W_NULL);
    EXPECT_EQ(file->fmode,FMODE_R);
    EXPECT_NE(file->fileobj,W_NULL);
    EXPECT_EQ(file->isdir,0);
    EXPECT_EQ(file->offset,0);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = wind_fopen("/test1.txt",FMODE_R);
    EXPECT_NE(file,W_NULL);
    EXPECT_STR_EQ(file->fullpath,"/test1.txt");
    EXPECT_STR_EQ(file->realpath,"/test1.txt");
    EXPECT_NE(file->vfs,W_NULL);
    EXPECT_NE(file->mutex,W_NULL);
    EXPECT_EQ(file->childfile,W_NULL);
    EXPECT_EQ(file->fmode,FMODE_R);
    EXPECT_NE(file->fileobj,W_NULL);
    EXPECT_EQ(file->isdir,0);
    EXPECT_EQ(file->offset,0);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    

    err = wind_fremove("/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/test1.txt");
    EXPECT_EQ(err,W_ERR_OK);

    file = wind_fopen("/test.txt",FMODE_R);
    EXPECT_EQ(file,W_NULL);
    file = wind_fopen("/test1.txt",FMODE_R);
    EXPECT_EQ(file,W_NULL);


}

CASE_SETUP(exist) FUNC_EMPTY
CASE_TEARDOWN(exist) FUNC_EMPTY
CASE_FUNC(exist)
{
    w_bool_t exist;
    w_file_s *file = (w_file_s *)W_NULL;
    w_err_t err;
    file = wind_fopen("/exist.txt",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    exist = wind_fexist("/exist.txt");
    EXPECT_EQ(exist,W_TRUE);
   
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = wind_fremove("/exist.txt");
    EXPECT_EQ(err,W_ERR_OK);

    exist = wind_fexist("/exist.txt");
    EXPECT_EQ(exist,W_FALSE);

}


CASE_SETUP(readwrite) FUNC_EMPTY
CASE_TEARDOWN(readwrite) FUNC_EMPTY
static w_uint8_t buff[32];
CASE_FUNC(readwrite)
{
    w_int32_t len;
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
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


CASE_SETUP(readdir)
{
    w_file_s *file = (w_file_s *)W_NULL;
    file = wind_fopen("/readdir_test/",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    wind_fclose(file);
    
    file = wind_fopen("/readdir_test/test1",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    wind_fclose(file);
    
    file = wind_fopen("/readdir_test/test2",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    wind_fclose(file);
    
    file = wind_fopen("/readdir_test/testdir/",FMODE_CRT);
    EXPECT_NE(file,W_NULL);
    wind_fclose(file);
    return W_ERR_OK;
    
}

CASE_TEARDOWN(readdir)
{
    w_err_t err;
    err = wind_fremove("/readdir_test/");
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(readdir)
{
    w_err_t err;
    w_file_s *file = (w_file_s *)W_NULL;
    w_file_s *sub = (w_file_s *)W_NULL;
    file = wind_fopen("/readdir_test/",FMODE_R);
    EXPECT_NE(file,W_NULL);
    EXPECT_STR_EQ(file->fullpath,"/readdir_test/");
    EXPECT_STR_EQ(file->realpath,"/readdir_test/");
    EXPECT_NE(file->vfs,W_NULL);
    EXPECT_NE(file->mutex,W_NULL);
    EXPECT_EQ(file->childfile,W_NULL);
    EXPECT_EQ(file->fmode,FMODE_R);
    EXPECT_NE(file->fileobj,W_NULL);
    EXPECT_EQ(file->isdir,1);
    EXPECT_EQ(file->offset,0);

    
    sub = wind_freaddir(file);
    EXPECT_NE(sub,W_NULL);
    sub = wind_freaddir(file);
    EXPECT_NE(sub,W_NULL);
    sub = wind_freaddir(file);
    EXPECT_NE(sub,W_NULL);
    sub = wind_freaddir(file);
    EXPECT_EQ(sub,W_NULL);
    err = wind_fclose(file);
    EXPECT_EQ(err,W_ERR_OK);
}

SUITE_SETUP(fs) FUNC_EMPTY
SUITE_TEARDOWN(fs) FUNC_EMPTY




TEST_CASES_START(fs)
TEST_CASE(create)
TEST_CASE(exist)
TEST_CASE(readwrite)
TEST_CASE(readdir)
TEST_CASES_END
TEST_SUITE(fs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
