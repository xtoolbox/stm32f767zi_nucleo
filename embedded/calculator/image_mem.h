/*
 * File      : image_mem.h
 * COPYRIGHT (C) 2017, XToolbox.org
 *
 * In most embedded environment, we need constant resource witch are prepared at compile time
 * So I define a memory image type, it can be placed in ROM, without any variant data
 * this image type must have same color space format with the hardware driver, so it can render quickly
 */

#ifndef _IMAGE_MEM_H_
#define _IMAGE_MEM_H_

#include <rtthread.h>
#include <rtgui/dc_hw.h>
#include <rtgui/image.h>

struct rtgui_image_mem_data
{
    const rt_uint8_t bit_per_pixel;
    const rt_uint8_t pixel_format;
    const rt_uint8_t *pixels;
};

extern const struct rtgui_image_engine rtgui_image_mem_engine;

#define MAKE_MEM_IMAGE(name, w, h, data, bit_per_pixel, format) \
const struct rtgui_image_mem_data _mem_image_##name = {\
bit_per_pixel, format, data\
};\
const struct rtgui_image name = {\
    w,h, &rtgui_image_mem_engine, 0, (void*)&_mem_image_##name\
};


#endif
