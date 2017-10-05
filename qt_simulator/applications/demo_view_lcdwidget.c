/*
 * 程序清单：自定义控件演示
 *
 * 这个例子会在创建出的container上添加两个自定义控件
 */
#include "../ui_examples/demo_view.h"
#include "lcdwidget.h"

/* 创建用于演示自定义控件的视图 */
rtgui_container_t *demo_view_lcdwidget(void)
{
    rtgui_container_t *container;
    rtgui_rect_t rect;
    rtgui_lcdwidget_t *lcdwidget;

    /* 先创建一个演示用的视图 */
    container = demo_view("LCDWidget View");

    /* 获得视图的位置信息 */
    demo_view_get_rect(container, &rect);
    rect.x1 += 10;
    rect.x2 = rect.y1 + 80;
    rect.y1 += 10;
    rect.y2 = rect.y1 + 80;
    /* 创建一个自定义控件 */
    lcdwidget = rtgui_lcdwidget_create(&rect);
    /* container是一个container控件，调用add_child方法添加这个自控件 */
    rtgui_container_add_child(container, RTGUI_WIDGET(lcdwidget));
#if 0
    /* 获得视图的位置信息 */
    demo_view_get_rect(container, &rect);
    rect.x1 += 25;
    rect.x2 = rect.y1 + 40;
    rect.y1 += 5 + 100;
    rect.y2 = rect.y1 + 40;
    /* 创建一个自定义控件 */
    mywidget = rtgui_mywidget_create(&rect);
    /* container是一个container控件，调用add_child方法添加这个自控件 */
    rtgui_container_add_child(container, RTGUI_WIDGET(mywidget));
#endif
    return container;
}
