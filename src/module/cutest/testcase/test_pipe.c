/****************************************Copyright (c)**************************************************
**                                       God's harbor
** FileName    : test_pipe.c
** Author      : Jason Zhou
** Last Date   : 2018/01/09 16:29:55
** Description : Pipe unit test suite
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
#include "wind_pipe.h"
#include "wind_string.h"
#include "wind_queue.h"

#if  TEST_PIPE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus




w_uint8_t pipebuf[128];
w_pipe_s test_pp;











CASE_SETUP(init) FUNC_EMPTY
CASE_TEARDOWN(init) FUNC_EMPTY
CASE_FUNC(init)
{
    w_err_t err;
    w_pipe_s *pipe;
    err = wind_pipe_init(&test_pp,"test",pipebuf,sizeof(pipebuf));
    EXPECT_EQ(err,W_ERR_OK);
    pipe = wind_pipe_get("test");
    EXPECT_NE(pipe,W_NULL);
    EXPECT_EQ(pipe->obj.magic,WIND_PIPE_MAGIC);
    EXPECT_STR_EQ(pipe->obj.name,"test");
	EXPECT_FALSE(IS_F_OBJ_POOL(pipe->obj));
    EXPECT_EQ(pipe->buff,pipebuf);
    EXPECT_EQ(pipe->buflen,sizeof(pipebuf));
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);

    err = wind_pipe_init(&test_pp,W_NULL,pipebuf,sizeof(pipebuf));
    EXPECT_EQ(err,W_ERR_OK);
    pipe = wind_pipe_get(W_NULL);
    EXPECT_EQ(pipe,W_NULL);
    err = wind_pipe_destroy(&test_pp);
    EXPECT_EQ(W_ERR_OK,err);
    EXPECT_EQ(test_pp.obj.magic,(~WIND_PIPE_MAGIC));
}

CASE_SETUP(info) FUNC_EMPTY
CASE_TEARDOWN(info) FUNC_EMPTY
CASE_FUNC(info)
{
    w_err_t err;
    w_pipe_s *pipe;
    pipe = wind_pipe_create("test",pipebuf,sizeof(pipebuf));
    EXPECT_NE(pipe,W_NULL);
    EXPECT_EQ(pipe->obj.magic,WIND_PIPE_MAGIC);
    EXPECT_STR_EQ(pipe->obj.name,"test");
	EXPECT_TRUE(IS_F_OBJ_POOL(pipe->obj));
    EXPECT_EQ(pipe->buff,pipebuf);
    EXPECT_EQ(pipe->buflen,sizeof(pipebuf));
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);
}




CASE_SETUP(func) FUNC_EMPTY
CASE_TEARDOWN(func) FUNC_EMPTY
CASE_FUNC(func)
{
    w_err_t err;
    w_int32_t i;
    w_int32_t res;
    w_pipe_s *pipe;
    w_int8_t buf[14];
    pipe = wind_pipe_create("test",pipebuf,sizeof(pipebuf));
    EXPECT_NE(pipe,W_NULL);
    for(i = 0;i < 10;i ++)
    {
        wind_memset(buf,0,14);
        res = wind_pipe_write(pipe,"test123456789",13);
        EXPECT_EQ(res,13);
        res = wind_pipe_read(pipe,buf,14);
        EXPECT_EQ(res,13);
        res = wind_memcmp(buf,"test123456789",13);
        EXPECT_EQ(res,0);
    }
    err = wind_pipe_destroy(pipe);
    EXPECT_EQ(W_ERR_OK,err);

}





SUITE_SETUP(pipe) FUNC_EMPTY
SUITE_TEARDOWN(pipe) FUNC_EMPTY

TEST_CASES_START(pipe)
TEST_CASE(init)
TEST_CASE(info)
TEST_CASE(func)
TEST_CASES_END
TEST_SUITE(pipe)

#ifdef __cplusplus
}
#endif
#endif 
