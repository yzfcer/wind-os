/****************************************Copyright (c)**************************************************
**                                       清  风  海  岸
**
**                                       yzfcer@163.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: cmd_blkdev.c
**创   建   人: Jason Zhou
**最后修改日期: 2013.10.19
**描        述: 块设备操作命令
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Jason Zhou
** 版  本: v1.0
** 日　期: 2013.10.19
** 描　述: 原始版本
**
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: Jason Zhou
** 日　期: 2013.10.19
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_cmd.h"
#include "wind_blkdev.h"
#include "wind_conv.h"
#include "wind_heap.h"
#if (CMD_BLKDEV_SUPPORT)

COMMAND_DISC(blkdev)
{
    wind_printf("to operate a block device.\r\n");
}

COMMAND_USAGE(blkdev)
{
    wind_printf("blkdev list:--to show all block devices.\r\n");
    wind_printf("blkdev read  <dev> <addr> <blknum>:--to read date from the block device.\r\n");
    wind_printf("blkdev write <dev> <addr> <string>:--to write into the block device.\r\n");
    wind_printf("blkdev erase <dev> <addr> <blknum>:--to erase the block device sectors.\r\n");
}


//static w_uint8_t buffer[512];
COMMAND_MAIN(blkdev,argc,argv)
{
    w_bool_t res;
    w_blkdev_s *dev;
    w_uint8_t *buff = W_NULL;
    w_addr_t addr;
    w_int32_t cnt;
    w_err_t err;
    
    WIND_ASSERT_RETURN(argc>= 2,W_ERR_INVALID);
    if(0 == wind_strcmp(argv[1],"list"))
    {
        wind_blkdev_print();
        return W_ERR_OK;
    }

    do
    {
        err = W_ERR_OK;
        WIND_ASSERT_BREAK(argc == 5,W_ERR_INVALID,"args count error");
        dev = wind_blkdev_get(argv[2]);
        wind_blkdev_open(dev);
        WIND_ASSERT_BREAK(dev != W_NULL,W_ERR_INVALID,"open blkdev failed");
        res = wind_str_to_uint(argv[3],(w_uint32_t*)&addr);
        WIND_ASSERT_BREAK(res,W_ERR_INVALID,"addr error");
        buff = wind_malloc(dev->blksize);
        WIND_ASSERT_BREAK(buff != W_NULL,W_ERR_MEM,"mallc buff error");
        wind_memset(buff,0,dev->blksize);
        if(0 == wind_strcmp(argv[1],"read"))
        {
            cnt = wind_blkdev_read(dev,addr,buff,1);
            WIND_ASSERT_BREAK(err = cnt > 0,W_ERR_HARDFAULT,"read blkdata failed");
            wind_printf("%s",buff);
        }
        else if(0 == wind_strcmp(argv[1],"write"))
        {
            wind_strcpy((char*)buff,argv[4]);
            cnt = wind_blkdev_write(dev,addr,buff,1);
            WIND_ASSERT_BREAK(err = cnt > 0,W_ERR_HARDFAULT,"write blkdata failed");
        }
        else if(0 == wind_strcmp(argv[1],"erase"))
        {
            err = wind_blkdev_erase(dev,addr,1);
            WIND_ASSERT_BREAK(err == 0,W_ERR_HARDFAULT,"erase blkdata failed");
        }
        else 
            err = W_ERR_FAIL;
        
    }while(0);
    
    wind_blkdev_close(dev);
    if(buff != W_NULL)
        wind_free(buff);
    return err;
}

COMMAND_DEF(blkdev);

#endif

