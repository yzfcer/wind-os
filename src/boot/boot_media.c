/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "wind_config.h" 
#include "boot_media.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_string.h"

#ifdef __cplusplus
extern "C" {
#endif
static w_media_s *g_media[MEDIA_COUNT];
static char *get_mtype_name(w_media_type_e mtype)
{
    char *name[2] = {"RAM","ROM"};
    return name[mtype];
}

w_err_t boot_media_init(void)
{
    w_err_t err;
    wind_memset(g_media,0,sizeof(g_media));
    err = boot_medias_register();
    return err;
}


w_err_t boot_media_register(w_media_s *media)
{
    w_int32_t i;
    wind_notice("register media: %s",media->name);
    WIND_ASSERT_RETURN(media != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < MEDIA_COUNT;i ++)
    {
        if(g_media[i] == W_NULL)
        {
            g_media[i] = media;
            media->ops->init(media);
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_err_t boot_media_unregister(w_media_s *media)
{
    w_int32_t i;
    WIND_ASSERT_RETURN(media != W_NULL,W_ERR_PTR_NULL);
    for(i = 0;i < MEDIA_COUNT;i ++)
    {
        if(g_media[i] == media)
        {
            g_media[i] = W_NULL;
            return W_ERR_OK;
        }
    }
    return W_ERR_FAIL;
}

w_media_s *boot_media_get(char *name)
{
    w_int32_t i;
    WIND_ASSERT_RETURN(name != W_NULL,W_NULL);
    for(i = 0;i < MEDIA_COUNT;i ++)
    {
        if(wind_strcmp(name,g_media[i]->name) == 0)
        {
            return g_media[i];
        }
    }
    return W_NULL;
}

void boot_media_print(void)
{
    w_int32_t i;
    w_media_s *media = g_media[0];
    wind_printf("media infomation:\r\n");
    wind_print_space(9);
    wind_printf("%-12s %-8s %-10s %-10s %-10s %-8s %-8s\r\n","name","type","base","size","alloced","blksize","usage");
    wind_print_space(9);
    for(i = 0;i < MEDIA_COUNT;i ++)
    {
        media = g_media[i];
        if(media != W_NULL)
        {
            wind_printf("%-12s %-8s 0x%-8x 0x%-8x 0x%-8x %-8d %4d%%\r\n",
                media->name,get_mtype_name(media->mtype),media->base,
                media->size,media->offset,media->blksize,media->offset * 100 / media->size);
        }
    }
    wind_print_space(9);
}



#ifdef __cplusplus
}
#endif

