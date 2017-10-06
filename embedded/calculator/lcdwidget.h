/*
 * File      : lcdwidget.h
 * COPYRIGHT (C) 2017, XToolbox.org
 *
 * A simple LCD simulator widget with 7 segment display
 *
 * segment defined as below:
 *    _a_
 *  f|   |b
 *    -g-
 *  e|   |c
 *    -d-
 *        .dot
 *
 */
#ifndef __LCD_WIDGET_H__
#define __LCD_WIDGET_H__

#include <rtgui/rtgui.h>
#include <rtgui/widgets/widget.h>


DECLARE_CLASS_TYPE(lcdwidget);
#define RTGUI_LCDWIDGET_TYPE       (RTGUI_TYPE(lcdwidget))
#define RTGUI_LCDWIDGET(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_LCDWIDGET_TYPE, rtgui_lcdwidget_t))
#define RTGUI_IS_LCDWIDGET(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_LCDWIDGET_TYPE))

#define  SEG_a    (1<<0)
#define  SEG_b    (1<<1)
#define  SEG_c    (1<<2)
#define  SEG_d    (1<<3)
#define  SEG_e    (1<<4)
#define  SEG_f    (1<<5)
#define  SEG_g    (1<<6)
#define  SEG_DOT  (1<<7)
#define  SEG_0    (0)

#define MAKE_SEG(a,b,c,d,e,f,g)  (SEG_##a | SEG_##b| SEG_##c| SEG_##d| SEG_##e| SEG_##f| SEG_##g)
#define  NUM_0  MAKE_SEG(a,b,c,d,e,f,0)
#define  NUM_1  MAKE_SEG(b,c,0,0,0,0,0)
#define  NUM_2  MAKE_SEG(a,b,g,e,d,0,0)
#define  NUM_3  MAKE_SEG(a,b,g,c,d,0,0)
#define  NUM_4  MAKE_SEG(f,g,b,c,0,0,0)
#define  NUM_5  MAKE_SEG(a,f,g,c,d,0,0)
#define  NUM_6  MAKE_SEG(a,f,g,c,d,e,0)
#define  NUM_7  MAKE_SEG(a,b,c,0,0,0,0)
#define  NUM_8  MAKE_SEG(a,b,c,d,e,f,g)
#define  NUM_9  MAKE_SEG(a,b,c,d,f,g,0)

#define  NUM_A  MAKE_SEG(a,f,g,b,e,c,0)
#define  NUM_B  MAKE_SEG(f,g,e,d,c,0,0)
#define  NUM_C  MAKE_SEG(a,f,e,d,0,0,0)
#define  NUM_D  MAKE_SEG(b,g,e,d,c,0,0)
#define  NUM_E  MAKE_SEG(a,f,g,e,d,0,0)
#define  NUM_F  MAKE_SEG(a,f,g,e,0,0,0)
#define  NUM_G  MAKE_SEG(a,f,e,d,c,0,0)
#define  NUM_H  MAKE_SEG(f,g,b,e,c,0,0)
#define  NUM_I  MAKE_SEG(e,0,0,0,0,0,0)
#define  NUM_J  MAKE_SEG(b,c,d,0,0,0,0)
#define  NUM_K  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_L  MAKE_SEG(f,e,d,0,0,0,0)
#define  NUM_M  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_N  MAKE_SEG(a,f,e,b,c,0,0)
#define  NUM_O  MAKE_SEG(e,g,d,c,0,0,0)
#define  NUM_P  MAKE_SEG(a,f,g,b,e,0,0)
#define  NUM_Q  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_R  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_S  NUM_5
#define  NUM_T  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_U  MAKE_SEG(f,e,d,b,c,0,0)
#define  NUM_V  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_W  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_X  MAKE_SEG(a,g,d,0,0,0,0)  // not support
#define  NUM_Y  MAKE_SEG(f,g,b,c,d,0,0)
#define  NUM_Z  NUM_2

#define  NUM_MINUS MAKE_SEG(g,0,0,0,0,0,0)
#define  SEG_MEM   (1<<0)
#define  SEG_ERROR (1<<1)
#define  SEG_MINUS (1<<2)
struct rtgui_lcdwidget
{
    struct rtgui_widget parent;

    rt_uint8_t seg_status[8]; // 8 digits
    rt_uint8_t flag;          // the memory, error, minus sign
};
typedef struct rtgui_lcdwidget rtgui_lcdwidget_t;

struct rtgui_lcdwidget *rtgui_lcdwidget_create(rtgui_rect_t *r);
void rtgui_lcdwidget_destroy(struct rtgui_lcdwidget *me);

rt_bool_t rtgui_lcdwidget_event_handler(struct rtgui_object *object, struct rtgui_event *event);

void rtgui_lcdwidget_set_number(struct rtgui_lcdwidget *me, const char* num, int neg, int memory, int error);

#endif
