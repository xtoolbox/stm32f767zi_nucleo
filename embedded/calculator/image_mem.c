
#include <rtthread.h>
#include <rtgui/dc_hw.h>
#include <rtgui/image.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/blit.h>
#include "image_mem.h"

#define hw_driver               (rtgui_graphic_driver_get_default())

static rt_bool_t rtgui_image_mem_check(struct rtgui_filerw *file);
static rt_bool_t rtgui_image_mem_load(struct rtgui_image *image, struct rtgui_filerw *file, rt_bool_t load);
static void rtgui_image_mem_unload(struct rtgui_image *image);
static void rtgui_image_bmp_blit(struct rtgui_image *image, struct rtgui_dc *dc, struct rtgui_rect *rect);
static void rtgui_image_mem_blit(struct rtgui_image *image, struct rtgui_dc *dc, struct rtgui_rect *rect);

const struct rtgui_image_engine rtgui_image_mem_engine =
{
    "mem",
    { RT_NULL },
    rtgui_image_mem_check,
    rtgui_image_mem_load,
    rtgui_image_mem_unload,
    rtgui_image_mem_blit,
};

rt_bool_t rtgui_image_mem_check(struct rtgui_filerw *file){ return RT_TRUE; }
rt_bool_t rtgui_image_mem_load(struct rtgui_image *image, struct rtgui_filerw *file, rt_bool_t load){ return RT_TRUE; }
void rtgui_image_mem_unload(struct rtgui_image *image){}

void rtgui_image_mem_blit(struct rtgui_image *image, struct rtgui_dc *dc, struct rtgui_rect *dst_rect)
{
    const struct rtgui_image_mem_data* mem_data = (const struct rtgui_image_mem_data*)image->data;
    rt_uint32_t bytes_per_line;
    rt_uint16_t w, h;
    rt_uint16_t i;
    const rt_uint8_t* pixel_data;

    RT_ASSERT(image != RT_NULL && dc != RT_NULL && dst_rect != RT_NULL && mem_data != RT_NULL
    && (mem_data->pixel_format == hw_driver->pixel_format));
    bytes_per_line = _UI_BITBYTES(mem_data->bit_per_pixel) * image->w;
    pixel_data = mem_data->pixels;

    w = _UI_MIN(image->w, rtgui_rect_width(*dst_rect));
    h = _UI_MIN(image->h, rtgui_rect_height(*dst_rect));
    for(i=0;i<h;i++){
        dc->engine->blit_line(dc, dst_rect->x1, dst_rect->x1+w, dst_rect->y1 + i, (rt_uint8_t*)pixel_data);
        pixel_data += bytes_per_line;
    }
}



