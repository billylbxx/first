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

screen_label_1 = lv.label(screen)
screen_label_1.set_pos(70,12)
screen_label_1.set_size(110,24)
screen_label_1.set_text("聚四氟")
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
style_screen_label_1_main_main_default.set_text_color(lv.color_make(0xe7,0x8d,0x0d))
try:
    style_screen_label_1_main_main_default.set_text_font(lv.font_simsun_16)
except AttributeError:
    try:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_label_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_label_1_main_main_default.set_text_letter_space(2)
style_screen_label_1_main_main_default.set_pad_left(0)
style_screen_label_1_main_main_default.set_pad_right(0)
style_screen_label_1_main_main_default.set_pad_top(0)
style_screen_label_1_main_main_default.set_pad_bottom(0)

# add style for screen_label_1
screen_label_1.add_style(style_screen_label_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_ta_1 = lv.textarea(screen)
screen_ta_1.set_pos(8,43)
screen_ta_1.set_size(176,26)
screen_ta_1.set_text("Hello World")
try:
    screen_kb
except NameError:
    screen_kb = lv.keyboard(screen)

screen_kb.add_flag(lv.obj.FLAG.HIDDEN)
screen_kb.set_textarea(screen_ta_1)
screen_ta_1.add_event_cb(lambda e: ta_event_cb(e,screen_kb), lv.EVENT.ALL, None)
# create style style_screen_ta_1_main_main_default
style_screen_ta_1_main_main_default = lv.style_t()
style_screen_ta_1_main_main_default.init()
style_screen_ta_1_main_main_default.set_radius(4)
style_screen_ta_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_ta_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_ta_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_ta_1_main_main_default.set_bg_opa(255)
style_screen_ta_1_main_main_default.set_border_color(lv.color_make(0xe6,0xe6,0xe6))
style_screen_ta_1_main_main_default.set_border_width(2)
style_screen_ta_1_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_ta_1_main_main_default.set_text_font(lv.font_simsun_12)
except AttributeError:
    try:
        style_screen_ta_1_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_ta_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_ta_1_main_main_default.set_text_letter_space(2)
style_screen_ta_1_main_main_default.set_text_align(lv.TEXT_ALIGN.LEFT)
style_screen_ta_1_main_main_default.set_pad_left(4)
style_screen_ta_1_main_main_default.set_pad_right(4)
style_screen_ta_1_main_main_default.set_pad_top(4)
style_screen_ta_1_main_main_default.set_pad_bottom(4)

# add style for screen_ta_1
screen_ta_1.add_style(style_screen_ta_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_ta_1_main_scrollbar_default
style_screen_ta_1_main_scrollbar_default = lv.style_t()
style_screen_ta_1_main_scrollbar_default.init()
style_screen_ta_1_main_scrollbar_default.set_radius(0)
style_screen_ta_1_main_scrollbar_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_ta_1_main_scrollbar_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_ta_1_main_scrollbar_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_ta_1_main_scrollbar_default.set_bg_opa(255)

# add style for screen_ta_1
screen_ta_1.add_style(style_screen_ta_1_main_scrollbar_default, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

screen_btn_1 = lv.btn(screen)
screen_btn_1.set_pos(187,42)
screen_btn_1.set_size(45,26)
screen_btn_1_label = lv.label(screen_btn_1)
screen_btn_1_label.set_text("确认")
screen_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_1_label.align(lv.ALIGN.CENTER,0,0)
screen_btn_1_label.set_style_text_color(lv.color_make(0x00,0x00,0x00), lv.STATE.DEFAULT)
try:
    screen_btn_1_label.set_style_text_font(lv.font_simsun_12, lv.STATE.DEFAULT)
except AttributeError:
    try:
        screen_btn_1_label.set_style_text_font(lv.font_montserrat_12, lv.STATE.DEFAULT)
    except AttributeError:
        screen_btn_1_label.set_style_text_font(lv.font_montserrat_16, lv.STATE.DEFAULT)
# create style style_screen_btn_1_main_main_default
style_screen_btn_1_main_main_default = lv.style_t()
style_screen_btn_1_main_main_default.init()
style_screen_btn_1_main_main_default.set_radius(5)
style_screen_btn_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_btn_1_main_main_default.set_bg_opa(255)
style_screen_btn_1_main_main_default.set_shadow_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_shadow_opa(255)
style_screen_btn_1_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_border_width(0)
style_screen_btn_1_main_main_default.set_border_opa(255)

# add style for screen_btn_1
screen_btn_1.add_style(style_screen_btn_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_img_1 = lv.img(screen)
screen_img_1.set_pos(60,100)
screen_img_1.set_size(120,120)
screen_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
try:
    with open('D:\\guider\\test1\\generated\\mp1602774216.png','rb') as f:
        screen_img_1_img_data = f.read()
except:
    print('Could not open D:\\guider\\test1\\generated\\mp1602774216.png')
    sys.exit()

screen_img_1_img = lv.img_dsc_t({
  'data_size': len(screen_img_1_img_data),
  'header': {'always_zero': 0, 'w': 120, 'h': 120, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_img_1_img_data
})

screen_img_1.set_src(screen_img_1_img)
screen_img_1.set_pivot(0,0)
screen_img_1.set_angle(0)
# create style style_screen_img_1_main_main_default
style_screen_img_1_main_main_default = lv.style_t()
style_screen_img_1_main_main_default.init()
style_screen_img_1_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_img_1_main_main_default.set_img_recolor_opa(0)
style_screen_img_1_main_main_default.set_img_opa(255)

# add style for screen_img_1
screen_img_1.add_style(style_screen_img_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)



def screen_btn_1_clicked_1_event_cb(e,screen_ta_1):
    src = e.get_target()
    code = e.get_code()
    try:
        screen_ta_1.set_style_text_font(lv.font_simsun_16, lv.STATE.DEFAULT)
    except AttributeError:
        try:
            screen_ta_1.set_style_text_font(lv.font_montserrat_16, lv.STATE.DEFAULT)
        except AttributeError:
            screen_ta_1.set_style_text_font(lv.font_montserrat_16, lv.STATE.DEFAULT)
    screen_ta_1.set_text("按键按下")

screen_btn_1.add_event_cb(lambda e: screen_btn_1_clicked_1_event_cb(e,screen_ta_1), lv.EVENT.CLICKED, None)



# content from custom.py

# Load the default screen
lv.scr_load(screen)

while SDL.check():
    time.sleep_ms(5)
