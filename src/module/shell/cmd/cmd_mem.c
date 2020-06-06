/****************************************Copyright (c)**************************************************
**                                       God's harbor
**
**                                       yzfcer@163.com
**
**--------------File infomation-------------------------------------------------------------------------
** FileName    : cmd_mem.c
** Author      : Jason Zhou
** Last Date   : 2017.08.16
** Description : Commands for viewing memory area values for a system
**              
**--------------History---------------------------------------------------------------------------------
** Author      : Jason Zhou
** Version     : v1.0
** Date        : 2013.10.19
** Description : First version
**
**--------------Cureent version-------------------------------------------------------------------------
** Modify      : Jason Zhou
** Date        : 2013.10.19
** Description : 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_string.h"
#include "wind_thread.h"
#if (CMD_MEM_SUPPORT)

static w_bool_t str2int64(const char *str,w_uint64_t *value)
{
    w_uint64_t temp = 0;
    while(*str != 0)
    {
        if ((*str < '0') || (*str > '9'))  
            return W_FALSE;
        temp = temp *10 + (*str - '0');
        str ++;
    }
    *value = temp;
    return W_TRUE;
}

static w_bool_t strh2int64(const char *str,w_uint64_t *value)
{
    w_uint64_t temp = 0;
    while(*str != 0)
    {
        if ((*str >= '0') && (*str <= '9'))  
            temp = temp *16 + (*str - '0');
        else if ((*str >= 'a') && (*str <= 'f'))  
            temp = temp *16 + (*str - 'a' + 10);
        else if ((*str >= 'A') && (*str <= 'F'))  
            temp = temp *16 + (*str - 'A' + 10);
        else
            return W_FALSE;
        str++;     
    }
    *value = temp;
    wind_printf("value:%p\r\n",value);
    return W_TRUE;
}


static w_bool_t get_num(char *str,w_uint32_t *value)
{
    w_bool_t res;
	w_uint64_t tmpva = 0;
    if((wind_memcmp(str,"0x",2) == 0) || 
       (wind_memcmp(str,"0X",2) == 0)) 
    {
        res = strh2int64(&str[2],&tmpva);
    }
    else
        res = str2int64(str,&tmpva);
    if(res)
        *value = (w_uint32_t)tmpva;
    return res;
}

static w_bool_t get_addr(char *str,w_addr_t *value)
{
    w_bool_t res;
	w_uint64_t tmpva = 0;
    if((wind_memcmp(str,"0x",2) == 0) || 
       (wind_memcmp(str,"0X",2) == 0)) 
    {
        res = strh2int64(&str[2],&tmpva);
    }
    else
        res = str2int64(str,&tmpva);
    if(res)
        *value = (w_addr_t)tmpva;
    return res;
}

static void print_mem(w_addr_t start,w_uint32_t len)
{
    w_uint32_t i,j,va;
    char *strptr;
    start = ((start >> 2) << 2);
    len = ((len + 3) >> 2);
    wind_printf("memory addr: %p,size: %d\r\n",start,len);
    for(i = 0;i < len;i ++)
    {
        if((i & 0x03) == 0)
        {
            wind_printf("%p:  ",start+i*4);
            strptr = (char*)(start+i*4);
        }
            
        va = *(w_addr_t*)((void*)(start+i*4));
		wind_printf("%08x ",va);
        if(((i+1) & 0x03) == 0)
        {
            wind_printf("        ");
            for(j = 0;j < 16;j ++)
                wind_printf("%c",(strptr[j] >= 0x20 && strptr[j] < 0x7f)?strptr[j]:'.');
            wind_printf("\r\n");
        }
            //wind_printf("\r\n");
    }
    wind_printf("\r\n");
}

static w_err_t display_mem(w_int32_t argc,char **argv)
{
    w_uint32_t len;
    w_addr_t start;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(!get_addr(argv[1],&start))
    {
        return W_ERR_FAIL;
    }
    else if(!get_num(argv[2],&len))
    {
        return W_ERR_FAIL;
    }
    print_mem(start,len);
    return W_ERR_OK;
}

w_err_t display_stack(w_int32_t argc,char **argv)
{
    w_thread_s *thread;
    WIND_ASSERT_RETURN(argc >= 3,W_ERR_INVALID);
    if(wind_strcmp(argv[1],"stack") != 0)
    {
        return W_ERR_FAIL;
    }
    thread = wind_thread_get(argv[2]);
    if(thread == W_NULL)
    {
        return W_ERR_FAIL;
    }
    return wind_thread_print_stack(thread);
    
}



COMMAND_DISC(mem)
{
    wind_printf("to show memory data values.\r\n");
}

COMMAND_USAGE(mem)
{
    wind_printf("--WARNING--:this command is NOT safe for product.\r\n");
    wind_printf("mem <start> <lenth>:--to show thread infomation.\r\n");
    wind_printf("mem stack <threadname>:--to show thread stack infomation.\r\n");
}

COMMAND_MAIN(mem,argc,argv)
{
    if(display_mem(argc,argv) == W_ERR_OK)
        return W_ERR_OK;
    else if(display_stack(argc,argv) == W_ERR_OK)
        return W_ERR_OK;
    wind_printf("command [%s] format error.\r\n",argv[0]);
    return W_ERR_FAIL;
}

COMMAND_DEF(mem);

#endif

