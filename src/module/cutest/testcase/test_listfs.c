/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_listfs.c
** Author      : Jason Zhou
** Last Date   : 2017/10/22 16:29:55
** Description : listfs文件系统单元测试
**  
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2017/10/22 16:29:55
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 
** Description : 
** 
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cut.h"
#include "wind_string.h"
#include "listfs.h"
#include "wind_memblk.h"
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_LISTFS_SUPPORT)
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_uint8_t testblk[64*512];
static w_blkdev_s testblkdev;



static w_err_t testblk_reg(void)
{
    return wind_memblk_create(&testblkdev,"testblk",testblk,sizeof(testblk),512);
}

static w_err_t testblk_unreg(void)
{
    return wind_memblk_destroy(&testblkdev);
}


//=================================================================
w_listfs_s g_lfs;

static w_err_t lfs_init(void)
{
    w_blkdev_s *blkdev;
    w_err_t err = W_ERR_OK;
    if(g_lfs.lfs_info.magic == LISTFS_MAGIC)
    {
        err = listfs_deinit(&g_lfs);
        EXPECT_EQ(err,W_ERR_OK);
    }
    blkdev = wind_blkdev_get("testblk");
    if(blkdev == W_NULL)
    {
        wind_memset(&g_lfs,0,sizeof(w_listfs_s));
        return W_ERR_FAIL;
    }
        
    wind_blkdev_open(blkdev);
    wind_memset(&g_lfs,0,sizeof(g_lfs));
    err = listfs_init(&g_lfs,blkdev);
    if(err != W_ERR_OK)
    {
        err = listfs_format(&g_lfs,blkdev);
        EXPECT_EQ(err,W_ERR_OK);
    }
    return err;
        
}

static w_err_t lfs_deinit(void)
{
    w_err_t err = W_ERR_OK;
    if(g_lfs.lfs_info.magic == LISTFS_MAGIC)
    {
        err = listfs_deinit(&g_lfs);
        EXPECT_EQ(err,W_ERR_OK);
        g_lfs.lfs_info.magic = 0;
    }
    return err;
}

CASE_SETUP(create)
{
    w_err_t err;
    err = lfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}


CASE_TEARDOWN(create)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(create)
{
    w_err_t err;
    w_listfile_s *file;
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_R);
    EXPECT_EQ(file,W_NULL);

    file = listfile_open(&g_lfs,"/test1.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = listfile_open(&g_lfs,"/test2.txt",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    file = listfile_open(&g_lfs,"/test1.txt",LFMODE_R);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    file = listfile_open(&g_lfs,"/test2.txt",LFMODE_R);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    
    err = listfile_remove(&g_lfs,"/test1.txt");
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test2.txt");
    EXPECT_EQ(err,W_ERR_OK);
}

CASE_SETUP(readdir)
{
    w_err_t err;
    err = lfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}


CASE_TEARDOWN(readdir)
{
    w_err_t err;
    err = listfile_remove(&g_lfs,"/readdir_test/");
    EXPECT_EQ(err,W_ERR_OK);

    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(readdir)
{
    w_err_t err;
    w_listfile_s *file;
    w_listfile_s *sub = W_NULL;

    file = listfile_open(&g_lfs,"/readdir_test/",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = listfile_open(&g_lfs,"/readdir_test/test1",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = listfile_open(&g_lfs,"/readdir_test/test2",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    
    file = listfile_open(&g_lfs,"/readdir_test/testdir/",LFMODE_CRT);
    EXPECT_NE(file,W_NULL);
    err = listfile_close(file);
    EXPECT_NE(file,W_NULL);

	file = listfile_open(&g_lfs,"/readdir_test/",LFMODE_R);
    EXPECT_NE(file,W_NULL);
    EXPECT_EQ(file->info.magic,LISTFILE_MAGIC);
    EXPECT_STR_EQ(file->info.name,"readdir_test");
    EXPECT_EQ(file->info.filesize,0);
    EXPECT_EQ(file->info.spacesize,0);
    EXPECT_EQ(file->info.parent_addr,file->lfs->lfs_info.root_addr);
    EXPECT_NE(file->info.self_addr,0);
    EXPECT_EQ(file->info.last_addr,0);
    EXPECT_EQ(file->info.last_addr,0);
    EXPECT_EQ(file->info.nextfile_addr,0);
    EXPECT_EQ(file->info.children_cnt,3);
    EXPECT_NE(file->info.headchild_addr,0);
    EXPECT_NE(file->info.tailchild_addr,0);
    EXPECT_NE(IS_LFILE_ATTR_DIR(file->info.attr),0);

    
    err = listfile_readdir(file,&sub);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_readdir(file,&sub);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_readdir(file,&sub);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_readdir(file,&sub);
    EXPECT_NE(err,W_ERR_OK);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
}


CASE_SETUP(readwrite)
{
    w_err_t err;
    err = lfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_TEARDOWN(readwrite)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

static w_uint8_t buff[32];
CASE_FUNC(readwrite)
{
    w_int32_t len;
    w_err_t err;
    w_listfile_s *file;
    char *str = "this is a file test string.";

	file = listfile_open(&g_lfs,"/test.txt",LFMODE_CRT | LFMODE_W);
    EXPECT_NE(file,W_NULL);
    len = listfile_write(file,(w_uint8_t*)str,wind_strlen(str));
    EXPECT_EQ(len,wind_strlen(str));
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);

    wind_memset(buff,0,32);
    file = listfile_open(&g_lfs,"/test.txt",LFMODE_R);
    EXPECT_NE(file,W_NULL);
    len = listfile_read(file,buff,32);
    EXPECT_EQ(len,wind_strlen(str));
    len = wind_strcmp(str,(char*)buff);
    EXPECT_EQ(len,0);
    err = listfile_close(file);
    EXPECT_EQ(err,W_ERR_OK);
    err = listfile_remove(&g_lfs,"/test.txt");
    EXPECT_EQ(err,W_ERR_OK);
    
}


CASE_SETUP(format)
{
    w_err_t err;
    err = lfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_TEARDOWN(format)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

CASE_FUNC(format)
{

}

SUITE_SETUP(listfs)
{
    w_err_t err;
    testblk_reg();
    err = lfs_init();
    EXPECT_EQ(err,W_ERR_OK);
    return W_ERR_OK;
}

SUITE_TEARDOWN(listfs)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    testblk_unreg();
    return W_ERR_OK;
}



TEST_CASES_START(listfs)
TEST_CASE(format)
TEST_CASE(create)
TEST_CASE(readdir)
TEST_CASE(readwrite)
TEST_CASES_END
TEST_SUITE(listfs)

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif
