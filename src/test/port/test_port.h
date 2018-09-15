/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
** 文   件   名: test_port.h / test_port.c
** 创   建   人: 周江村
** 最后修改日期: 2015/1/24 15:42:27
** 描        述: wind_os的系统的测试框架移植接口
**  
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2015/1/24 15:42:27
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
** 本文件由C语言源文件模板软件生成。------------清风海岸出品，必属精品！------------
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#ifndef __TEST_PORT_H__
#define __TEST_PORT_H__
#include "wind_config.h"
#include "wind_type.h"
#include <wind_debug.h>
#if WIND_CUTEST_SUPPORT
#define test_printf wind_printf
//typedef unsigned int ut_uint32_t;
//typedef int w_int32_t;
void test_suite_register_all(void);
#endif
#endif
