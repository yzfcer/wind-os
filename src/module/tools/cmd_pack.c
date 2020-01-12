/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: boot_pack.c
**创   建   人: Jason Zhou
**最后修改日期: 
**描        述: wind-os的固件打包程序
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
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

#include "wind_debug.h"
#include "wind_string.h"
#include "wind_conv.h"
#include "wind_heap.h"
#include "wind_imghead.h"
#include "wind_encrypt.h"
#include "wind_crc32.h"
#include "wind_macro.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#if CMD_PACK_SUPPORT
#include "file_port.h"

//--------------------------------------------------------------------
//--------------------------------------------------------------------

typedef struct 
{
    char input_file[128];
    w_int32_t offset;
    w_int32_t flen;
    w_uint8_t *buff;
}infile_info_s;

typedef struct
{
    char       cfgname[128];
    char       output_file[128];
    char       board_name[32];
    char       arch_name[32];
    char       cpu_name[32];
    w_uint8_t encrypt_type;
    w_uint8_t key_len;
    w_uint8_t keys[36];
    w_uint32_t hw_version;
    w_uint32_t sw_version;
    w_int32_t  file_cnt;
    infile_info_s fileinfo[10];
}pack_info_s;


static char databuff[4096];
static pack_info_s pack_info;
static w_img_head_s imghead;
static w_err_t read_bin_files(void)
{
    w_int32_t i,flen;
    infile_info_s *info;
    pack_info_s *pkinfo = &pack_info;
    for(i = 0;i < pkinfo->file_cnt;i ++)
    {
        info = &pkinfo->fileinfo[i];
        wind_notice("read file:%s",info->input_file);
        flen = read_long_file(info->input_file,0,&info->buff);
        WIND_ASSERT_RETURN(flen > 0,W_ERR_FAIL);
        info->flen = flen;
    }
    return W_ERR_OK; 
}

static void release_file_buff(void)
{
    w_int32_t i;
    pack_info_s *pkinfo = &pack_info;
    for(i = 0;i < pkinfo->file_cnt;i ++)
    {
        if(pkinfo->fileinfo[i].buff)
        {
            wind_free(pkinfo->fileinfo[i].buff);
            pkinfo->fileinfo[i].buff = W_NULL;
        }
    }    
}


static void pack_info_init(void)
{
    w_int32_t i;
    pack_info_s *pkinfo = &pack_info;
    wind_memset(databuff,0,sizeof(databuff));
    wind_memset(&imghead,0,sizeof(imghead));
    if(pkinfo->file_cnt > 0)
    {
        for(i = 0;i < pkinfo->file_cnt;i ++)
            wind_free(pkinfo->fileinfo[i].buff);
    }
    wind_memset(&pack_info,0,sizeof(pack_info));
}

static w_err_t get_cfginfo(char *boradname,char *buff,w_int32_t size)
{
    w_uint32_t len;
    pack_info_s *pkinfo = &pack_info;
    wind_strcpy(pkinfo->cfgname,boradname);
    wind_strcat(pkinfo->cfgname,".cfg");
    wind_notice("read config file:%s",pkinfo->cfgname);
    len = read_file(pkinfo->cfgname,0,buff,size);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    wind_notice("config file:%s",pkinfo->cfgname);
    return W_ERR_OK;    
}

char *get_line_from_buff(char *buff)
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

static w_err_t parse_version(w_uint32_t *version,char *versionstr)
{
    w_int32_t i,cnt;
    w_uint32_t ver = 0;
    char *arr[8];
    cnt = wind_strsplit(versionstr,'.',arr,8);
    WIND_ASSERT_RETURN(cnt > 0,W_ERR_FAIL);
    WIND_ASSERT_RETURN(cnt <= 4,W_ERR_FAIL);
    for(i = 0;i < cnt;i ++)
    {
        ver <<= 8;
        ver += (*arr[i] - 0x30);
    }
    *version = ver;
    return W_ERR_OK;
}

static w_err_t parse_encrypt_type(pack_info_s *info,char *hwstr)
{
    if(wind_strcmp(hwstr,"none") == 0)
        info->encrypt_type = 0;
    else if(wind_strcmp(hwstr,"rc4") == 0)
        info->encrypt_type = 1;
    else
        info->encrypt_type = 255;
    return info->encrypt_type == 255?W_ERR_FAIL:W_ERR_OK;
}

static w_err_t parse_encrypt_key(pack_info_s *info,char *hwstr)
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
        wind_hexstr_to_int(arr[i],&value);
        info->keys[i] = (w_uint8_t)value;
    }
    info->key_len = (w_uint8_t)cnt;
    return W_ERR_OK;
}

