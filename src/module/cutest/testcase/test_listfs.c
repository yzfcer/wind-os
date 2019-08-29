/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_listfs.c
** 创   建   人: Jason Zhou
** 最后修改日期: 2017/10/22 16:29:55
** 描        述: listfs文件系统单元测试
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
#if (WIND_MODULE_CUTEST_SUPPORT && TEST_LISTFS_SUPPORT)
#include "listfs.h"
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

static w_uint8_t testblk[32768];
w_err_t   testblk_init(w_blkdev_s *dev)
{
    dev->blkaddr = (w_addr_t)testblk;
    dev->blkcnt = 64;
    dev->blksize = 512;
    return W_ERR_OK;
}

w_err_t   testblk_open(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

w_err_t   testblk_erase(w_blkdev_s *dev,w_addr_t addr,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr + addr * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return W_ERR_OK;
}

w_err_t   testblk_eraseall(w_blkdev_s *dev)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr + 0 * dev->blksize);
    size = dev->blkcnt * dev->blksize;
    wind_memset(start,0,size);
    return W_ERR_OK;
}


w_int32_t testblk_read(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr + addr * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memcpy(buf,start,size);
    return blkcnt;
}

w_int32_t testblk_write(w_blkdev_s *dev,w_addr_t addr,w_uint8_t *buf,w_int32_t blkcnt)
{
    w_uint8_t *start;
    w_int32_t size;
    start = (w_uint8_t *)(dev->blkaddr + addr * dev->blksize);
    size = blkcnt * dev->blksize;
    wind_memcpy(start,buf,size);
    return blkcnt;
}

w_err_t   testblk_close(w_blkdev_s *dev)
{
    return W_ERR_OK;
}

const w_blkdev_ops_s testblk_ops = 
{
    testblk_init,
    W_NULL,
    testblk_open,
    testblk_erase,
    testblk_eraseall,
    testblk_read,
    testblk_write,
    testblk_close
};

w_blkdev_s testblk_dev[1] = 
{
    WIND_BLKDEV_DEF("testblk",BLKDEV_RAM,0,0,0,0,&testblk_ops)
};

static w_err_t testblk_reg(void)
{
    wind_memset(testblk,0,sizeof(testblk));
    return wind_blkdev_register(testblk_dev,1);
}

static w_err_t testblk_unreg(void)
{
    return wind_blkdev_unregister(testblk_dev);
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
}


CASE_TEARDOWN(create)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
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
}


CASE_TEARDOWN(readdir)
{
    w_err_t err;
    err = listfile_remove(&g_lfs,"/readdir_test/");
    EXPECT_EQ(err,W_ERR_OK);

    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);

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
}

CASE_TEARDOWN(readwrite)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
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
}

CASE_TEARDOWN(format)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
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
}

SUITE_TEARDOWN(listfs)
{
    w_err_t err;
    err = lfs_deinit();
    EXPECT_EQ(err,W_ERR_OK);
    testblk_unreg();
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
