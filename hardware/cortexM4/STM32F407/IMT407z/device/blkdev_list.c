#include "wind_blkdev.h"
#include "wind_debug.h"
#if 0
extern blkdev_s gpio_dev;
extern blkdev_s led0_dev;
extern blkdev_s led1_dev;
extern blkdev_s led2_dev;
#endif
extern blkdev_s memblk_dev;
extern blkdev_s spi_flash_dev;

w_err_t _register_blkdevs(void)
{
    //wind_register_blkdev(&gpio_dev,1);
    //wind_register_blkdev(&led0_dev,1);
    wind_register_blkdev(&spi_flash_dev,1);
    wind_register_blkdev(&memblk_dev,1);
    return ERR_OK;
}
