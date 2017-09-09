/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_mem.c
**创   建   人: 周江村
**最后修改日期: 2017.08.16
**描        述: 系统的控制台命令show处理函数，输出一些系统内部信息
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 周江村
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 周江村
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_config.h"
#include "wind_types.h"
#include "console_framework.h"
#include "wind_err.h"
#include "wind_debug.h"
#include "wind_mem.h"
#include "wind_string.h"
#include "wind_thread.h"
#include "wind_sem.h"
#include "wind_list.h"
#include "wind_var.h"

#include "console_framework.h"
cmd_s g_cmd_mem[];

static w_bool_t str2int(const char *str,w_int32_t *value)
{
    w_uint32_t temp = 0;
    const char *ptr = str;  //ptr保存str字符串开头
    while(*str != 0)
    {
        if ((*str < '0') || (*str > '9'))  //如果当前字符不是数字
            return B_FALSE;
        temp = temp * 10 + (*str - '0'); //如果当前字符是数字则计算数值
        str++;      //移到下一个字符
    }
    *value = temp;
    return B_TRUE;
}

static w_bool_t strh2int(const char *str,w_int32_t *value)
{
    w_uint32_t temp = 0;
    const char *ptr = str;  //ptr保存str字符串开头
    while(*str != 0)
    {
        if ((*str >= '0') && (*str <= '9'))  
            temp = temp * 16 + (*str - '0');
        else if ((*str >= 'a') && (*str <= 'f'))  
            temp = temp * 16 + (*str - 'a' + 10);
        else if ((*str >= 'A') && (*str <= 'F'))  
            temp = temp * 16 + (*str - 'A' + 10);
        else
            return B_FALSE;
        str++;     
    }
    *value = temp;
    return B_TRUE;
}


static w_bool_t get_num(char *str,w_uint32_t *value)
{
    if((wind_memcmp(str,"0x",2) == 0) || 
       (wind_memcmp(str,"0X",2) == 0)) 
    {
        return strh2int(&str[2],value);
    }
    else
        return str2int(str,value);
        
}

static print_mem(w_uint32_t start,w_uint32_t len)
{
    w_uint32_t i,va;
    start = ((start >> 2) << 2);
    len = ((len + 3) >> 2);
    wind_printf("memory 0x%0x %d\r\n",start,len);
    for(i = 0;i < len;i ++)
    {
        if((i & 0x03) == 0)
            wind_printf("0x%08x:  ",start+i*4);
        va = *(w_uint32_t*)((void*)(start+i*4));
        wind_printf("%08x ",va);
        if(((i+1) & 0x03) == 0)
            wind_printf("\r\n");
    }
    wind_printf("\r\n");
}

static w_bool_t display_mem(char **argv)
{
    w_uint32_t start,len;
    if(!get_num(argv[1],&start))
    {
        return ERR_COMMAN;
    }
    else if(!get_num(argv[2],&len))
    {
        return ERR_COMMAN;
    }
    print_mem(start,len);
    return ERR_OK;
}

static w_bool_t display_stack(char **argv)
{
    w_uint32_t start,len;
    pthread_s thr;
    if(wind_strcmp(argv[1],"stack") != 0)
    {
        return ERR_COMMAN;
    }
    thr = wind_thread_get_byname(argv[2]);
    if(thr == NULL)
    {
        return ERR_COMMAN;
    }
    start = (w_uint32_t)thr->pstk;
    len = ((w_uint32_t)thr->pstktop + thr->stksize*sizeof(w_stack_t) - (w_uint32_t)thr->pstk);
    wind_printf("stack start 0x%08x,size %d,cur 0x%08x,end 0x%08x\r\n",
                (w_uint32_t)thr->pstktop,thr->stksize,(w_uint32_t)thr->pstk,
                (w_uint32_t)thr->pstktop + thr->stksize*sizeof(w_stack_t));
    if(len <= 4096)
        print_mem(start,len);
    return ERR_OK;
}


w_err_t cmd_mem_main(w_int32_t argc,char **argv)
{
    pnode_s pnode;
    cmd_s *cmd = g_cmd_mem;
    w_uint32_t start,len,i,va;
    if(argc < 3)
    {
        wind_printf(cmd->helpdetails);
        return ERR_OK;
    }
    if(display_mem(argv) == ERR_OK)
        return ERR_OK;
    else if(display_stack(argv) == ERR_OK)
        return ERR_OK;
    wind_printf("command [%s] format error.",argv[0]);
    return ERR_COMMAN;
}


cmd_s g_cmd_mem[] = 
{
    {
        NULL,
        "mem",
        "show memory data values.",
        "mem <start> <lenth>:to show thread infomation.\r\n\
mem stack <threadname>:to show thread stack infomation.\r\n",
        cmd_mem_main
    }
};

void register_cmd_mem(console_s *ctrl)
{
    wind_cmd_register(&ctrl->cmd_list,g_cmd_mem,sizeof(g_cmd_mem)/sizeof(cmd_s));
}

