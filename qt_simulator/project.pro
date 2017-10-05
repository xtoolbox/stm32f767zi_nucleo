# Qt project file create by rt-thread
QT += core
TARGET = rtthread
TEMPLATE = app
CONFIG   += console




# source files
SOURCES += applications\application.c
SOURCES += applications\demo_view_lcdwidget.c
SOURCES += applications\init.c
SOURCES += applications\lcd_application.c
SOURCES += applications\platform.c
SOURCES += applications\startup.c
SOURCES += applications\main.cpp
SOURCES += drivers\board.c
SOURCES += drivers\uart_console.c
SOURCES += ..\embedded\board_driver\drv_usb_lcd.c
SOURCES += ..\embedded\Src\rtgui_demo.c
SOURCES += ui_examples\demo_application.c
SOURCES += ui_examples\demo_view.c
SOURCES += ui_examples\demo_xml.c
SOURCES += ui_examples\demo_view_benchmark.c
SOURCES += ui_examples\demo_view_dc.c
SOURCES += ui_examples\demo_view_ttf.c
SOURCES += ui_examples\demo_view_dc_buffer.c
SOURCES += ui_examples\demo_view_animation.c
SOURCES += ui_examples\demo_view_buffer_animation.c
SOURCES += ui_examples\demo_view_instrument_panel.c
SOURCES += ui_examples\demo_view_window.c
SOURCES += ui_examples\demo_view_label.c
SOURCES += ui_examples\demo_view_button.c
SOURCES += ui_examples\demo_view_checkbox.c
SOURCES += ui_examples\demo_view_progressbar.c
SOURCES += ui_examples\demo_view_scrollbar.c
SOURCES += ui_examples\demo_view_radiobox.c
SOURCES += ui_examples\demo_view_textbox.c
SOURCES += ui_examples\demo_view_listbox.c
SOURCES += ui_examples\demo_view_menu.c
SOURCES += ui_examples\demo_view_listctrl.c
SOURCES += ui_examples\demo_view_combobox.c
SOURCES += ui_examples\demo_view_slider.c
SOURCES += ui_examples\demo_view_notebook.c
SOURCES += ui_examples\demo_view_mywidget.c
SOURCES += ui_examples\demo_view_box.c
SOURCES += ui_examples\demo_view_edit.c
SOURCES += ui_examples\demo_view_bmp.c
SOURCES += ui_examples\demo_plot.c
SOURCES += ui_examples\mywidget.c
SOURCES += ui_examples\demo_view_digtube.c
SOURCES += ..\embedded\calculator\lcdwidget.c
SOURCES += ..\embedded\calculator\image_mem.c
SOURCES += ..\embedded\calculator\calculator.c
SOURCES += ..\embedded\calculator\calc_button_resource.c
SOURCES += ..\..\rt-thread\src\clock.c
SOURCES += ..\..\rt-thread\src\device.c
SOURCES += ..\..\rt-thread\src\idle.c
SOURCES += ..\..\rt-thread\src\ipc.c
SOURCES += ..\..\rt-thread\src\irq.c
SOURCES += ..\..\rt-thread\src\kservice.c
SOURCES += ..\..\rt-thread\src\mem.c
SOURCES += ..\..\rt-thread\src\memheap.c
SOURCES += ..\..\rt-thread\src\mempool.c
SOURCES += ..\..\rt-thread\src\object.c
SOURCES += ..\..\rt-thread\src\scheduler.c
SOURCES += ..\..\rt-thread\src\thread.c
SOURCES += ..\..\rt-thread\src\timer.c
SOURCES += ..\..\rt-thread\libcpu\sim\win32\cpu_port.c
SOURCES += ..\..\rt-thread\components\drivers\serial\serial.c
SOURCES += ..\..\rt-thread\components\drivers\src\completion.c
SOURCES += ..\..\rt-thread\components\drivers\src\dataqueue.c
SOURCES += ..\..\rt-thread\components\drivers\src\pipe.c
SOURCES += ..\..\rt-thread\components\drivers\src\portal.c
SOURCES += ..\..\rt-thread\components\drivers\src\ringbuffer.c
SOURCES += ..\..\rt-thread\components\drivers\src\workqueue.c
SOURCES += ..\..\rt-thread\components\finsh\shell.c
SOURCES += ..\..\rt-thread\components\finsh\symbol.c
SOURCES += ..\..\rt-thread\components\finsh\cmd.c
SOURCES += ..\..\rt-thread\components\finsh\msh.c
SOURCES += ..\..\rt-thread\components\finsh\msh_cmd.c
SOURCES += ..\..\rt-thread\components\finsh\msh_file.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_compiler.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_error.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_heap.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_init.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_node.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_ops.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_parser.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_var.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_vm.c
SOURCES += ..\..\rt-thread\components\finsh\finsh_token.c
SOURCES += ..\..\rt-thread\components\gui\src\asc12font.c
SOURCES += ..\..\rt-thread\components\gui\src\asc16font.c
SOURCES += ..\..\rt-thread\components\gui\src\blit.c
SOURCES += ..\..\rt-thread\components\gui\src\box.c
SOURCES += ..\..\rt-thread\components\gui\src\color.c
SOURCES += ..\..\rt-thread\components\gui\src\container.c
SOURCES += ..\..\rt-thread\components\gui\src\dc.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_blend.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_buffer.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_client.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_hw.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_rotozoom.c
SOURCES += ..\..\rt-thread\components\gui\src\dc_trans.c
SOURCES += ..\..\rt-thread\components\gui\src\filerw.c
SOURCES += ..\..\rt-thread\components\gui\src\font.c
SOURCES += ..\..\rt-thread\components\gui\src\font_bmp.c
SOURCES += ..\..\rt-thread\components\gui\src\font_fnt.c
SOURCES += ..\..\rt-thread\components\gui\src\font_freetype.c
SOURCES += ..\..\rt-thread\components\gui\src\font_hz_bmp.c
SOURCES += ..\..\rt-thread\components\gui\src\font_hz_file.c
SOURCES += ..\..\rt-thread\components\gui\src\hz12font.c
SOURCES += ..\..\rt-thread\components\gui\src\hz16font.c
SOURCES += ..\..\rt-thread\components\gui\src\image.c
SOURCES += ..\..\rt-thread\components\gui\src\image_bmp.c
SOURCES += ..\..\rt-thread\components\gui\src\image_hdc.c
SOURCES += ..\..\rt-thread\components\gui\src\image_jpg.c
SOURCES += ..\..\rt-thread\components\gui\src\image_png.c
SOURCES += ..\..\rt-thread\components\gui\src\image_xpm.c
SOURCES += ..\..\rt-thread\components\gui\src\matrix.c
SOURCES += ..\..\rt-thread\components\gui\src\mouse.c
SOURCES += ..\..\rt-thread\components\gui\src\region.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_app.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_driver.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_object.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_system.c
SOURCES += ..\..\rt-thread\components\gui\src\server.c
SOURCES += ..\..\rt-thread\components\gui\src\title.c
SOURCES += ..\..\rt-thread\components\gui\src\topwin.c
SOURCES += ..\..\rt-thread\components\gui\src\widget.c
SOURCES += ..\..\rt-thread\components\gui\src\window.c
SOURCES += ..\..\rt-thread\components\gui\widgets\button.c
SOURCES += ..\..\rt-thread\components\gui\widgets\caret.c
SOURCES += ..\..\rt-thread\components\gui\widgets\checkbox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\combobox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\digfont.c
SOURCES += ..\..\rt-thread\components\gui\widgets\digtube.c
SOURCES += ..\..\rt-thread\components\gui\widgets\edit.c
SOURCES += ..\..\rt-thread\components\gui\widgets\filelist_view.c
SOURCES += ..\..\rt-thread\components\gui\widgets\groupbox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\iconbox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\label.c
SOURCES += ..\..\rt-thread\components\gui\widgets\list_view.c
SOURCES += ..\..\rt-thread\components\gui\widgets\listbox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\listctrl.c
SOURCES += ..\..\rt-thread\components\gui\widgets\menu.c
SOURCES += ..\..\rt-thread\components\gui\widgets\mv_view.c
SOURCES += ..\..\rt-thread\components\gui\widgets\plot.c
SOURCES += ..\..\rt-thread\components\gui\widgets\plot_curve.c
SOURCES += ..\..\rt-thread\components\gui\widgets\progressbar.c
SOURCES += ..\..\rt-thread\components\gui\widgets\radiobox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\scrollbar.c
SOURCES += ..\..\rt-thread\components\gui\widgets\slider.c
SOURCES += ..\..\rt-thread\components\gui\widgets\staticline.c
SOURCES += ..\..\rt-thread\components\gui\widgets\textbox.c
SOURCES += ..\..\rt-thread\components\gui\widgets\textview.c
SOURCES += ..\..\rt-thread\components\gui\widgets\panel.c
SOURCES += ..\..\rt-thread\components\gui\widgets\notebook.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_theme.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_mv_model.c
SOURCES += ..\..\rt-thread\components\gui\src\rtgui_xml.c


