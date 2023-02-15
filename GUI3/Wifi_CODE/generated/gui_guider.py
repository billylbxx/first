# Copyright 2021 NXP
# SPDX-License-Identifier: MIT

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct

lv.init()
SDL.init(w=240,h=240)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(240*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 240
disp_drv.ver_res = 240
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.add_flag(lv.obj.FLAG.HIDDEN)


screen = lv.obj()
# create style style_screen_main_main_default
style_screen_main_main_default = lv.style_t()
style_screen_main_main_default.init()
style_screen_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_main_main_default.set_bg_opa(0)

# add style for screen
screen.add_style(style_screen_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_tabview_1 = lv.tabview(screen, lv.DIR.TOP, 38)
screen_tabview_1_tab_btns = screen_tabview_1.get_tab_btns()
# create style style_screen_tabview_1_extra_btnm_main_default
style_screen_tabview_1_extra_btnm_main_default = lv.style_t()
style_screen_tabview_1_extra_btnm_main_default.init()
style_screen_tabview_1_extra_btnm_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_tabview_1_extra_btnm_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_tabview_1_extra_btnm_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_tabview_1_extra_btnm_main_default.set_bg_opa(255)
style_screen_tabview_1_extra_btnm_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tabview_1_extra_btnm_main_default.set_border_width(0)
style_screen_tabview_1_extra_btnm_main_default.set_border_opa(100)
style_screen_tabview_1_extra_btnm_main_default.set_pad_left(0)
style_screen_tabview_1_extra_btnm_main_default.set_pad_right(0)
style_screen_tabview_1_extra_btnm_main_default.set_pad_top(0)
style_screen_tabview_1_extra_btnm_main_default.set_pad_bottom(0)

# add style for screen_tabview_1_tab_btns
screen_tabview_1_tab_btns.add_style(style_screen_tabview_1_extra_btnm_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_tabview_1_extra_btnm_items_default
style_screen_tabview_1_extra_btnm_items_default = lv.style_t()
style_screen_tabview_1_extra_btnm_items_default.init()
style_screen_tabview_1_extra_btnm_items_default.set_text_color(lv.color_make(0x4d,0x4d,0x4d))
try:
    style_screen_tabview_1_extra_btnm_items_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_tabview_1_extra_btnm_items_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_tabview_1_extra_btnm_items_default.set_text_font(lv.font_montserrat_16)

# add style for screen_tabview_1_tab_btns
screen_tabview_1_tab_btns.add_style(style_screen_tabview_1_extra_btnm_items_default, lv.PART.ITEMS|lv.STATE.DEFAULT)

# create style style_screen_tabview_1_extra_btnm_items_checked
style_screen_tabview_1_extra_btnm_items_checked = lv.style_t()
style_screen_tabview_1_extra_btnm_items_checked.init()
style_screen_tabview_1_extra_btnm_items_checked.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tabview_1_extra_btnm_items_checked.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tabview_1_extra_btnm_items_checked.set_bg_opa(60)
style_screen_tabview_1_extra_btnm_items_checked.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_tabview_1_extra_btnm_items_checked.set_border_width(4)
style_screen_tabview_1_extra_btnm_items_checked.set_border_opa(255)
style_screen_tabview_1_extra_btnm_items_checked.set_border_side(lv.BORDER_SIDE.BOTTOM)
style_screen_tabview_1_extra_btnm_items_checked.set_text_color(lv.color_make(0x21,0x95,0xf6))
try:
    style_screen_tabview_1_extra_btnm_items_checked.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_tabview_1_extra_btnm_items_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_tabview_1_extra_btnm_items_checked.set_text_font(lv.font_montserrat_16)

# add style for screen_tabview_1_tab_btns
screen_tabview_1_tab_btns.add_style(style_screen_tabview_1_extra_btnm_items_checked, lv.PART.ITEMS|lv.STATE.CHECKED)

screen_tabview_1_title1 = screen_tabview_1.add_tab("title1")
screen_sw_1 = lv.switch(screen_tabview_1_title1)
screen_sw_1.set_pos(180,18)
screen_sw_1.set_size(40,20)
# create style style_screen_sw_1_main_main_default
style_screen_sw_1_main_main_default = lv.style_t()
style_screen_sw_1_main_main_default.init()
style_screen_sw_1_main_main_default.set_radius(100)
style_screen_sw_1_main_main_default.set_bg_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_1_main_main_default.set_bg_grad_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_1_main_main_default.set_bg_opa(255)

# add style for screen_sw_1
screen_sw_1.add_style(style_screen_sw_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_sw_1_main_indicator_checked
style_screen_sw_1_main_indicator_checked = lv.style_t()
style_screen_sw_1_main_indicator_checked.init()
style_screen_sw_1_main_indicator_checked.set_radius(100)
style_screen_sw_1_main_indicator_checked.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_1_main_indicator_checked.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_1_main_indicator_checked.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_1_main_indicator_checked.set_bg_opa(255)

# add style for screen_sw_1
screen_sw_1.add_style(style_screen_sw_1_main_indicator_checked, lv.PART.INDICATOR|lv.STATE.CHECKED)

# create style style_screen_sw_1_main_knob_default
style_screen_sw_1_main_knob_default = lv.style_t()
style_screen_sw_1_main_knob_default.init()
style_screen_sw_1_main_knob_default.set_radius(100)
style_screen_sw_1_main_knob_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_1_main_knob_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_1_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_1_main_knob_default.set_bg_opa(255)

# add style for screen_sw_1
screen_sw_1.add_style(style_screen_sw_1_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)

screen_cpicker_1 = lv.colorwheel(screen_tabview_1_title1, True)
screen_cpicker_1.set_pos(90,119)
screen_cpicker_1.set_size(60,60)
# create style style_screen_cpicker_1_main_main_default
style_screen_cpicker_1_main_main_default = lv.style_t()
style_screen_cpicker_1_main_main_default.init()
style_screen_cpicker_1_main_main_default.set_arc_width(10)

# add style for screen_cpicker_1
screen_cpicker_1.add_style(style_screen_cpicker_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_cb_3 = lv.checkbox(screen_tabview_1_title1)
screen_cb_3.set_pos(20,80)
screen_cb_3.set_text("LED3")
# create style style_screen_cb_3_main_main_default
style_screen_cb_3_main_main_default = lv.style_t()
style_screen_cb_3_main_main_default.init()
style_screen_cb_3_main_main_default.set_radius(6)
style_screen_cb_3_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_3_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_3_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_3_main_main_default.set_bg_opa(0)
style_screen_cb_3_main_main_default.set_text_color(lv.color_make(0x0D,0x30,0x55))
try:
    style_screen_cb_3_main_main_default.set_text_font(lv.font_simsun_16)
except AttributeError:
    try:
        style_screen_cb_3_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_cb_3_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_cb_3_main_main_default.set_text_letter_space(0)

# add style for screen_cb_3
screen_cb_3.add_style(style_screen_cb_3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_cb_3_main_indicator_default
style_screen_cb_3_main_indicator_default = lv.style_t()
style_screen_cb_3_main_indicator_default.init()
style_screen_cb_3_main_indicator_default.set_radius(6)
style_screen_cb_3_main_indicator_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_3_main_indicator_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_3_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_3_main_indicator_default.set_bg_opa(255)
style_screen_cb_3_main_indicator_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_cb_3_main_indicator_default.set_border_width(2)

# add style for screen_cb_3
screen_cb_3.add_style(style_screen_cb_3_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

screen_cb_2 = lv.checkbox(screen_tabview_1_title1)
screen_cb_2.set_pos(20,50)
screen_cb_2.set_text("LED2")
# create style style_screen_cb_2_main_main_default
style_screen_cb_2_main_main_default = lv.style_t()
style_screen_cb_2_main_main_default.init()
style_screen_cb_2_main_main_default.set_radius(6)
style_screen_cb_2_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_2_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_2_main_main_default.set_bg_opa(0)
style_screen_cb_2_main_main_default.set_text_color(lv.color_make(0x0D,0x30,0x55))
try:
    style_screen_cb_2_main_main_default.set_text_font(lv.font_simsun_16)
except AttributeError:
    try:
        style_screen_cb_2_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_cb_2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_cb_2_main_main_default.set_text_letter_space(0)

# add style for screen_cb_2
screen_cb_2.add_style(style_screen_cb_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_cb_2_main_indicator_default
style_screen_cb_2_main_indicator_default = lv.style_t()
style_screen_cb_2_main_indicator_default.init()
style_screen_cb_2_main_indicator_default.set_radius(6)
style_screen_cb_2_main_indicator_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_2_main_indicator_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_2_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_2_main_indicator_default.set_bg_opa(255)
style_screen_cb_2_main_indicator_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_cb_2_main_indicator_default.set_border_width(2)

# add style for screen_cb_2
screen_cb_2.add_style(style_screen_cb_2_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

screen_cb_1 = lv.checkbox(screen_tabview_1_title1)
screen_cb_1.set_pos(20,20)
screen_cb_1.set_text("LED1")
# create style style_screen_cb_1_main_main_default
style_screen_cb_1_main_main_default = lv.style_t()
style_screen_cb_1_main_main_default.init()
style_screen_cb_1_main_main_default.set_radius(6)
style_screen_cb_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_1_main_main_default.set_bg_opa(0)
style_screen_cb_1_main_main_default.set_text_color(lv.color_make(0x0D,0x30,0x55))
try:
    style_screen_cb_1_main_main_default.set_text_font(lv.font_simsun_16)
except AttributeError:
    try:
        style_screen_cb_1_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_cb_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_cb_1_main_main_default.set_text_letter_space(0)

# add style for screen_cb_1
screen_cb_1.add_style(style_screen_cb_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_cb_1_main_indicator_default
style_screen_cb_1_main_indicator_default = lv.style_t()
style_screen_cb_1_main_indicator_default.init()
style_screen_cb_1_main_indicator_default.set_radius(6)
style_screen_cb_1_main_indicator_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_1_main_indicator_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cb_1_main_indicator_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_cb_1_main_indicator_default.set_bg_opa(255)
style_screen_cb_1_main_indicator_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_cb_1_main_indicator_default.set_border_width(2)

# add style for screen_cb_1
screen_cb_1.add_style(style_screen_cb_1_main_indicator_default, lv.PART.INDICATOR|lv.STATE.DEFAULT)

screen_label_3 = lv.label(screen_tabview_1_title1)
screen_label_3.set_pos(145,85)
screen_label_3.set_size(40,20)
screen_label_3.set_text("RGB")
screen_label_3.set_long_mode(lv.label.LONG.WRAP)
screen_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
# create style style_screen_label_3_main_main_default
style_screen_label_3_main_main_default = lv.style_t()
style_screen_label_3_main_main_default.init()
style_screen_label_3_main_main_default.set_radius(0)
style_screen_label_3_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_3_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_3_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_label_3_main_main_default.set_bg_opa(0)
style_screen_label_3_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_label_3_main_main_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_label_3_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_label_3_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_label_3_main_main_default.set_text_letter_space(2)
style_screen_label_3_main_main_default.set_pad_left(0)
style_screen_label_3_main_main_default.set_pad_right(0)
style_screen_label_3_main_main_default.set_pad_top(0)
style_screen_label_3_main_main_default.set_pad_bottom(0)

# add style for screen_label_3
screen_label_3.add_style(style_screen_label_3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_label_2 = lv.label(screen_tabview_1_title1)
screen_label_2.set_pos(133,54)
screen_label_2.set_size(52,20)
screen_label_2.set_text("蜂鸣器")
screen_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
# create style style_screen_label_2_main_main_default
style_screen_label_2_main_main_default = lv.style_t()
style_screen_label_2_main_main_default.init()
style_screen_label_2_main_main_default.set_radius(0)
style_screen_label_2_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_2_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_label_2_main_main_default.set_bg_opa(0)
style_screen_label_2_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_label_2_main_main_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_label_2_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_label_2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_label_2_main_main_default.set_text_letter_space(2)
style_screen_label_2_main_main_default.set_pad_left(0)
style_screen_label_2_main_main_default.set_pad_right(0)
style_screen_label_2_main_main_default.set_pad_top(0)
style_screen_label_2_main_main_default.set_pad_bottom(0)

# add style for screen_label_2
screen_label_2.add_style(style_screen_label_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_label_1 = lv.label(screen_tabview_1_title1)
screen_label_1.set_pos(134,22)
screen_label_1.set_size(59,13)
screen_label_1.set_text("风扇")
screen_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)
# create style style_screen_label_1_main_main_default
style_screen_label_1_main_main_default = lv.style_t()
style_screen_label_1_main_main_default.init()
style_screen_label_1_main_main_default.set_radius(0)
style_screen_label_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_label_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_label_1_main_main_default.set_bg_opa(0)
style_screen_label_1_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_label_1_main_main_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_label_1_main_main_default.set_text_letter_space(2)
style_screen_label_1_main_main_default.set_pad_left(0)
style_screen_label_1_main_main_default.set_pad_right(0)
style_screen_label_1_main_main_default.set_pad_top(0)
style_screen_label_1_main_main_default.set_pad_bottom(0)

# add style for screen_label_1
screen_label_1.add_style(style_screen_label_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_sw_3 = lv.switch(screen_tabview_1_title1)
screen_sw_3.set_pos(180,82)
screen_sw_3.set_size(40,20)
# create style style_screen_sw_3_main_main_default
style_screen_sw_3_main_main_default = lv.style_t()
style_screen_sw_3_main_main_default.init()
style_screen_sw_3_main_main_default.set_radius(100)
style_screen_sw_3_main_main_default.set_bg_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_3_main_main_default.set_bg_grad_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_3_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_3_main_main_default.set_bg_opa(255)

# add style for screen_sw_3
screen_sw_3.add_style(style_screen_sw_3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_sw_3_main_indicator_checked
style_screen_sw_3_main_indicator_checked = lv.style_t()
style_screen_sw_3_main_indicator_checked.init()
style_screen_sw_3_main_indicator_checked.set_radius(100)
style_screen_sw_3_main_indicator_checked.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_3_main_indicator_checked.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_3_main_indicator_checked.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_3_main_indicator_checked.set_bg_opa(255)

# add style for screen_sw_3
screen_sw_3.add_style(style_screen_sw_3_main_indicator_checked, lv.PART.INDICATOR|lv.STATE.CHECKED)

# create style style_screen_sw_3_main_knob_default
style_screen_sw_3_main_knob_default = lv.style_t()
style_screen_sw_3_main_knob_default.init()
style_screen_sw_3_main_knob_default.set_radius(100)
style_screen_sw_3_main_knob_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_3_main_knob_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_3_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_3_main_knob_default.set_bg_opa(255)

# add style for screen_sw_3
screen_sw_3.add_style(style_screen_sw_3_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)

screen_sw_2 = lv.switch(screen_tabview_1_title1)
screen_sw_2.set_pos(180,50)
screen_sw_2.set_size(40,20)
# create style style_screen_sw_2_main_main_default
style_screen_sw_2_main_main_default = lv.style_t()
style_screen_sw_2_main_main_default.init()
style_screen_sw_2_main_main_default.set_radius(100)
style_screen_sw_2_main_main_default.set_bg_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_2_main_main_default.set_bg_grad_color(lv.color_make(0xe6,0xe2,0xe6))
style_screen_sw_2_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_2_main_main_default.set_bg_opa(255)

# add style for screen_sw_2
screen_sw_2.add_style(style_screen_sw_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_sw_2_main_indicator_checked
style_screen_sw_2_main_indicator_checked = lv.style_t()
style_screen_sw_2_main_indicator_checked.init()
style_screen_sw_2_main_indicator_checked.set_radius(100)
style_screen_sw_2_main_indicator_checked.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_2_main_indicator_checked.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_sw_2_main_indicator_checked.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_2_main_indicator_checked.set_bg_opa(255)

# add style for screen_sw_2
screen_sw_2.add_style(style_screen_sw_2_main_indicator_checked, lv.PART.INDICATOR|lv.STATE.CHECKED)

# create style style_screen_sw_2_main_knob_default
style_screen_sw_2_main_knob_default = lv.style_t()
style_screen_sw_2_main_knob_default.init()
style_screen_sw_2_main_knob_default.set_radius(100)
style_screen_sw_2_main_knob_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_2_main_knob_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_sw_2_main_knob_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_sw_2_main_knob_default.set_bg_opa(255)

# add style for screen_sw_2
screen_sw_2.add_style(style_screen_sw_2_main_knob_default, lv.PART.KNOB|lv.STATE.DEFAULT)

screen_tabview_1_title2 = screen_tabview_1.add_tab("title2")
screen_meter_1 = lv.meter(screen_tabview_1_title2)
screen_meter_1.set_pos(34,18)
screen_meter_1.set_size(167,167)
screen_meter_1_scale_1 = screen_meter_1.add_scale()
screen_meter_1.set_scale_ticks(screen_meter_1_scale_1, 41, 2, 10,
    lv.color_make(0x00, 0xbf, 0xff))
screen_meter_1.set_scale_major_ticks(screen_meter_1_scale_1, 8, 5, 15,
	lv.color_make(0xff, 0x00, 0x19), 10)
screen_meter_1.set_scale_range(screen_meter_1_scale_1, 0, 100, 300, 90)
# create style style_screen_meter_1_main_main_default
style_screen_meter_1_main_main_default = lv.style_t()
style_screen_meter_1_main_main_default.init()
style_screen_meter_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_meter_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_meter_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_meter_1_main_main_default.set_bg_opa(255)

# add style for screen_meter_1
screen_meter_1.add_style(style_screen_meter_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_meter_1_main_ticks_default
style_screen_meter_1_main_ticks_default = lv.style_t()
style_screen_meter_1_main_ticks_default.init()
style_screen_meter_1_main_ticks_default.set_text_color(lv.color_make(0xff,0x00,0x00))
try:
    style_screen_meter_1_main_ticks_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_meter_1_main_ticks_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_meter_1_main_ticks_default.set_text_font(lv.font_montserrat_16)

# add style for screen_meter_1
screen_meter_1.add_style(style_screen_meter_1_main_ticks_default, lv.PART.TICKS|lv.STATE.DEFAULT)

screen_tabview_1_title3 = screen_tabview_1.add_tab("title3")
screen_tabview_1_title3_label = lv.label(screen_tabview_1_title3)
screen_tabview_1_title3_label.set_text("con3")
screen_tabview_1.set_pos(0,0)
screen_tabview_1.set_size(240,240)
# create style style_screen_tabview_1_main_main_default
style_screen_tabview_1_main_main_default = lv.style_t()
style_screen_tabview_1_main_main_default.init()
style_screen_tabview_1_main_main_default.set_bg_color(lv.color_make(0xea,0xef,0xf3))
style_screen_tabview_1_main_main_default.set_bg_grad_color(lv.color_make(0xea,0xef,0xf3))
style_screen_tabview_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_tabview_1_main_main_default.set_bg_opa(255)
style_screen_tabview_1_main_main_default.set_border_color(lv.color_make(0xc0,0xc0,0xc0))
style_screen_tabview_1_main_main_default.set_border_width(0)
style_screen_tabview_1_main_main_default.set_border_opa(100)
style_screen_tabview_1_main_main_default.set_text_color(lv.color_make(0x4d,0x4d,0x4d))
try:
    style_screen_tabview_1_main_main_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_tabview_1_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_tabview_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_tabview_1_main_main_default.set_text_letter_space(2)
style_screen_tabview_1_main_main_default.set_text_line_space(16)

# add style for screen_tabview_1
screen_tabview_1.add_style(style_screen_tabview_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)



def screen_sw_3_value_changed_1_event_cb(e,screen_cpicker_1):
    src = e.get_target()
    code = e.get_code()
    screen_cpicker_1.add_flag(lv.obj.FLAG.HIDDEN)
screen_sw_3.add_event_cb(lambda e: screen_sw_3_value_changed_1_event_cb(e,screen_cpicker_1), lv.EVENT.VALUE_CHANGED, None)



# content from custom.py

# Load the default screen
lv.scr_load(screen)

while SDL.check():
    time.sleep_ms(5)
