#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/button.h>

#include <rtgui/widgets/window.h>
#include <rtgui/widgets/notebook.h>
#include "lcdwidget.h"
#include "image_mem.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "calculator.h"
#include "finsh.h"

struct rtgui_win *main_win;
struct rtgui_lcdwidget* lcdwidget;

typedef struct calc_state{
    calc_value_t mem_val; // memory value, is memory value less than the minium value,
    calc_value_t pre_val; // previous result value
    calc_value_t cur_val; // current value, maybe threat as current input or calc result
    rt_uint8_t is_error;         // is error occur
    rt_uint8_t input_count;      // input digital count
    rt_uint8_t input_divider;    // input digital divider with
    rt_uint8_t last_oper;
}calc_state_t;

static calc_state_t  state;
// translate key event to calculator event
static int translate_key_event(rt_uint16_t key)
{
    switch(key){
    case RTGUIK_0:  case RTGUIK_1:   case RTGUIK_2:   case RTGUIK_3:   case RTGUIK_4:
    case RTGUIK_5:  case RTGUIK_6:  case RTGUIK_7:   case RTGUIK_8:
    case RTGUIK_9:           return key - RTGUIK_0;
    case RTGUIK_KP0:  case RTGUIK_KP1:  case RTGUIK_KP2:  case RTGUIK_KP3:  case RTGUIK_KP4:
    case RTGUIK_KP5:  case RTGUIK_KP6:  case RTGUIK_KP7:  case RTGUIK_KP8:
    case RTGUIK_KP9:         return key - RTGUIK_0;
    case RTGUIK_KP_PLUS:
    case RTGUIK_PLUS:        return ADD;
    case RTGUIK_KP_MINUS:
    case RTGUIK_MINUS:       return SUB;
    case RTGUIK_ASTERISK:
    case RTGUIK_KP_MULTIPLY: return MUL;
    case RTGUIK_SLASH:
    case RTGUIK_KP_DIVIDE:   return DIV;
    case RTGUIK_PERIOD:
    case RTGUIK_KP_PERIOD:   return DOT;
    case RTGUIK_DELETE:
    case RTGUIK_BACKSPACE:
    case RTGUIK_ESCAPE:      return CLR;
    case RTGUIK_KP_EQUALS:
    case RTGUIK_EQUALS:
    case RTGUIK_KP_ENTER:    return EQU;
    case RTGUIK_s:           return SQR;
    case RTGUIK_n:           return NEG;
    case RTGUIK_p:           return PERCENT;
    case RTGUIK_a:           return M_ADD;
    case RTGUIK_m:           return M_SUB;
    case RTGUIK_c:           return M_CLR;
    case RTGUIK_v:           return M_VAL;
    }
    return DUMMY;
}

// check the result, and set the error flag of the calculator
static void check_val(calc_state_t* s)
{
    int c = fpclassify(s->cur_val);
    if(c == FP_NORMAL || c == FP_ZERO || c == FP_SUBNORMAL)
    {
        if(fabs(s->cur_val) < CALC_MIN){
            s->cur_val = 0;
        }
        if(fabs(s->cur_val)>CALC_MAX){
            s->is_error = 1;
        }
    }else{
        s->is_error = 1;
    }
}

// get previous result with last operation
static calc_value_t get_pre_result(calc_state_t* s)
{
    switch(s->last_oper & OPER_MASK){
    case ADD:
        return s->pre_val + s->cur_val;
    case SUB:
        return s->pre_val - s->cur_val;
    case MUL:
        return s->pre_val * s->cur_val;
    case DIV:
        return s->pre_val / s->cur_val;
    }
    return s->cur_val;
}

// get immediately result
static calc_value_t immediately_result(calc_state_t* s, int val)
{
    switch(val){
    case SQR: return sqrt(s->cur_val);
    case NEG: return -s->cur_val;
    case PERCENT: return s->cur_val/100.0;
    }
    return s->cur_val;
}

static int calc_process_input(calc_state_t* s, int val)
{
    if(s->is_error && val != CLR)return 0;
    switch(val){
    case 0: case 1: case 2: case 3: case 4:
    case 5: case 6: case 7: case 8: case 9:
        if(s->input_count>= MAX_DIGI_CNT){
            return 0;
        }
        if(!s->input_count){
            s->cur_val = 0;
            if(s->input_divider != 10){
                s->input_divider = 0;
            }
            s->input_count = 1;
        }else{
            s->input_count++;
        }
        if(s->input_divider){
            calc_value_t t = val;
            s->cur_val = s->cur_val + t/s->input_divider;
            s->input_divider = s->input_divider*10;
        }else{
            s->cur_val = s->cur_val*10 + val;
        }
        return 1;
    case DOT:
        if(!s->input_divider){
            s->input_divider = 10;
        }
        if(!s->input_count){
            s->input_count = 1;
        }
        return 0;
    case CLR:
        s->input_divider = 0;
        s->cur_val = 0;
        s->pre_val = 0;
        s->is_error = 0;
        s->input_count = 0;
        s->last_oper = DUMMY;
        return 1;
    case ADD: case SUB: case MUL: case DIV:
        if(s->last_oper & EQU_FLAG){
            s->pre_val = s->cur_val;
        }else{
            s->cur_val = s->pre_val = get_pre_result(s);
        }
        check_val(s);
        s->input_count = 0;
        s->last_oper = val;
        return 1;
    case EQU:
        s->cur_val = get_pre_result(s);
        check_val(s);
        s->input_count = 0;
        s->last_oper |= EQU_FLAG;
        return 1;
    case PERCENT:
    case SQR:
    case NEG:
        s->cur_val = immediately_result(s, val);
        check_val(s);
        s->input_count = 0;
        s->last_oper = DUMMY;
        return 1;
    case M_ADD:
        s->mem_val += s->cur_val;
        return 1;
    case M_SUB:
        s->mem_val -= s->cur_val;
        return 1;
    case M_CLR:
        s->mem_val = 0;
        return 1;
    case M_VAL:
        s->cur_val = s->mem_val;
        check_val(s);
        s->input_count = 0;
        s->last_oper = DUMMY;
        return 1;
    }
    return 0;
}

