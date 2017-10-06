/*
 * File      : calculator.h
 * COPYRIGHT (C) 2017, XToolbox.org
 *
 * A simple LCD simulate calculator based on rt gui
 *
 *
 */
#ifndef  _CALCULATOR_H_
#define  _CALCULATOR_H_


#define DOT 0x10
#define ADD 0x11
#define SUB 0x12
#define MUL 0x13
#define DIV 0x14
#define SQR 0x15
#define EQU 0x16
#define CLR 0x17
#define PERCENT 0x18
#define NEG 0x19
#define M_ADD 0x21
#define M_SUB 0x22
#define M_CLR 0x27
#define M_VAL 0x28
#define DUMMY  0xff
#define OPER_MASK 0x3f
#define EQU_FLAG  0x40
typedef double calc_value_t;
#define  MAX_DIGI_CNT   8
#define  CALC_MAX       99999999
#define  CALC_MIN       0.0000001

int calc_application_init(void);
void calc_on_button(struct rtgui_object *object, struct rtgui_event *event);


#endif
