/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: boot_pack.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: wind-os的固件打包程序
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2018.11.10
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "boot_pack.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "wind_conv.h"
#include "boot_imghead.h"
#include <stdio.h>
#include <malloc.h>

typedef struct 
{
    char input_file[128];
    w_int32_t offset;
    w_int32_t flen;
    w_uint8_t *buff;
}file_info_s;

typedef struct
{
    char       output_file[128];
    char       board_name[32];
    char       arch_name[32];
    char       cpu_name[32];
    w_uint8_t encrypt_type;
    w_uint8_t key_len;
    w_uint8_t keys[36];
    w_uint32_t hw_version;
    w_int32_t  file_cnt;
    file_info_s fileinfo[10];
}cfg_info_s;

static char cfgname[128];
static char databuff[4096000];
cfg_info_s cfg_info;

static void *wind_malloc(w_uint32_t size)
{
    return malloc(size);
}
static void wind_free(void *ptr)
{
    free(ptr);
}

static w_uint32_t read_file(char *filename,char *buff,w_int32_t size)
{
    FILE *file;
    w_uint32_t len;
    file = fopen(cfgname,"rb");
    WIND_ASSERT_RETURN(file != W_NULL,0);
    len = fread(buff,1,size,file);
    WIND_ASSERT_TODO(len > 0,fclose(file),W_ERR_FAIL);
    fclose(file);
    return len;
}


static w_int32_t get_file_lenth(char *filename)
{
    FILE*fp;
    w_int32_t flen;
    w_int8_t *p;
    fp=fopen(filename,"rb");// localfile文件名       
    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    flen=ftell(fp); /* 得到文件大小 */
    p=(char *)wind_malloc(flen+1); /* 根据文件大小动态分配内存空间 */
    if(p==NULL)
    {
        fclose(fp);
        return 0;
    }
    fseek(fp,0L,SEEK_SET); /* 定位到文件开头 */
    fread(p,flen,1,fp); /* 一次性读取全部文件内容 */
    p[flen]=0; /* 字符串结束标志 */
}




static void pack_info_init(void)
{
    w_int32_t i;
    wind_memset(cfgname,0,sizeof(cfgname));
    wind_memset(databuff,0,sizeof(databuff));
    if(cfg_info.file_cnt > 0)
    {
        for(i = 0;i < cfg_info.file_cnt;i ++)
            wind_free(cfg_info.fileinfo[i].buff);
    }
    wind_memset(&cfg_info,0,sizeof(cfg_info));
}

static w_err_t get_cfginfo(char *boradname,char *buff,w_int32_t size)
{
    w_uint32_t len;
    wind_memset(cfgname,0,sizeof(cfgname));
    wind_memset(buff,0,size);
    wind_strcpy(cfgname,boradname);
    wind_strcat(cfgname,".cfg");
    len = read_file(boradname,buff,size);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    return W_ERR_OK;    
}

char *get_line(char *buff)
{
    char *ch = buff;
    char charr[] = {'\r','\n','\t',' '};
    if(*buff == '\0')
        return W_NULL;
    buff = wind_strskip(buff,charr,sizeof(charr));
    if(buff == W_NULL)
        return W_NULL;
    ch = buff;
    for(;;)
    {
        if((*ch == '\r')||(*ch == '\n')||(*ch == '\0'))
        {
            *ch = 0;
            break;
        }
        ch ++;
    }
    if(buff[0] == '\0')
        return W_NULL;
    return buff;    
}

static w_err_t parse_hw_version(cfg_info_s *info,char *hwstr)
{
    w_int32_t i,cnt;
    w_uint32_t hw_version = 0;
    char *arr[8];
    cnt = wind_strsplit(hwstr,'.',arr,8);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(cnt <= 4,W_ERR_FAIL);
    for(i = 0;i < cnt;i ++)
    {
        hw_version << 8;
        hw_version += (*arr[i] - 0x30);
    }
    info->hw_version = hw_version;
    return W_ERR_OK;
}

