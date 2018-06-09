/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
** ��   ��   ��: cmd_treefs.c
** ��   ��   ��: �ܽ���
** ����޸�����: 2018/5/13 20:24:37
** ��        ��: 
**  
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �ܽ���
** ��  ��: v1.0
** �ա���: 2018/5/13 20:24:37
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: �ļ�ϵͳ����
** ���ļ���C����Դ�ļ�ģ���������ɡ�------------��纣����Ʒ��������Ʒ��------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_heap.h"
#include "wind_string.h"
#include "treefs.h"

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus


/*********************************************ͷ�ļ�����***********************************************/

#if (WIND_CONSOLE_SUPPORT && WIND_FS_SUPPORT)

/********************************************�ڲ���������**********************************************/
static char *curpath = NULL;



/********************************************�ڲ���������*********************************************/


static void path_init(void)
{
    if(curpath == NULL)
    {
        curpath = treefs_malloc(2);
        curpath[0] = '/';
        curpath[1] = 0;
    }
}

static w_err_t treefs_cmd_pwd(void)
{
    console_printf("%s\r\n",curpath);
    return ERR_OK;
}

static w_err_t treefs_cmd_cd(w_int32_t argc,char **argv)
{
    w_bool_t isexist;
    char *path;
    treefile_s *file;
    tf_attr_s attr;
    if(argc < 3)
        return ERR_FAIL;
    path = treefs_get_full_path(curpath,argv[2],1);
    isexist = treefile_existing(path);
    if(!isexist)
    {
        console_printf("open directory or file failed.\r\n");
        treefs_free(path);
        return ERR_FILE_NOT_EXIT;
    }
    file = treefile_open(path,FMODE_R);
    treefile_get_attr(file,&attr);
    if(!attr.isdir)
    {
        console_printf("%s is NOT a valid directory\r\n",path);
        treefs_free(path);
        return ERR_INVALID_PARAM;
    }
    treefile_close(file);
    treefs_free(curpath);
    curpath = path;
    return ERR_OK;
}

static w_err_t mknode(w_int32_t argc,char **argv,w_uint16_t isdir)
{
    w_bool_t isexist;
    treefile_s *file;
    char * path;
    if(argc < 3)
        return ERR_INVALID_PARAM;
    path = treefs_get_full_path(curpath,argv[2],isdir);
    isexist = treefile_existing(path);
    if(isexist)
    {
        console_printf("directory has been existing.\r\n");
        treefs_free(path);
        treefile_close(file);
        return ERR_FAIL;
    }
    
    file = treefs_mk_file(path);
    if(file == NULL)
        console_printf("make directory failed.");
    treefs_free(path);
    return ERR_OK;
}

static w_err_t treefs_cmd_mkdir(w_int32_t argc,char **argv)
{
    return mknode(argc,argv,1);
}

static w_err_t treefs_cmd_touch(w_int32_t argc,char **argv)
{
    return mknode(argc,argv,0);
}

static w_err_t treefs_cmd_rm(w_int32_t argc,char **argv)
{
    w_err_t err;
    treefile_s *file;
    char * path = treefs_get_full_path(curpath,argv[2],1);
    file = treefile_open(path,FMODE_R);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n");
        treefs_free(path);
        return ERR_FILE_NOT_EXIT;
    }
    err = treefs_rm_file(file);
    if(err != ERR_OK)
        treefile_close(file);
    treefs_free(path);
    return ERR_OK;
}

static w_err_t treefs_cmd_ls(w_int32_t argc,char **argv)
{
    w_int32_t i;
    treefile_s *file,*sub;
    char * path;
    if(argc >= 3)
       path = treefs_get_full_path(curpath,argv[2],1);
    else
        path = treefs_get_full_path(curpath,curpath,1);
    file = treefile_open(path,FMODE_R);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n");
        treefs_free(path);
        return ERR_FILE_NOT_EXIT;
    }
    for(i = 0;;i ++)
    {
        sub = treefile_readdir(file,i);
        if(sub == NULL)
            break;
        console_printf("%-24s ",sub->filename);
        if(i%4 == 3)
            console_printf("\r\n");
    }
    treefile_close(file);
    treefs_free(path);
    return ERR_OK;
}

