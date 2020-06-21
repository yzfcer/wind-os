/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : tb_model_file.c
** Author      : Jason Zhou
** Last Date   : 2020.3.9
** Description : Datasheet model framework serialization function set
**              
**--------------History---------------------------------------------------------------------------------
** Author      : 
** Version     : v1.0
** Date        : 2020.3.9
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : 
** Date        : 2020.3.9
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#include "wind_type.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "tb_model.h"
#include "wind_file.h"
#include "wind_filepath.h"
#include "db_if.h"

#define MAX_TBM_FILENAME_LEN 128
#define NODE_TO_TBMODEL(node) NODEPTR_TO_ITEMPTR(node,tb_model_s,obj.objnode)

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
    w_file_s *dir = (w_file_s*)W_NULL;
    WIND_ASSERT_RETURN(dirpath != W_NULL,W_ERR_NULL_PTR);
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

w_err_t tbmodel_file_save(char *filepath,char *modelname)
{
    return W_ERR_FAIL;
}

w_err_t tbmodel_file_save_all(char *dirpath)
{
    w_err_t err;
    w_dnode_s *dnode;
    w_dlist_s *tbmlist;
    tb_model_s *tbmodel;
    char *fullpath = (char*)W_NULL;
    char *tbm_filename = (char*)W_NULL;
    WIND_ASSERT_RETURN(dirpath != W_NULL,W_ERR_NULL_PTR);
    do
    {
        err = W_ERR_OK;
        tbmlist = wind_tbmodel_get_list();
        WIND_CHECK_BREAK(tbmlist != W_NULL,W_ERR_OK);
        tbm_filename = db_malloc(MAX_TBM_FILENAME_LEN);
        WIND_ASSERT_BREAK(tbm_filename != W_NULL,W_ERR_MEM,"alloc filename failed");
        foreach_node(dnode,tbmlist)
        {
            tbmodel = NODE_TO_TBMODEL(dnode);
            wind_memset(tbm_filename,0,MAX_TBM_FILENAME_LEN);
            wind_sprintf(tbm_filename,"%s.tbm",tbmodel->obj.name);
            fullpath = wind_filepath_generate(dirpath,tbm_filename,0);
            err = tbmodel_file_save(fullpath,tbmodel->obj.name);
            WIND_ASSERT_BREAK(err == W_ERR_OK,err,"save file %s failed",fullpath);
            wind_filepath_release(fullpath);
            fullpath = (char*)W_NULL;
        }
        WIND_ASSERT_BREAK(err == W_ERR_OK,err,"save tbmodel files failed");
    }while(0);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(tbm_filename != W_NULL)
        db_free(tbm_filename);
    return err;
}


