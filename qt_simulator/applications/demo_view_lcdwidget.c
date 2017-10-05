/*
 * �����嵥���Զ���ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����������Զ���ؼ�
 */
#include "../ui_examples/demo_view.h"
#include "lcdwidget.h"

/* ����������ʾ�Զ���ؼ�����ͼ */
rtgui_container_t *demo_view_lcdwidget(void)
{
    rtgui_container_t *container;
    rtgui_rect_t rect;
    rtgui_lcdwidget_t *lcdwidget;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("LCDWidget View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 10;
    rect.x2 = rect.y1 + 80;
    rect.y1 += 10;
    rect.y2 = rect.y1 + 80;
    /* ����һ���Զ���ؼ� */
    lcdwidget = rtgui_lcdwidget_create(&rect);
    /* container��һ��container�ؼ�������add_child�����������Կؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(lcdwidget));
#if 0
    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 25;
    rect.x2 = rect.y1 + 40;
    rect.y1 += 5 + 100;
    rect.y2 = rect.y1 + 40;
    /* ����һ���Զ���ؼ� */
    mywidget = rtgui_mywidget_create(&rect);
    /* container��һ��container�ؼ�������add_child�����������Կؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(mywidget));
#endif
    return container;
}