static int calc_process_display(calc_state_t* s)
{
    char res[32] = "";
    int pt_pos = 0;
    char* ps = res;
    int memory = 0;
    int neg = 0;
    int i;
    sprintf(res, "%.9f", s->cur_val);
    if(*ps == '-') {
        neg = 1;
        ps++;
    }
    for(i=0;i<32;i++){
        if(ps[i] == '.'){
            pt_pos = i;
        }
    }
    ps[MAX_DIGI_CNT+1] = 0;
    for(i=MAX_DIGI_CNT;i>pt_pos;--i){
        if(ps[i] == '0' && (!s->input_count || i > s->input_count)) ps[i] = 0;
        else break;
    }
    if(fabs(s->mem_val)> CALC_MIN){
        memory = 1;
    }
    // display number on the LCD widget
    rtgui_lcdwidget_set_number(lcdwidget, ps, neg, memory, s->is_error);
    rtgui_widget_update(RTGUI_WIDGET(main_win));
    //rt_kprintf("%s  %s %s\n", res, s->is_error?"error":"succ", memory?"M": "");
		return 0;
}

static rt_bool_t calc_handle_key(struct rtgui_object *object, struct rtgui_event *event)
{
    struct rtgui_event_kbd *ekbd = (struct rtgui_event_kbd *)event;

    if (ekbd->type == RTGUI_KEYUP)
    {
        int val = translate_key_event(ekbd->key);
        if(calc_process_input(&state, val)){
            calc_process_display(&state);
        }
    }
    return RT_TRUE;
}

void calc_on_button(struct rtgui_object *object, struct rtgui_event *event)
{
    int val = (int)RTGUI_WIDGET(object)->user_data;
    if(calc_process_input(&state, val)){
        calc_process_display(&state);
    }
}
void add_buttons(rtgui_container_t* container);
static void application_entry(void *parameter)
{
    struct rtgui_app *app;
    struct rtgui_rect rect;
    rt_device_t device;

    device = rt_device_find("lcd");
    /* re-set graphic device */
    rtgui_graphic_set_device(device);


    app = rtgui_app_create("calc_app");
    if (app == RT_NULL)
        return;

    /* create a full screen window */
    rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);

    main_win = rtgui_win_create(RT_NULL, "calc_win", &rect,
                                RTGUI_WIN_STYLE_NO_BORDER | RTGUI_WIN_STYLE_NO_TITLE);
    if (main_win == RT_NULL)
    {
        rtgui_app_destroy(app);
        return;
    }
    RTGUI_WIDGET_BACKGROUND(main_win) = RTGUI_RGB(0xdc,0xdc,0xdc);
    rtgui_win_set_onkey(main_win, calc_handle_key);

    rect.x1 += 10;
    rect.x2 = rect.y1 + 10;
    rect.y1 += 10;
    rect.y2 = rect.y1 + 10;
    // put the lcd widget at (10,10), ignore the x2,y2, they will be set to the real value
    lcdwidget = rtgui_lcdwidget_create(&rect);
    if (lcdwidget == RT_NULL)
    {
        rtgui_win_destroy(main_win);
        rtgui_app_destroy(app);
        return;
    }
    rtgui_container_add_child(RTGUI_CONTAINER(main_win), RTGUI_WIDGET(lcdwidget));

    // put buttons
    add_buttons(RTGUI_CONTAINER(main_win));

    rtgui_win_show(main_win, RT_FALSE);

    rtgui_app_run(app);

    rtgui_app_destroy(app);
}

int calc_application_init(void)
{
    static rt_bool_t inited = RT_FALSE;

    if (inited == RT_FALSE)
    {
        rt_thread_t tid;

        tid = rt_thread_create("calc",
                               application_entry, RT_NULL,
                               2048 * 2, 25, 10);

        if (tid != RT_NULL)
            rt_thread_startup(tid);

        inited = RT_TRUE;
    }
		return 0;
}
INIT_APP_EXPORT(calc_application_init);


long calc(int v)
{
	v = calc_process_input(&state, v);
	if(v){
		calc_process_display(&state);
	}
	return v;
}

FINSH_FUNCTION_EXPORT(calc, manul input calc event);
