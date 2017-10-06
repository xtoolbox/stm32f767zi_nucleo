#include <rtgui/dc.h>
//#include <rtgui/image.h>
#include "lcdwidget.h"
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include "string.h"

//static const rt_uint8_t back_image_data[] = {
//    #include "H:/work/stm32f767zi_nucleo/sim_calc/back_data.c"
//};
#define LCD_WIDTH  300
#define LCD_HEIGHT 80
#define LCD_LINE_DATA_SIZE (LCD_WIDTH*2)
static const rt_uint8_t back_raw_565[] = {
    #include "lcd_back_raw_565_data.c"
};
const int seg_a [] = { /* size */  4,
   34,    12,    55,    12,    51,    18,    41,    18, };
const int seg_b [] = { /* size */  6,
   56,    13,    58,    14,    58,    34,    56,    36,    52,    33,    52,    20, };
const int seg_c [] = { /* size */  6,
   56,    38,    58,    41,    58,    59,    56,    61,    52,    55,    52,    41, };
const int seg_d [] = { /* size */  5,
   41,    56,    51,    56,    55,    62,    34,    62,    33,    60, };
const int seg_e [] = { /* size */  5,
   33,    41,    35,    38,    39,    41,    39,    55,    33,    58, };
const int seg_f [] = { /* size */  5,
   33,    13,    39,    20,    39,    33,    35,    36,    33,    35, };
const int seg_g [] = { /* size */  6,
   36,    37,    41,    34,    51,    34,    55,    37,    51,    40,    41,    40, };
const int seg_dot [] = { /* size */  4,
   59,    65,    64,    65,    64,    71,    59,    71, };
const int seg_memory [] = { /* size */  12,
    5,    12,    10,    12,    15,    20,    20,    12,    25,    12,    25,    31,    20,    31,    20,    17,    15,    25,    10,    17,    10,    31,     5,    31, };
const int seg_error [] = { /* size */  12,
    5,    43,    25,    43,    25,    47,     9,    47,     9,    50,    25,    50,    25,    54,     9,    54,     9,    58,    25,    58,    25,    62,     5,    62, };
const int seg_minus [] = { /* size */  4,
    5,    36,    25,    36,    25,    39,     5,    39, };
#define SEG_MAX_SIZE  12
#define DISTANCE_X  33
typedef const int* const_int_p;
const const_int_p segs[] = {
    seg_a,seg_b,seg_c,seg_d,seg_e,seg_f,seg_g,seg_dot
};

static void draw_seg(struct rtgui_dc *dc, const int* data, int pos)
{
    int vx[SEG_MAX_SIZE];
    int vy[SEG_MAX_SIZE];
    int i;
    int size;
    if(pos<0)return;
    if(pos>8)return;
    size = *data++;
    for(i=0;i<size;++i){
        vx[i] = data[0] + pos*DISTANCE_X;
        vy[i] = data[1];
        data+=2;
    }
    rtgui_dc_fill_polygon(dc, vx, vy, size);
}

static void rtgui_lcdwidget_ondraw(struct rtgui_lcdwidget *me)
{
    struct rtgui_dc *dc;
    //struct rtgui_rect rect;
    const rt_uint8_t* line_data = back_raw_565;
    int y;

    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(me));
    if (dc == RT_NULL) return;

    //rtgui_widget_get_rect(RTGUI_WIDGET(me), &rect);
    //RTGUI_DC_BC(dc) = white;
    //rtgui_dc_fill_rect(dc, &rect);

    for(y=0;y<LCD_HEIGHT;++y){
        dc->engine->blit_line(dc, 0, LCD_WIDTH, y,  (rt_uint8_t*)line_data);
        line_data += LCD_LINE_DATA_SIZE;
    }

    RTGUI_DC_FC(dc) = black;
    if(me->flag & SEG_MEM){
        draw_seg(dc, seg_memory, 0);
    }
    if(me->flag & SEG_ERROR){
        draw_seg(dc, seg_error, 0);
    }
    if(me->flag & SEG_MINUS){
        draw_seg(dc, seg_minus, 0);
    }
    for(y=0;y<sizeof(me->seg_status);++y){
        int i=0;
        rt_uint8_t f = 1;
        while(i<8){
            if(me->seg_status[y] & f){
                draw_seg(dc, segs[i], y);
            }
            ++i;f<<=1;
        }
    }
    rtgui_dc_end_drawing(dc);
    return;
}
const rt_uint8_t num_pat[] = {
    NUM_0,NUM_1,NUM_2,NUM_3,NUM_4,NUM_5,NUM_6,NUM_7,NUM_8,NUM_9,
};
const rt_uint8_t letter_pat[] = {
    NUM_A,NUM_B,NUM_C,NUM_D,NUM_E,NUM_F,NUM_G,
    NUM_H,NUM_I,NUM_J,NUM_K,NUM_L,NUM_M,NUM_N,
    NUM_O,NUM_P,NUM_Q,      NUM_R,NUM_S,NUM_T,
    NUM_U,NUM_V,NUM_W,      NUM_X,NUM_Y,NUM_Z,
};
void rtgui_lcdwidget_set_number(struct rtgui_lcdwidget *me, const char* num, int neg, int memory, int error)
{
    int i = 0;
    const char* end = num;
    while(*end){i++; end++; if(i>10)break; }
    memset(me->seg_status,0,sizeof(me->seg_status));
    me->flag = 0;
    if(i>10){
        me->seg_status[0] = NUM_E;
        me->seg_status[1] = NUM_MINUS;
        me->seg_status[2] = NUM_L;
        me->seg_status[3] = NUM_E;
        me->seg_status[4] = NUM_N;
    }else{
        end--; num--;
        i = sizeof(me->seg_status) - 1;
        if(error){
            end--;
        }
        while(end != num){
            if(*end == '.'){
                me->seg_status[i] |= SEG_DOT;
            }
            else if(*end>='0' && *end<='9'){
                me->seg_status[i] |= num_pat[*end - '0'];
                i--;
            }else{
                me->seg_status[i] |= NUM_E;
                i--;
                error |= 1;
                break;
            }
            end--;
        }
        if(neg){
            if(i<0) me->flag |= SEG_MINUS;
            else me->seg_status[i] = NUM_MINUS;
        }
        if(error){
            me->flag |= SEG_ERROR;
            if(i<0)i=0;
            me->seg_status[i] |= SEG_DOT;
        }
        if(memory) me->flag |= SEG_MEM;
    }
    //rtgui_server_post_event();
    //rtgui_lcdwidget_ondraw(me);
    //rtgui_widget_update(&me->parent);
}

