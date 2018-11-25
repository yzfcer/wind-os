/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: boot_imghead.c
**创   建   人: 周江村
**最后修改日期: 
**描        述: wind-boot的image文件头部处理函数
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
#include "boot_imghead.h"
#include "wind_crc32.h"
#include "wind_conv.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_macro.h"
static w_err_t parse_version(w_uint32_t version,char *buff,w_int32_t len)
{
    wind_memset(buff, 0, len);
    wind_sprintf(buff, "%d.%d.%d",DWORD_HBYTE2(version),
        DWORD_HBYTE3(version),DWORD_HBYTE4(version));
    return W_ERR_OK;
}

void boot_img_head_print(img_head_s *head)
{
    static char *encty_type[4] = 
    {
        "no encrypt",
        "RC4"
    };
    char buff[16];
    wind_printf("img head info:\r\n");
    wind_printf("board name     : %s\r\n",(char*)head->board_name);
    wind_printf("cpu arch       : %s\r\n",(char*)head->arch_name);
    wind_printf("CPU model      : %s\r\n",(char*)head->cpu_name);
    wind_printf("img file name  : %s\r\n",(char*)head->img_name);
    wind_printf("img file lenth : %d\r\n",head->img_len);
	parse_version(head->hard_ver, buff,sizeof(buff));
	wind_printf("hard version   : %s\r\n",buff);
    
    parse_version(head->soft_ver, buff,sizeof(buff));
    wind_printf("soft version   : %s\r\n",buff);
    if(head->encrypt_type < 2)
        wind_printf("encrypt type   : %s\r\n",encty_type[head->encrypt_type]);
    else
        wind_printf("encrypt type   : unkown\r\n");
    wind_printf("\r\n");
}

w_err_t boot_img_head_get(img_head_s *head,w_uint8_t *buff)
{
    w_uint32_t crc_calc,crc_read;
    w_int32_t index = 0;
    head->magic = 0;
    wind_to_uint32(&buff[index],&head->magic);

    //这种情况应该是bin文件没有包含头部结构
    //if(head->magic != IMG_MAGIC)
    //    return W_ERR_OK;
    index += 4;
    wind_to_uint32(&buff[index],&head->img_len);
    index += 4;
    wind_to_uint32(&buff[index],&head->head_len);
    index += 4;
    wind_to_uint32(&buff[index],&head->head_ver);
    index += 4;
    wind_to_uint32(&buff[index],&head->hard_ver);
    index += 4;
    wind_to_uint32(&buff[index],&head->soft_ver);
    index += 4;
    wind_to_uint32(&buff[index],&head->bin_crc);
    index += 4;
    wind_to_uint32(&buff[index],&head->bin_offset);
    index += 4;
    wind_to_uint32(&buff[index],&head->encrypt_type);
    index += 4;
    wind_memset(head->img_name,0,sizeof(head->img_name));
    wind_strcpy(head->img_name,(const char*)&buff[index]);
    index += sizeof(head->img_name);

    wind_memset(head->arch_name,0,sizeof(head->arch_name));
    wind_strcpy(head->arch_name,(const char*)&buff[index]);
    index += sizeof(head->arch_name);

    wind_memset(head->cpu_name,0,sizeof(head->cpu_name));
    wind_strcpy(head->cpu_name,(const char*)&buff[index]);
    index += sizeof(head->cpu_name);
    
    wind_memset(head->board_name,0,sizeof(head->board_name));
    wind_strcpy(head->board_name,(const char*)&buff[index]);
    index += sizeof(head->board_name);

    wind_to_uint32(&buff[head->head_len-4],&crc_read);
    crc_calc = wind_crc32(buff,head->head_len - 4,0xffffffff);
    if(crc_calc != crc_read)
    {
        head->magic = 0;
        wind_error("img head crc_calc error.");
        return W_ERR_INVALID;
    }
    boot_img_head_print(head);
    return W_ERR_OK;

}

w_err_t boot_img_head_set(img_head_s *head,w_uint8_t *buff)
{
    w_uint32_t crc;
    w_int32_t index = 0;
    wind_memset(buff,0,head->head_len);
    wind_from_uint32(&buff[index],head->magic);

    index += 4;
    wind_from_uint32(&buff[index],head->img_len);
    index += 4;
    wind_from_uint32(&buff[index],head->head_len);
    index += 4;
    wind_from_uint32(&buff[index],head->head_ver);
    index += 4;
    wind_from_uint32(&buff[index],head->hard_ver);
    index += 4;
    wind_from_uint32(&buff[index],head->soft_ver);
    index += 4;
    wind_from_uint32(&buff[index],head->bin_crc);
    index += 4;
    wind_from_uint32(&buff[index],head->bin_offset);
    index += 4;
    wind_from_uint32(&buff[index],head->encrypt_type);
    index += 4;
    wind_strcpy((char*)&buff[index],(const char*)head->img_name);
    index += sizeof(head->img_name);

    wind_strcpy((char*)&buff[index],(const char*)head->arch_name);
    index += sizeof(head->arch_name);

    wind_strcpy((char*)&buff[index],(const char*)head->cpu_name);
    index += sizeof(head->cpu_name);

    wind_strcpy(&buff[index],(const char*)head->board_name);
    index += sizeof(head->board_name);
    
    crc = wind_crc32(buff,head->head_len - 4,0xffffffff);
    index = head->head_len - 4;
    wind_from_uint32(&buff[index],crc);
    return W_ERR_OK;
}

