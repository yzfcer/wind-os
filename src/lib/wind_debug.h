/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_debug.h
**创   建   人: 周江村
**最后修改日期: 2012.09.26
**描        述: 调试信息的输出方式头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2012.09.26
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2012.10.20
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/

#ifndef WIND_DEBUG_H__
#define WIND_DEBUG_H__

#include "wind_config.h"
#include "wind_os_hwif.h"

#ifdef __cplusplus
extern "C" {
#endif
#define WIND_ERROR_OUT_EN 1 //允许输出错误信息
#define WIND_INFO_OUT_EN  1 //允许输出普通的消息信息
#define WIND_WARN_OUT_EN  1 //允许输出警告信息
#define WIND_DEBUG_OUT_EN 0 //允许调试信息输出


//void wind_printf(const w_int8_t *string,...);
int wind_printf(const char *fmt, ...);
//WIND_DEBUG
#if WIND_DEBUG_OUT_EN
#define WIND_DEBUG(fmt,...) do{wind_printf("debug:"fmt" [%s,%d]\r\n",##__VA_ARGS__,__FILE__,__LINE__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_DEBUG(fmt,...)
#endif

//WIND_INFO
#if WIND_INFO_OUT_EN
#define WIND_INFO(...) do{wind_printf(__VA_ARGS__);}while(0)
#else 
#define WIND_INFO(...) 
#endif

//WIND_WARN
#if WIND_WARN_OUT_EN
#define WIND_WARN(...) do{wind_printf(__VA_ARGS__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_WARN(...)
#endif

//WIND_ERROR
#if WIND_ERROR_OUT_EN
#define WIND_ERROR(...) do{wind_printf(__VA_ARGS__);wind_printf("[%s,%d]\r\n",__FILE__,__LINE__);}while(0)
#else 
#define WIND_ERROR(...)
#endif


#ifdef __cplusplus
}
#endif

#endif  //WIND_DEBUG_H__

