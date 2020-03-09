/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: tb_model_file.c
**创   建   人: Jason Zhou
**最后修改日期: 2020.3.9
**描        述: 数据表模型框架序列化功能函数即
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: v1.0
** 日　期: 2020.3.9
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 2020.3.9
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef TB_MODEL_FILE_H__
#define TB_MODEL_FILE_H__

#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "tb_model.h"
#include "wind_file.h"
#include "wind_filepath.h"

static w_bool_t is_tbmodel_filename(char *filename)
{
    w_int32_t res;
    char *suffix;
    WIND_ASSERT_RETURN(filename != W_NULL,W_FALSE);
    suffix = wind_filepath_suffix(filename);
    WIND_CHECK_RETURN(suffix != W_NULL,W_FALSE);
    res = wind_strcmp(suffix,"tbm");
    WIND_CHECK_RETURN(res == 0,W_TRUE);
    return W_FALSE;
}

w_err_t tbmodel_file_init(void)
{
    w_dlist_s *tbmlist = wind_tbmodel_get_list();
    PDLIST_INIT(tbmlist);
    return W_ERR_OK;
}
w_err_t tbmodel_file_load(char *filepath)
{
    return W_ERR_FAIL;
}

w_err_t tbmodel_file_load_all(char *dirpath)
{
    w_err_t err;
    char *fullpath = (char*)W_NULL;
    w_file_s *sub;
    w_file_s *dir = (char*)W_NULL;
    WIND_ASSERT_RETURN(dirpath != W_NULL,W_ERR_PTR_NULL);
    do
    {
        err = W_ERR_OK;
        dir = wind_fopen(dirpath,FMODE_R);
        WIND_ASSERT_BREAK(dir != W_NULL,W_ERR_NOFILE,"open dir %s failed",dirpath);
        while(1)
        {
            sub = wind_freaddir(dir);
            WIND_CHECK_BREAK(sub != W_NULL,W_ERR_OK);
            if(is_tbmodel_filename(sub->obj.name) != W_TRUE)
                continue;
            fullpath = wind_filepath_generate(dirpath,sub->obj.name,0);
            WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_FAIL,"gen file path failed");
            err = tbmodel_file_load(fullpath);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"load file %s failed",fullpath);
            wind_filepath_release(fullpath);
            fullpath = (char*)W_NULL;
        }
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"load tbmodel files failed");
    }while(0);
    if(dir != W_NULL)
        wind_fclose(dir);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    return err;
}

w_err_t tbmodel_file_save(char *filepath)
{
    return W_ERR_FAIL;
}
w_err_t tbmodel_file_save_all(char *dirpath)
{
    
    return W_ERR_FAIL;
}



#endif
