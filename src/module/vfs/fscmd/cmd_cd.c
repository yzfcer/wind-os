/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_cd.c
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

static w_err_t cmd_cd(w_int32_t argc,char **argv)
{
    w_err_t err;
    char *fullpath = W_NULL;
    char *curpath;
    w_file_s *file;

    do
    {
        err = W_ERR_OK;
        curpath = wind_filepath_get_current();
        if(argc >= 2)
           fullpath = wind_filepath_generate(curpath,argv[1],1);
        else
            fullpath = wind_filepath_generate(curpath,curpath,1);
        WIND_ASSERT_BREAK(fullpath != W_NULL,W_ERR_MEM,"generate fullpath failed");
        file = wind_fopen(fullpath,FMODE_R);
        WIND_ASSERT_BREAK(file != W_NULL,W_ERR_NOFILE,"directory is NOT exist.");
        WIND_ASSERT_BREAK(file->isdir == 1,W_ERR_FAIL,"%s is not a directory",fullpath);
        wind_filepath_set_current(fullpath);
        
    }while(0);
    if(fullpath != W_NULL)
        wind_filepath_release(fullpath);
    if(file != W_NULL)
        wind_fclose(file);
    return err;
}


/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(cd)
{
    wind_printf("to set current user directory.\r\n");
}

COMMAND_USAGE(cd)
{
    wind_printf("cd [dir]:--to change current user path.\r\n");
    wind_printf("    dir : the directory to set.\r\n");
}

COMMAND_MAIN(cd,argc,argv)
{
    WIND_ASSERT_RETURN(argc >= 1,W_ERR_INVALID);
    if(wind_strcmp(argv[0],"cd") == 0)
        return cmd_cd(argc,argv);
    return W_ERR_OK;
}

COMMAND_DEF(cd);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
