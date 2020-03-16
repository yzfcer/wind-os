/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_imghead.c
**��   ��   ��: Jason Zhou
**����޸�����: 
**��        ��: wind-os��image�ļ�ͷ����������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2018.11.10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_imghead.h"
#include "wind_crc32.h"
#include "wind_conv.h"
#include "wind_string.h"
#include "wind_debug.h"
#include "wind_macro.h"
#ifdef __cplusplus
extern "C" {
#endif //#ifdef __cplusplus

static w_err_t parse_version(w_uint32_t version,char *buff,w_int32_t len)
{
    wind_memset(buff,0,len);
    wind_sprintf(buff,"%d.%d.%d",DWORD_HBYTE2(version),
        DWORD_HBYTE3(version),DWORD_HBYTE4(version));
    return W_ERR_OK;
}

void wind_img_head_print(w_img_head_s *head)
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
	parse_version(head->hard_ver,buff,sizeof(buff));
	wind_printf("hard version   : %s\r\n",buff);
    
    parse_version(head->soft_ver,buff,sizeof(buff));
    wind_printf("soft version   : %s\r\n",buff);
    if(head->encrypt_type < 2)
        wind_printf("encrypt type   : %s\r\n",encty_type[head->encrypt_type]);
    else
        wind_printf("encrypt type   : unkown\r\n");
    wind_printf("\r\n");
}

w_err_t wind_img_head_get(w_img_head_s *head,w_uint8_t *buff)
{
    w_uint32_t crc_calc,crc_read;
    w_int32_t index = 0;
    head->magic = (~IMG_MAGIC);
    wind_to_uint32(&buff[index],&head->magic);

    //�������Ӧ����bin�ļ�û�а���ͷ���ṹ
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
        head->magic = (~IMG_MAGIC);
        wind_error("img head crc_calc error.");
        return W_ERR_CRC;
    }
    wind_img_head_print(head);
    return W_ERR_OK;

}

w_err_t wind_img_head_set(w_img_head_s *head,w_uint8_t *buff)
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

    wind_strcpy((char*)&buff[index],(const char*)head->board_name);
    index += sizeof(head->board_name);
    
    crc = wind_crc32(buff,head->head_len - 4,0xffffffff);
    index = head->head_len - 4;
    wind_from_uint32(&buff[index],crc);
    return W_ERR_OK;
}

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus
