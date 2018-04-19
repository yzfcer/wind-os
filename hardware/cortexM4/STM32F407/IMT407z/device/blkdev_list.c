#include "wind_blkdev.h"
#include "wind_debug.h"

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