static void rtgui_lcdwidget_onmouse(struct rtgui_lcdwidget *me, struct rtgui_event_mouse *mouse)
{
    //struct rtgui_rect rect;
    //rt_uint16_t x, y;

    if (!(mouse->button & RTGUI_MOUSE_BUTTON_UP)) return;

    //rtgui_widget_get_rect(RTGUI_WIDGET(me), &rect);
    //rtgui_widget_rect_to_device(RTGUI_WIDGET(me), &rect);

    //x = (rect.x2 + rect.x1) / 2;
    //y = (rect.y2 + rect.y1) / 2;
}

rt_bool_t rtgui_lcdwidget_event_handler(struct rtgui_object *object, struct rtgui_event *event)
{
    struct rtgui_lcdwidget *me = RTGUI_LCDWIDGET(object);

    switch (event->type)
    {
    case RTGUI_EVENT_PAINT:
        rtgui_lcdwidget_ondraw(me);
        break;

    case RTGUI_EVENT_MOUSE_BUTTON:
        rtgui_lcdwidget_onmouse(RTGUI_LCDWIDGET(me), (struct rtgui_event_mouse *) event);
        break;

    default:
        return rtgui_widget_event_handler(object, event);
    }

    return RT_FALSE;
}

static void _rtgui_lcdwidget_constructor(rtgui_lcdwidget_t *lcdwidget)
{
    RTGUI_WIDGET(lcdwidget)->flag |= RTGUI_WIDGET_FLAG_FOCUSABLE;
    rtgui_object_set_event_handler(RTGUI_OBJECT(lcdwidget), rtgui_lcdwidget_event_handler);

    lcdwidget->flag = 0;
    memset(lcdwidget->seg_status, 0, sizeof(lcdwidget->seg_status));
    lcdwidget->seg_status[0] = NUM_H;
    lcdwidget->seg_status[1] = NUM_E;
    lcdwidget->seg_status[2] = NUM_L;
    lcdwidget->seg_status[3] = NUM_L;
    lcdwidget->seg_status[4] = NUM_O;
    lcdwidget->seg_status[5] = 0;
    lcdwidget->seg_status[6] = NUM_G;
    lcdwidget->seg_status[7] = NUM_G;
}

DEFINE_CLASS_TYPE(lcdwidget, "lcdwidget",
                  RTGUI_PARENT_TYPE(widget), 
                  _rtgui_lcdwidget_constructor,
                  RT_NULL,
                  sizeof(struct rtgui_lcdwidget));

struct rtgui_lcdwidget *rtgui_lcdwidget_create(rtgui_rect_t *r)
{
    struct rtgui_lcdwidget *me;

    //back_image = rtgui_image_create_from_mem("bmp", back_image_data, sizeof(back_image_data), 1);
    //if(back_image){
    //}
    r->x2 = LCD_WIDTH + r->x1;
    r->y2 = LCD_HEIGHT + r->y1;

    me = (struct rtgui_lcdwidget *) rtgui_widget_create(RTGUI_LCDWIDGET_TYPE);
    if (me != RT_NULL)
    {
        rtgui_widget_set_rect(RTGUI_WIDGET(me), r);
    }

    return me;
}

void rtgui_lcdwidget_destroy(struct rtgui_lcdwidget *me)
{
    rtgui_widget_destroy(RTGUI_WIDGET(me));
}