static w_err_t parse_input_file(pack_info_s *info,char *hwstr)
{
    w_int32_t cnt,index;
    char *arr[4];
    w_uint32_t value;
    cnt = wind_strsplit(hwstr,',',arr,4);
    WIND_ASSERT_RETURN(cnt == 2,W_ERR_FAIL);
    WIND_ASSERT_RETURN(arr[0][0] == '0',W_ERR_FAIL);
    WIND_ASSERT_RETURN((arr[0][1] == 'x')||(arr[0][1] == 'X'),W_ERR_FAIL);
    wind_hexstr_to_int(&arr[0][2],&value);
    index = info->file_cnt;
    info->fileinfo[index].offset = (w_int32_t)value;
    wind_strcpy(info->fileinfo[index].input_file,arr[1]);
    wind_notice("inputfile : %s",info->fileinfo[index].input_file);
    info->file_cnt ++;
    return W_ERR_OK;
}


w_err_t parse_line(char *buff,pack_info_s *info)
{
    w_int32_t cnt;
    char *str[2];
    cnt = wind_strsplit(buff,'=',str,2);
    WIND_ASSERT_RETURN(cnt == 2,W_ERR_FAIL);
    if(wind_strcmp(str[0],"arch") == 0)
    {
        wind_strcpy(info->arch_name,str[1]);
        wind_notice("arch  : %s",info->arch_name);
    }
    else if(wind_strcmp(str[0],"cpu") == 0)
    {
        wind_strcpy(info->cpu_name,str[1]);
        wind_notice("cpu   : %s",info->cpu_name);
    }
    else if(wind_strcmp(str[0],"board") == 0)
    {
        wind_strcpy(info->board_name,str[1]);
        wind_notice("board : %s",info->board_name);
    }
    else if(wind_strcmp(str[0],"hardversion") == 0)
    {
        parse_version(&info->hw_version,str[1]);
        wind_notice("hard_version : %d.%d.%d",DWORD_HBYTE2(info->hw_version),
            DWORD_HBYTE3(info->hw_version),DWORD_HBYTE4(info->hw_version));
    }
    else if(wind_strcmp(str[0],"softversion") == 0)
    {
        parse_version(&info->sw_version,str[1]);
        wind_notice("soft_version : %d.%d.%d",DWORD_HBYTE2(info->sw_version),
            DWORD_HBYTE3(info->sw_version),DWORD_HBYTE4(info->sw_version));
    }
    //else if(wind_strcmp(str[0],"encrypt") == 0)
    //    parse_encrypt_type(info,str[1]);
    //else if(wind_strcmp(str[0],"encryptkey") == 0)
    //    parse_encrypt_key(info,str[1]);
    else if(wind_strcmp(str[0],"inputfile") == 0)
    {
        parse_input_file(info,str[1]);
    }
    else if(wind_strcmp(str[0],"outputfile") == 0)
    {
        wind_strcpy(info->output_file,str[1]);
        wind_notice("outputfile : %s",info->output_file);
    }
    return W_ERR_OK;
}

w_err_t parse_file(char *buff,pack_info_s *info)
{
    w_int32_t index,len;
    char *str;
    
    index = 0;
    str = buff;
    for(;;)
    {
        str = get_line_from_buff(&str[index]);
        if(str == W_NULL)
            break;
        len = wind_strlen(str);
        if((len > 0)&&(str[0] != '#'))
        {
            //wind_printf("str:%s\r\n",str);
            parse_line(str,info);
        }
            
        index = len + 1;
    }
    return W_ERR_OK;
}

static w_int32_t calc_img_lenth(void)
{
    w_int32_t i,maxlen = 0;
    infile_info_s *info,*ifo;
    pack_info_s *pkinfo = &pack_info;
    for(i = 0;i < pkinfo->file_cnt;i ++)
    {
        info = &pkinfo->fileinfo[i];
        if(info->offset > maxlen)
        {
            maxlen = info->offset;
            ifo = info;
        }
    }
    maxlen += ifo->flen;
    return maxlen;
}