static w_err_t treefs_cmd_cat(w_int32_t argc,char **argv)
{
    treefile_s *file;
    char * path;
    w_uint8_t *buff;
    w_int32_t len;

    if(argc < 3)
        return ERR_INVALID_PARAM;
    path = treefs_get_full_path(curpath,argv[2],0);
    file = treefile_open(path,FMODE_R);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n",path);
        treefs_free(path);
        return ERR_FILE_NOT_EXIT;
    }
    buff = treefs_malloc(TREEFS_BLK_SIZE+1);
    if(buff == NULL)
    {
        treefs_free(path);
        return ERR_FAIL;
    }
    console_printf("\r\n---------%s---------\r\n",path);
    while(1)
    {
        wind_memset(buff,0,TREEFS_BLK_SIZE+1);
        len = treefile_read(file,buff,TREEFS_BLK_SIZE);
        if(len > 0)
            console_printf("%s",(char*)buff);
        else
            break;
    }
    console_printf("\r\n---------%s---------\r\n",path);
    treefile_close(file);
    treefs_free(buff);
    return ERR_OK;
}
    
static w_err_t treefs_cmd_write(w_int32_t argc,char **argv)
{
    treefile_s *file;
    char * path;
    w_int32_t len,filelen;

    if(argc < 4)
        return ERR_INVALID_PARAM;
    path = treefs_get_full_path(curpath,argv[2],0);
    file = treefile_open(path,FMODE_W|FMODE_CRT);
    if(file == NULL)
    {
        console_printf("open directory or file failed.\r\n",path);
        treefs_free(path);
        return ERR_FILE_NOT_EXIT;
    }

    len = wind_strlen(argv[3]);
    filelen = treefile_write(file,(w_uint8_t*)argv[3],len);
    treefile_close(file);
    if(filelen == len)
    {
        console_printf("write file OK.\r\n");
        treefs_free(path);
        return ERR_OK;
    }
    console_printf("write file failed.\r\n");
    treefs_free(path);
    return ERR_FAIL;
}



/********************************************ȫ�ֱ�������**********************************************/



/********************************************ȫ�ֺ�������**********************************************/
COMMAND_DISC(treefs)
{
    console_printf("to operate treefs file system.\r\n");
}

COMMAND_USAGE(treefs)
{
    console_printf("treefs pwd:to show current user path.\r\n");
    console_printf("treefs cd:to change current user path.\r\n");
    console_printf("treefs mkdir:to make a directory path.\r\n");
    console_printf("treefs touch:to make a file.\r\n");
    console_printf("treefs rm:to remove a directory or file.\r\n");
    console_printf("treefs ls:to show files in a directory.\r\n");
    console_printf("treefs cat:to show file context.\r\n");
    console_printf("treefs write:to write context into a file.\r\n");
}

COMMAND_MAIN(treefs,argc,argv)
{
    path_init();
    if(argc < 2)
        return ERR_FAIL;
    if(wind_strcmp(argv[1],"pwd") == 0)
        return treefs_cmd_pwd();
    else if(wind_strcmp(argv[1],"cd") == 0)
        return treefs_cmd_cd(argc,argv);
    else if(wind_strcmp(argv[1],"mkdir") == 0)
        return treefs_cmd_mkdir(argc,argv);
    else if(wind_strcmp(argv[1],"touch") == 0)
        return treefs_cmd_touch(argc,argv);
    else if(wind_strcmp(argv[1],"ls") == 0)
        return treefs_cmd_ls(argc,argv);
    else if(wind_strcmp(argv[1],"cat") == 0)
        return treefs_cmd_cat(argc,argv);
    else if(wind_strcmp(argv[1],"write") == 0)
        return treefs_cmd_write(argc,argv);
    else if(wind_strcmp(argv[1],"rm") == 0)
        return treefs_cmd_rm(argc,argv);
    return ERR_OK;
}

COMMAND_DEF(treefs);

#endif
#ifdef __cplusplus
}
#endif // #ifdef __cplusplus