/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_mkdir.c
** ��   ��   ��: Jason Zhou
** ����޸�����: 2018/5/13 20:24:37
** ��        ��: �ļ�ϵͳ��������
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018/5/13 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
** ���ļ���C����Դ�ļ�ģ��������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*********************************************ͷ�ļ�����***********************************************/

#if (CMD_FS_SUPPORT)
#include "wind_file.h"
/********************************************�ڲ���������**********************************************/
/********************************************�ڲ���������*********************************************/
static w_err_t mk_dir_file(w_int32_t argc,char **argv,w_uint16_t isdir)
{
    w_err_t err;
    w_bool_t isexist;
    w_file_s *file = W_NULL;
    char * fullpath;
    w_int32_t i;
    char *curpath = wind_filepath_get_current();
    if(argc < 2)
        return W_ERR_INVALID;
    do
    {
        err = W_ERR_OK;
        for(i = 1;i < argc;i ++)
        {
            fullpath = wind_filepath_generate(curpath,argv[i],isdir);
            WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_MEM,"generate fullpath failed");
            isexist = wind_fexist(fullpath);
            WIND_ASSERT_BREAK(!isexist,W_ERR_NOFILE,"directory has been existing.")
            file = wind_fopen(fullpath,FMODE_CRT);
            WIND_ASSERT_BREAK(file != W_NULL,W_ERR_FAIL,"make directory failed.")
            wind_fclose(file); 
            file = W_NULL;
            if(fullpath != W_NULL)
                wind_filepath_release(fullpath);
            fullpath = W_NULL;
        }
        WIND_CHECK_BREAK(err == W_ERR_OK,err);

    }while(0);

    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(file != W_NULL)
        wind_fclose(file); 
    return W_ERR_OK;
}

static w_err_t cmd_mkdir(w_int32_t argc,char **argv)
{
    return mk_dir_file(argc,argv,1);
}

/********************************************ȫ�ֱ�������**********************************************/
/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(mkdir)
{
    wind_printf("to make a new file \r\n");
}

COMMAND_USAGE(mkdir)
{
    wind_printf("mkdir <dir1> [dir2 [dir3 [...]]]:--to make a directory file.\r\n");
    wind_printf("dirfile : the directory taht need to  make.\r\n");
}

COMMAND_MAIN(mkdir,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"mkdir") == 0)
        return cmd_mkdir(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(mkdir);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
