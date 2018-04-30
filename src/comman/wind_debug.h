/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_debug.h
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: ������Ϣ�������ʽͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2012.09.26
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2012.10.20
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_DEBUG_H__
#define WIND_DEBUG_H__
#include "wind_config.h"
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif

//ϵͳ������붨��
#define ERR_OK                 0 //�������ؽ��
#define ERR_COMMAN            -1 //һ�����
#define ERR_NULL_POINTER      -2 //ָ��Ϊ��
#define ERR_PARAM_OVERFLOW    -3 //����Ĳ��������˷�Χ
#define ERR_MEM               -4 //�洢�ռ��ܵ�����
#define ERR_INVALID_PARAM     -6//������Ч
#define ERR_TIMEOUT           -7//������ʱ������ʧ��
#define ERR_STATUS            -8//��ǰ״̬����
#define ERR_FILE_NOT_EXIT     -9//��ǰ״̬����
#define ERR_OBJ_REPEAT        -10//�ض������¶����ظ�


//---------------------------------------------------------------------
#if WIND_DEBUG_SUPPORT
#ifdef _USE_USER_PRINT
extern w_int32_t wind_std_output(w_uint8_t *str,w_int32_t len);
w_int32_t wind_printf(const char *fmt, ...);
#else
#include <stdio.h>
#define wind_printf printf
#endif
#else
#define wind_printf
#endif

//ϵͳ������Ϣ��ӡ����
#define PRINT_LV_DEBUG   1
#define PRINT_LV_NOTICE  2
#define PRINT_LV_WARN    3
#define PRINT_LV_ERROR   4
#define PRINT_LV_CRIT     5

#define PRINT_LEVEL PRINT_LV_NOTICE


#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_DEBUG))
#define wind_debug(fmt,...) do{/*wind_printf("[debug] ");*/wind_printf("%s"fmt"\r\n","[debug] ",##__VA_ARGS__);}while(0)
#else 
#define wind_debug(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_NOTICE))
#define wind_notice(fmt,...) do{/*wind_printf("[notice] ");*/wind_printf("%s"fmt"\r\n","[notice] ",##__VA_ARGS__);}while(0)
#else 
#define wind_notice(fmt,...) 
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_WARN))
#define wind_warn(fmt,...) do{/*wind_printf("[warn] ");*/wind_printf("%s[%s,%d] "fmt"\r\n","[warn] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_warn(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_ERROR))
#define wind_error(fmt,...) do{/*wind_printf("[error] ");*/wind_printf("%s[%s,%d] "fmt"\r\n","[error] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_error(fmt,...)
#endif

#if (WIND_DEBUG_SUPPORT && (PRINT_LEVEL <= PRINT_LV_CRIT))
#define wind_critical(fmt,...) do{/*wind_printf("[critical] ");*/wind_printf("%s[%s,%d] "fmt"\r\n","[critical] ",__FUNCTION__,__LINE__,##__VA_ARGS__);}while(0)
#else 
#define wind_critical(fmt,...)
#endif



//---------------------------------------------------------------------
//�ж���������
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo,res) do{if(!(cond)) {todo;return res;}}while(0)
    
        

#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

