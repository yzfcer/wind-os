#include "wind_dev.h"
#include "wind_debug.h"
#if WIND_DRVFRAME_SUPPORT

extern dev_s gpio_dev;
extern dev_s led_dev[3];

w_err_t _register_devs(void)
{
    wind_dev_register(&gpio_dev,1);
    wind_dev_register(led_dev,3);
    return ERR_OK;
}

#endif