static w_err_t pack_files(pack_info_s *info)
{
    w_int32_t i;
    w_uint8_t *buff;
    char *img;
    w_err_t err;
    w_img_head_s *head;
    w_encypt_ctx_s ctx;
    w_uint32_t crc;
    w_uint8_t key[] = ENCRYPT_KEY;
    pack_info_s *pkinfo = &pack_info;

    infile_info_s *finfo;
    w_int32_t imglen = calc_img_lenth();
    buff = wind_malloc(imglen);
    WIND_ASSERT_RETURN(buff != W_NULL,W_ERR_MEM);
    wind_memset(buff, 0, imglen);
    WIND_ASSERT_RETURN(info->fileinfo[0].offset >= IMG_HEAD_LEN,W_ERR_FAIL);
    for(i = 0;i < pkinfo->file_cnt;i ++)
    {
        finfo = &pkinfo->fileinfo[i];
        wind_memcpy(&buff[finfo->offset],finfo->buff,finfo->flen);
    }
    if(ENCRYPT_TYPE)
    {
        wind_notice("encrypt bin file");
        pkinfo->encrypt_type = ENCRYPT_TYPE;
        wind_memcpy(pkinfo->keys,key,sizeof(key));
        pkinfo->key_len = sizeof(key);
        wind_encrypt_init(&ctx,pkinfo->keys,pkinfo->key_len);
        wind_encrypt(&ctx,&buff[IMG_HEAD_LEN],imglen - IMG_HEAD_LEN);
    }
    wind_notice("calc bin file crc");
    crc = wind_crc32(&buff[IMG_HEAD_LEN],imglen - IMG_HEAD_LEN,0xffffffff);
    wind_notice("generate img file head info");
    head = &imghead;
    head->magic = IMG_MAGIC;
    head->img_len = imglen;
    head->head_len = IMG_HEAD_LEN;
    head->head_ver = IMG_HEAD_VER;
    head->hard_ver = pkinfo->hw_version;
    head->soft_ver = pkinfo->sw_version;
    head->bin_crc = crc;
    head->bin_offset = IMG_HEAD_LEN;
    head->encrypt_type = pkinfo->encrypt_type;
    img = wind_strrchr(pkinfo->output_file, '\\');
    if(img == W_NULL)
        img = wind_strrchr(pkinfo->output_file, '/');
    if(img == W_NULL)
        img = pkinfo->output_file;
    else
        img ++;
    wind_strncpy(head->img_name, img, 64);
    wind_strncpy(head->board_name, pkinfo->board_name, 32);
    wind_strncpy(head->cpu_name, pkinfo->cpu_name, 32);
    wind_strncpy(head->arch_name, pkinfo->arch_name, 32);
    wind_img_head_set(head,buff);
    wind_notice("flush image file");
    err = write_file(pkinfo->output_file,0, buff,imglen);
    wind_free(buff);
    return err;
}

static void sort_input_file(void)
{
    w_int32_t i,j;
    infile_info_s info;
    pack_info_s *pkinfo = &pack_info;
    if(pkinfo->file_cnt <= 1)
        return;
    for(i = 0;i < pkinfo->file_cnt - 1;i ++)
    {
        for(j = i+1;j < pkinfo->file_cnt;j ++)
        {
            if(pkinfo->fileinfo[i].offset > pkinfo->fileinfo[j].offset)
            {
                wind_memcpy(&info,&pkinfo->fileinfo[i],sizeof(infile_info_s));
                wind_memcpy(&pkinfo->fileinfo[i],&pkinfo->fileinfo[j],sizeof(infile_info_s));
                wind_memcpy(&pkinfo->fileinfo[j],&info,sizeof(infile_info_s));
            }
        }
    }
    
}

static w_err_t check_file_space(void)
{
    w_int32_t i,j;
    pack_info_s *pkinfo = &pack_info;
    if(pkinfo->file_cnt <= 1)
        return W_ERR_OK;
    for(i = 0;i < pkinfo->file_cnt - 1;i ++)
    {
        for(j = i+1;j < pkinfo->file_cnt;j ++)
        {
            if(pkinfo->fileinfo[i].offset + pkinfo->fileinfo[i].flen > pkinfo->fileinfo[j].offset)
            {
                wind_error("file %s is too long",pkinfo->fileinfo[i].input_file);
                return W_ERR_FAIL;
            }
        }
    }
    wind_notice("check file space OK");
    return W_ERR_OK;
}

static w_err_t pack_files_to_img(w_int32_t argc,char **argv) 
{
    w_err_t err;
    pack_info_init();
    err = get_cfginfo(argv[2],databuff,sizeof(databuff));
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    err = parse_file(databuff,&pack_info);
    WIND_ASSERT_RETURN(err == W_ERR_OK,-1);
    sort_input_file();
    err = check_file_space();
    WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,release_file_buff(),err);
    err = read_bin_files();
    WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,release_file_buff(),err);
    err = pack_files(&pack_info);
    WIND_ASSERT_TODO_RETURN(err == W_ERR_OK,release_file_buff(),-1);
    release_file_buff();
    return W_ERR_OK;
}

w_int32_t pack_main(w_int32_t argc,char **argv)
{
    
    pack_info_s *pkinfo = &pack_info;
    wind_notice("pack firmware start.\r\n");
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"cfg") == 0)
        pack_files_to_img(argc,argv);
    else
        wind_error("error parameter");
    return 0;
}


w_int32_t pack_main(w_int32_t argc,char **argv);

COMMAND_DISC(pack)
{
    wind_printf("[*PC*] to package binary files to a image file.\r\n");
}

COMMAND_USAGE(pack)
{
    wind_printf("pack cfg <boardname>:--to pack binary files with configuration defined by boardname.cfg.\r\n");
}
extern w_int32_t pack_main(w_int32_t argc,char **argv);

COMMAND_MAIN(pack,argc,argv)
{
    return pack_main(argc,argv);
}
COMMAND_DEF(pack);
#endif

