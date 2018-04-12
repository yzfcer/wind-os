#include "wind_dev.h"
#include "wind_debug.h"
extern dev_s gpio_dev;
extern dev_s led0_dev;
extern dev_s led1_dev;
extern dev_s led2_dev;

w_err_t _register_devs(void)
{
    wind_register_dev(&gpio_dev,1);
    wind_register_dev(&led0_dev,1);
    wind_register_dev(&led1_dev,1);
    wind_register_dev(&led2_dev,1);
    return ERR_OK;
}
