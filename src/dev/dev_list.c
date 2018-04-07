#include "wind_dev.h"
#include "wind_debug.h"
extern dev_s gpio_dev;

w_err_t _register_devs(void)
{
    wind_register_dev(&gpio_dev,1);
    return ERR_OK;
}
