#include "drv_usb_lcd.h"
#include "rtthread.h"
#include <rtgui/event.h>
#include <rtgui/rtgui.h>
#include <rtgui/driver.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>

static uint16_t  usb_lcd_mem_buffer[WIDTH*HEIGHT];
extern int update_image(int color);
uint32_t get_usb_lcd_address(void)
{
	return (uint32_t)usb_lcd_mem_buffer;
}

static rt_err_t lcd_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	switch (cmd)
	{
	case RTGRAPHIC_CTRL_GET_INFO:
	{
		struct rt_device_graphic_info *info;

		info = (struct rt_device_graphic_info*) args;
		RT_ASSERT(info != RT_NULL);

		info->bits_per_pixel = 16;
		info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
		info->framebuffer = RT_NULL;//(rt_uint8_t*)usb_lcd_mem_buffer;
		info->width = WIDTH;
		info->height = HEIGHT;
	}
	break;

        case RTGRAPHIC_CTRL_RECT_UPDATE:
            /* nothong to be done */
            update_image(1);
            break;

	default:
		return RT_ERROR;
	}

	return RT_EOK;
}

static void usb_lcd_set_pixel(const char* pixel, int x, int y)
{
	 usb_lcd_mem_buffer[x+y*WIDTH] = *(uint16_t *)pixel;
}
static void usb_lcd_get_pixel(char* pixel, int x, int y)
{
	 *(uint16_t *)pixel = usb_lcd_mem_buffer[x+y*WIDTH];
}

static void usb_lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
	uint16_t* p = &usb_lcd_mem_buffer[y*WIDTH];
	for (; x1 < x2; x1++)
	{
		p[x1] = *(uint16_t *)pixel;
	}
}
static void usb_lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
	for (; y1 < y2; y1++)
	{
		usb_lcd_mem_buffer[x+y1*WIDTH] = *(uint16_t *)pixel;
	}
}

static void usb_lcd_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
	rt_uint16_t *ptr = (rt_uint16_t*)pixels;
	uint16_t* p = &usb_lcd_mem_buffer[x+y*WIDTH];

	while (size--)
	{
		 *p++ = *ptr++;
	}
}


int rt_usb_lcd_init(void)
{
	//_lcd_low_level_init();
	memset(usb_lcd_mem_buffer, 0xff, sizeof(usb_lcd_mem_buffer));

	static struct rt_device lcd_device;

	static struct rt_device_graphic_ops usb_lcd_ops =
	{
		usb_lcd_set_pixel,
		usb_lcd_get_pixel,
		usb_lcd_draw_hline,
		usb_lcd_draw_vline,
		usb_lcd_blit_line
	};

	/* register lcd device */
	lcd_device.type = RT_Device_Class_Graphic;
	lcd_device.init = lcd_init;
	lcd_device.open = lcd_open;
	lcd_device.close = lcd_close;
	lcd_device.control = lcd_control;
	lcd_device.read = RT_NULL;
	lcd_device.write = RT_NULL;

	lcd_device.user_data = &usb_lcd_ops;

	/* register graphic device driver */
	rt_device_register(&lcd_device, "lcd",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

	return 0;
}

void mouse_action(int x, int y, int btn, int is_down)
{
    struct rtgui_event_mouse emouse ;
    emouse.parent.type = RTGUI_EVENT_MOUSE_BUTTON;
    emouse.parent.sender = RT_NULL;

    emouse.id = 1;
    emouse.x = x ;
    emouse.y = y ;
    emouse.button = is_down?RTGUI_MOUSE_BUTTON_DOWN:RTGUI_MOUSE_BUTTON_UP;
    emouse.button |= (btn & 7);
    /* init mouse button */
    rtgui_server_post_event(&emouse.parent, sizeof(struct rtgui_event_mouse));
}

void key_action(int btn, int mod, int is_press)
{
    struct rtgui_event_kbd key ;
    key.parent.type = RTGUI_EVENT_KBD;
    key.parent.sender = RT_NULL;
    key.key = btn;
    key.type = is_press ? RTGUI_KEYDOWN : RTGUI_KEYUP;
    key.mod = mod;
    rtgui_server_post_event(&key.parent, sizeof(struct rtgui_event_kbd));
}

INIT_BOARD_EXPORT(rt_usb_lcd_init);

