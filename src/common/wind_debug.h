/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.h
**创   建   人: Jason Zhou
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2012.10.20
** 描　述: 
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

//系统错误代码定义
#define ERR_OK                 0 //正常返回结果
#define ERR_FAIL            -1 //一般错误
#define ERR_NULL_POINTER      -2 //指针为空
#define ERR_PARAM_OVERFLOW    -3 //传入的参数超出了范围
#define ERR_MEM               -4 //存储空间受到限制
#define ERR_INVALID_PARAM     -6//参数无效
#define ERR_TIMEOUT           -7//操作超时，返回失败
#define ERR_STATUS            -8//当前状态错误
#define ERR_FILE_NOT_EXIT     -9//当前状态错误
#define ERR_OBJ_REPEAT        -10//特定条件下对象重复


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

//系统调试信息打印级别
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
//判断条件断言
#define WIND_ASSERT_RETURN(cond,res) do{if(!(cond)) {wind_error("ASSERT(%s)",#cond);return res;}}while(0)
#define WIND_ASSERT_TODO(cond,todo,res) do{if(!(cond)) {todo;return res;}}while(0)
    
        

#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