# head files
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\alarm.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\audio.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\can.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\hwtimer.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\i2c-bit-ops.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\i2c.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\i2c_dev.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mmcsd_card.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mmcsd_cmd.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mmcsd_core.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mmcsd_host.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mtd_nand.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\mtd_nor.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\pin.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\rtc.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\sd.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\sdio.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\sdio_func_ids.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\serial.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\spi.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\usb_common.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\usb_device.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\usb_host.h
HEADERS += ..\..\rt-thread\components\drivers\include\drivers\watchdog.h
HEADERS += ..\..\rt-thread\components\drivers\include\rtdevice.h
HEADERS += ..\..\rt-thread\components\finsh\finsh.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_error.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_heap.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_node.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_ops.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_parser.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_token.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_var.h
HEADERS += ..\..\rt-thread\components\finsh\finsh_vm.h
HEADERS += ..\..\rt-thread\components\finsh\msh.h
HEADERS += ..\..\rt-thread\components\finsh\shell.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\blit.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\color.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\dc.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\dc_client.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\dc_draw.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\dc_hw.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\dc_trans.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\driver.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\event.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\filerw.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\font.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\font_fnt.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image_bmp.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image_hdc.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image_jpeg.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image_png.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\image_xpm.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\kbddef.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\list.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\matrix.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\region.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_app.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_config.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_mv_model.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_object.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_server.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_system.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_theme.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\rtgui_xml.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\tree.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\box.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\button.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\caret.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\checkbox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\combobox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\container.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\digfont.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\digtube.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\edit.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\filelist_view.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\groupbox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\iconbox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\label.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\list_view.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\listbox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\listctrl.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\menu.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\mv_view.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\notebook.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\panel.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\plot.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\plot_curve.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\progressbar.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\radiobox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\scrollbar.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\slider.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\staticline.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\textbox.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\textview.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\title.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\widget.h
HEADERS += ..\..\rt-thread\components\gui\include\rtgui\widgets\window.h
HEADERS += ..\..\rt-thread\components\gui\src\mouse.h
HEADERS += ..\..\rt-thread\components\gui\src\topwin.h
HEADERS += ..\..\rt-thread\components\gui\widgets\text_encoding.h
HEADERS += ..\..\rt-thread\include\rtdebug.h
HEADERS += ..\..\rt-thread\include\rtdef.h
HEADERS += ..\..\rt-thread\include\rthw.h
HEADERS += ..\..\rt-thread\include\rtm.h
HEADERS += ..\..\rt-thread\include\rtservice.h
HEADERS += ..\..\rt-thread\include\rtthread.h
HEADERS += ..\..\rt-thread\libcpu\sim\win32\cpu_port.h
HEADERS += ..\embedded\Src\rtgui_demo.h
HEADERS += ..\embedded\board_driver\drv_usb_lcd.h
HEADERS += ..\embedded\calculator\calculator.h
HEADERS += ..\embedded\calculator\image_mem.h
HEADERS += ..\embedded\calculator\lcdwidget.h
HEADERS += applications\init.h
HEADERS += drivers\board.h
HEADERS += drivers\uart_console.h
HEADERS += drivers\usbd_video.h
HEADERS += rtconfig.h
HEADERS += ui_examples\demo_view.h
HEADERS += ui_examples\mywidget.h


# Include path
INCLUDEPATH += .
INCLUDEPATH += ..\..\rt-thread\components\drivers\include
INCLUDEPATH += ..\..\rt-thread\components\finsh
INCLUDEPATH += ..\..\rt-thread\components\gui\include
INCLUDEPATH += ..\..\rt-thread\include
INCLUDEPATH += ..\..\rt-thread\libcpu\sim\common
INCLUDEPATH += ..\..\rt-thread\libcpu\sim\win32
INCLUDEPATH += ..\embedded\calculator
INCLUDEPATH += applications
INCLUDEPATH += drivers


# defines
DEFINES += __fsymtab_start=__start_FSymTab
DEFINES += __fsymtab_end=__stop_FSymTab
DEFINES += __vsymtab_start=__start_VSymTab
DEFINES += __vsymtab_end=__stop_VSymTab


# libs
# lib paths
