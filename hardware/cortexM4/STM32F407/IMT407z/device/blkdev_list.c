#include "wind_blkdev.h"
#include "wind_debug.h"
#if 0
extern blkdev_s gpio_dev;
extern blkdev_s led0_dev;
extern blkdev_s led1_dev;
extern blkdev_s led2_dev;
#endif
extern blkdev_s memblk_dev;
extern blkdev_s spi_flash_dev[2];
extern blkdev_s at24c02_dev;

w_err_t _register_blkdevs(void)
{
    //wind_blkdev_register(&gpio_dev,1);
    wind_blkdev_register(&at24c02_dev,1);
    wind_blkdev_register(spi_flash_dev,2);
    wind_blkdev_register(&memblk_dev,1);
    return ERR_OK;
}
