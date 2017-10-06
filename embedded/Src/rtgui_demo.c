/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>

#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/dc_hw.h>
#include <rtgui/widgets/widget.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <string.h>

#include "finsh.h"
#include "rtgui_demo.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(...)   rt_kprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)   
#endif

#ifdef RT_USING_GUIENGINE

struct rtgui_win *main_win;
static struct rtgui_label *label = RT_NULL;
rt_bool_t dc_event_handler(struct rtgui_object *object, rtgui_event_t *event);


static void onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
        if (label != RT_NULL)
        {
        struct rtgui_button *button;

        button = RTGUI_BUTTON(object);

        if (strcmp(rtgui_label_get_text(label), "Hello World") == 0)
            rtgui_label_set_text(label, "Hello from button");
        else
            rtgui_label_set_text(label, "Hello World");

        if (strcmp(rtgui_label_get_text(RTGUI_LABEL(button)), "OK") == 0)
            rtgui_label_set_text(RTGUI_LABEL(button), "Hello");
        else
            rtgui_label_set_text(RTGUI_LABEL(button), "OK");

                rtgui_widget_update(RTGUI_WIDGET(label));
        rtgui_widget_update(RTGUI_WIDGET(button));
        }

        return;
}


static void rt_gui_demo_entry(void *parameter)
{    
    struct rtgui_app *app;
    //struct rtgui_dc *dc;
    struct rtgui_button *button;
    struct rtgui_box *box;

    rt_device_t device;

    device = rt_device_find("lcd");
    /* re-set graphic device */
    rtgui_graphic_set_device(device);



    DEBUG_PRINTF("gui demo entry\n");

    /* create gui app */
    app = rtgui_app_create("gui_demo");
    if (app == RT_NULL)
    {
        DEBUG_PRINTF("rtgui_app_create faild\n");
        return;
    }

    /* create main window */
    main_win = rtgui_mainwin_create(RT_NULL,
                                    "UiWindow", RTGUI_WIN_STYLE_NO_TITLE | RTGUI_WIN_STYLE_NO_BORDER);
    if (main_win == RT_NULL)
    {
        DEBUG_PRINTF("main_win is null\n");
        rtgui_app_destroy(app);
        return;
    }


    /* we use layout engine to place sub-widgets */
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(main_win), box);

    /* create the 'hello world' label */
        label = rtgui_label_create("Hello World");
        rtgui_widget_set_minwidth(RTGUI_WIDGET(label), 150);
        rtgui_container_add_child(RTGUI_CONTAINER(main_win), RTGUI_WIDGET(label));

        /* create the button */
        button = rtgui_button_create("OK");
        rtgui_button_set_onbutton(button, onbutton);
        rtgui_widget_set_minwidth(RTGUI_WIDGET(button), 80);
        rtgui_widget_set_minheight(RTGUI_WIDGET(button), 25);
        rtgui_container_add_child(RTGUI_CONTAINER(main_win), RTGUI_WIDGET(button));

    rtgui_container_layout(RTGUI_CONTAINER(main_win));



    //rtgui_object_set_event_handler(RTGUI_OBJECT(main_win), dc_event_handler);
	
    DEBUG_PRINTF("rtgui_win_show\n");
    rtgui_win_show(main_win, RT_FALSE);
    
    DEBUG_PRINTF("rtgui_app_run\n");
    rtgui_app_run(app);
    
    DEBUG_PRINTF("rtgui_win_destroy\n");
    rtgui_win_destroy(main_win);
    
    DEBUG_PRINTF("rtgui_app_destroy\n");
    rtgui_app_destroy(app);
}

rt_bool_t dc_event_handler_not_used_now(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);

    if (event->type == RTGUI_EVENT_PAINT)
    {
        struct rtgui_dc *dc;
        rtgui_rect_t rect;
        rt_kprintf("\r\n RTGUI_EVENT_PAINT \r\n");
        rtgui_win_event_handler(RTGUI_OBJECT(widget), event);


        rtgui_widget_get_rect(widget, &rect);
        DEBUG_PRINTF("widget react x1: %d, y1: %d, x2: %d, y2: %d\r\n",
                     rect.x1, rect.y1, rect.x2, rect.y2);

        dc = rtgui_dc_begin_drawing(widget);
        if(dc == RT_NULL)
        {
            DEBUG_PRINTF("\r\n dc is null \r\n");
            return RT_FALSE;
        }

        rtgui_dc_draw_line(dc,0,0,320,240);
        rtgui_dc_draw_line(dc,0,240,320,0);
        //rtgui_dc_draw_text(dc, __DATE__, &rect);
        rtgui_dc_draw_text_stroke(dc, __DATE__, &rect, HIGH_LIGHT, BLUE);

        rect.y1 += 20;
        rect.y2 += 20;
        rtgui_dc_draw_text_stroke(dc, __TIME__, &rect, HIGH_LIGHT, BLACK);
        rtgui_dc_end_drawing(dc);
    }
    return RT_FALSE;
}
void application_init(void);
void calc_application_init(void);
int rt_gui_demo_init(void)
{
#if 0
    rt_thread_t tid;
    tid = rt_thread_create("mygui",
        rt_gui_demo_entry, RT_NULL,
        2048, 25, 10);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
#else
    //application_init();
    calc_application_init();
#endif
    return 0;
}


//INIT_APP_EXPORT(rt_gui_demo_init);
#endif /* RT_USING_GUIENGINE */
