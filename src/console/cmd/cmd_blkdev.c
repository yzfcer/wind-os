/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: wind_blkdev.c
**创   建   人: 周江村
**最后修改日期: 2013.10.19
**描        述: 系统的控制台命令stat处理函数，统计资源使用情况
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
#include "wind_type.h"
#include "wind_debug.h"
#include "wind_cmd.h"
#include "wind_blkdev.h"
#include "wind_conv.h"
#include "wind_heap.h"
#if WIND_CONSOLE_SUPPORT


COMMAND_DISC(blkdev)
{
    console_printf("conreol one block device.\r\n");
}

COMMAND_USAGE(blkdev)
{
    console_printf("blkdev <dev> read <addr> <blknum>:to read date from the block device.\r\n");
    console_printf("blkdev <dev> write <addr> <string>:to write into the block device.\r\n");
    console_printf("blkdev <dev> erase <addr> <blknum>:to erase the block device sectors.\r\n");
}


static w_uint8_t buffer[512];
COMMAND_MAIN(blkdev,argc,argv)
{
    blkdev_s *dev;
    w_uint8_t *buff;
    w_addr_t addr;
    w_err_t err = ERR_COMMAN;
    
    WIND_ASSERT_RETURN(argc == 5,ERR_INVALID_PARAM);
    dev = wind_blkdev_get(argv[1]);
    wind_blkdev_open(dev);
    WIND_ASSERT_RETURN(dev != NULL,ERR_INVALID_PARAM);
    addr = (w_addr_t)wind_convert_str2u32_t(argv[3]);
    buff = buffer;//wind_heap_alloc_default(dev->blksize);
    wind_memset(buff,0,dev->blksize);
    if(0 == wind_strcmp(argv[2],"read"))
    {
        err = wind_blkdev_read(dev,addr,buff,1);
        wind_printf("%s",buff);
        wind_blkdev_close(dev);
        err = ERR_OK;
    }
    else if(0 == wind_strcmp(argv[2],"write"))
    {
        //wind_blkdev_open(dev);
        wind_strcpy((char*)buff,argv[4]);
        err = wind_blkdev_write(dev,addr,buff,1);
        wind_blkdev_close(dev);
        err = ERR_OK;
    }
    else if(0 == wind_strcmp(argv[2],"write"))
    {
        //wind_blkdev_open(dev);
        err = wind_blkdev_erase(dev,addr,1);
        wind_blkdev_close(dev);
        err = ERR_OK;
    }
    else ;
    //wind_heap_free(buff);
    return err;
}

COMMAND_DEF(blkdev);

#endif