static w_err_t parse_encrypt_type(cfg_info_s *info,char *hwstr)
{
    if(wind_strcmp(hwstr,"none") == 0)
        info->encrypt_type = 0;
    else if(wind_strcmp(hwstr,"rc4") == 0)
        info->encrypt_type = 1;
    else
        info->encrypt_type = 255;
    return info->encrypt_type == 255?W_ERR_FAIL:W_ERR_OK;
}

static w_err_t parse_encrypt_key(cfg_info_s *info,char *hwstr)
{
    w_int32_t i,cnt;
    char *arr[32];
    w_uint32_t value;
    cnt = wind_strsplit(hwstr,',',arr,32);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    for(i = 0;i < cnt;i ++)
    {
        WIND_ASSERT_RETURN(arr[i][0] == '0',W_ERR_FAIL);
        WIND_ASSERT_RETURN((arr[i][1] == 'x')||(arr[i][1] == 'X'),W_ERR_FAIL);
        wind_htoi(arr[i],&value);
        info->keys[i] = (w_uint8_t)value;
    }
    info->key_len = (w_uint8_t)cnt;
    return W_ERR_OK;
}

static w_err_t parse_input_file(cfg_info_s *info,char *hwstr)
{
    w_int32_t i,cnt,index;
    char *arr[4];
    w_uint32_t value;
    cnt = wind_strsplit(hwstr,',',arr,4);
    WIND_ASSERT_RETURN(cnt == 2,W_ERR_FAIL);
    WIND_ASSERT_RETURN(arr[0][0] == '0',W_ERR_FAIL);
    WIND_ASSERT_RETURN((arr[0][1] == 'x')||(arr[i][1] == 'X'),W_ERR_FAIL);
    wind_htoi(arr[0],&value);
    //info->keys[i] = (w_uint8_t)value;
    index = info->file_cnt;
    info->fileinfo[index].offset = (w_int32_t)value;
    wind_strcpy(info->fileinfo[index].input_file,arr[1]);
    info->file_cnt ++;
    return W_ERR_OK;
}


w_err_t line_parse(char *buff,cfg_info_s *info)
{
    w_int32_t cnt;
    char *str[2];
    cnt = wind_strsplit(buff,'=',str,2);
    WIND_ASSERT_RETURN(cnt == 2,W_ERR_FAIL);
    if(wind_strcmp(str[0],"arch") == 0)
        wind_strcpy(info->arch_name,str[1]);
    else if(wind_strcmp(str[0],"cpu") == 0)
        wind_strcpy(info->cpu_name,str[1]);
    else if(wind_strcmp(str[0],"board") == 0)
        wind_strcpy(info->board_name,str[1]);
    else if(wind_strcmp(str[0],"hardversion") == 0)
        parse_hw_version(info,str[1]);
    else if(wind_strcmp(str[0],"encrypt") == 0)
        parse_encrypt_type(info,str[1]);
    else if(wind_strcmp(str[0],"encryptkey") == 0)
        parse_encrypt_key(info,str[1]);
    else if(wind_strcmp(str[0],"inputfile") == 0)
        parse_input_file(info,str[1]);
    else if(wind_strcmp(str[0],"outputfile") == 0)
        wind_strcpy(info->output_file,str[1]);
    return W_ERR_OK;
}

w_err_t cfg_parse(char *buff,cfg_info_s *info)
{
    w_int32_t index,len;
    char *str;
    
    index = 0;
    str = buff;
    for(;;)
    {
        str = get_line(&str[index]);
        if(str == W_NULL)
            break;
        
        len = wind_strlen(str);
        if((len > 0)&&(str[0] != '#'))
        {
            wind_printf("str:%s\r\n",str);
            line_parse(str,info);
        }
            
        index = len + 1;
    }
    return W_ERR_OK;
}

w_int32_t pack_main(w_int32_t argc,char **argv)
{
    w_err_t err;
    wind_printf("boot pack start.\r\n");
    WIND_ASSERT_RETURN(argc >= 2,W_ERR_INVALID);
    pack_info_init();
    err = get_cfginfo(argv[1],databuff,sizeof(databuff));
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    wind_printf(databuff);
    err = cfg_parse(databuff,&cfg_info);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);

    
    system("pause");
    return 0;
}

