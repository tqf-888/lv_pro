# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct
import fs_driver

lv.init()
SDL.init(w=1280,h=800)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(1280*800*4)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 1280
disp_drv.ver_res = 800
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

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

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_zoom(v)

def anim_img_rotate_cb(obj, v):
    obj.set_angle(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.font_load(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.img_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = e.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    target = e.get_target()
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = target.get_screen()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = target.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, target), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    target = e.get_current_target()
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if target.get_pressed_date(date) == lv.RES.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.clear_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            target.delete()

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_100
screen_100 = lv.obj()
g_kb_screen_100 = lv.keyboard(screen_100)
g_kb_screen_100.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_100), lv.EVENT.ALL, None)
g_kb_screen_100.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_100.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100.set_size(1280, 800)
screen_100.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_100, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_100.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_100_btn_1
screen_100_btn_1 = lv.btn(screen_100)
screen_100_btn_1_label = lv.label(screen_100_btn_1)
screen_100_btn_1_label.set_text("Button")
screen_100_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_100_btn_1_label.set_width(lv.pct(100))
screen_100_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_100_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_100_btn_1.set_pos(51, 60)
screen_100_btn_1.set_size(575, 222)
# Set style for screen_100_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_100_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_bg_color(lv.color_hex(0xa600ff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_100_btn_2
screen_100_btn_2 = lv.btn(screen_100)
screen_100_btn_2_label = lv.label(screen_100_btn_2)
screen_100_btn_2_label.set_text("Button")
screen_100_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
screen_100_btn_2_label.set_width(lv.pct(100))
screen_100_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_100_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_100_btn_2.set_pos(74, 320)
screen_100_btn_2.set_size(573, 214)
# Set style for screen_100_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_100_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_bg_color(lv.color_hex(0xff6500), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_100_label_1
screen_100_label_1 = lv.label(screen_100)
screen_100_label_1.set_text("01234\n\n5 6 7 8")
screen_100_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_100_label_1.set_width(lv.pct(100))
screen_100_label_1.set_pos(695, 166)
screen_100_label_1.set_size(438, 214)
# Set style for screen_100_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_100_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_font(test_font("lv_font_ktv_30", 30), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_100_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_100.update_layout()
# Create screen_4
screen_4 = lv.obj()
g_kb_screen_4 = lv.keyboard(screen_4)
g_kb_screen_4.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_4), lv.EVENT.ALL, None)
g_kb_screen_4.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_4.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_size(1280, 800)
screen_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_list_1
screen_4_list_1 = lv.list(screen_4)
screen_4_list_1_item0 = screen_4_list_1.add_btn(lv.SYMBOL.IMAGE, "   图像")
screen_4_list_1_item1 = screen_4_list_1.add_btn(lv.SYMBOL.AUDIO, "声音")
screen_4_list_1_item2 = screen_4_list_1.add_btn(lv.SYMBOL.SETTINGS, "系统")
screen_4_list_1_item3 = screen_4_list_1.add_btn(lv.SYMBOL.VIDEO, "梯形校正")
screen_4_list_1_item4 = screen_4_list_1.add_btn(lv.SYMBOL.LOOP, "日历")
screen_4_list_1.set_pos(22, 104)
screen_4_list_1.set_size(322, 667)
screen_4_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_list_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_bg_color(lv.color_hex(0x1e1e1e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_border_opa(25, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_border_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_radius(22, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_list_1_extra_btns_main_default = lv.style_t()
style_screen_4_list_1_extra_btns_main_default.init()
style_screen_4_list_1_extra_btns_main_default.set_pad_top(26)
style_screen_4_list_1_extra_btns_main_default.set_pad_left(26)
style_screen_4_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_4_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_4_list_1_extra_btns_main_default.set_border_width(0)
style_screen_4_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0xffffff))
style_screen_4_list_1_extra_btns_main_default.set_text_font(test_font("Regular", 42))
style_screen_4_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_4_list_1_extra_btns_main_default.set_radius(0)
style_screen_4_list_1_extra_btns_main_default.set_bg_opa(0)
screen_4_list_1_item4.add_style(style_screen_4_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1_item3.add_style(style_screen_4_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1_item2.add_style(style_screen_4_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1_item1.add_style(style_screen_4_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_list_1_item0.add_style(style_screen_4_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_list_1_extra_texts_main_default = lv.style_t()
style_screen_4_list_1_extra_texts_main_default.init()
style_screen_4_list_1_extra_texts_main_default.set_pad_top(50)
style_screen_4_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_4_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_4_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_4_list_1_extra_texts_main_default.set_border_width(0)
style_screen_4_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0xffffff))
style_screen_4_list_1_extra_texts_main_default.set_text_font(test_font("Regular", 24))
style_screen_4_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_4_list_1_extra_texts_main_default.set_radius(4)
style_screen_4_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_4_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_4_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_4_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

# Create screen_4_cont_9
screen_4_cont_9 = lv.obj(screen_4)
screen_4_cont_9.set_pos(364, 14)
screen_4_cont_9.set_size(868, 776)
screen_4_cont_9.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_bg_opa(45, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_cont_17
screen_4_cont_17 = lv.obj(screen_4_cont_9)
screen_4_cont_17.set_pos(34, 672)
screen_4_cont_17.set_size(792, 64)
screen_4_cont_17.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_17, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_17.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_17.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_8
screen_4_label_8 = lv.label(screen_4_cont_17)
screen_4_label_8.set_text("缩放")
screen_4_label_8.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_8.set_width(lv.pct(100))
screen_4_label_8.set_pos(15, 16)
screen_4_label_8.set_size(204, 36)
# Set style for screen_4_label_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_7
screen_4_slider_7 = lv.slider(screen_4_cont_17)
screen_4_slider_7.set_range(80, 100)
screen_4_slider_7.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_7.set_value(90, lv.ANIM.OFF)
screen_4_slider_7.set_pos(272, 30)
screen_4_slider_7.set_size(388, 12)
# Set style for screen_4_slider_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_7.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_7, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_7.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_7, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_7.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_7.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_16
screen_4_cont_16 = lv.obj(screen_4_cont_9)
screen_4_cont_16.set_pos(34, 580)
screen_4_cont_16.set_size(792, 64)
screen_4_cont_16.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_16.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_16.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_7
screen_4_label_7 = lv.label(screen_4_cont_16)
screen_4_label_7.set_text("画面比例")
screen_4_label_7.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_7.set_width(lv.pct(100))
screen_4_label_7.set_pos(15, 16)
screen_4_label_7.set_size(204, 36)
# Set style for screen_4_label_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_5
screen_4_ddlist_5 = lv.dropdown(screen_4_cont_16)
screen_4_ddlist_5.set_options("16:9\n4:3")
screen_4_ddlist_5.set_pos(602, 15)
screen_4_ddlist_5.set_size(174, 39)
# Set style for screen_4_ddlist_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_5.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_5, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_5_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_5_extra_list_selected_checked.init()
style_screen_4_ddlist_5_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_5_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_5_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_5_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_5_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_5.get_list().add_style(style_screen_4_ddlist_5_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_5_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_5_extra_list_main_default.init()
style_screen_4_ddlist_5_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_5_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_5_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_5_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_5_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_5_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_5_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_5_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_5_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_5.get_list().add_style(style_screen_4_ddlist_5_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_15
screen_4_cont_15 = lv.obj(screen_4_cont_9)
screen_4_cont_15.set_pos(34, 489)
screen_4_cont_15.set_size(792, 64)
screen_4_cont_15.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_6
screen_4_label_6 = lv.label(screen_4_cont_15)
screen_4_label_6.set_text("色温")
screen_4_label_6.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_6.set_width(lv.pct(100))
screen_4_label_6.set_pos(15, 16)
screen_4_label_6.set_size(204, 36)
# Set style for screen_4_label_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_4
screen_4_ddlist_4 = lv.dropdown(screen_4_cont_15)
screen_4_ddlist_4.set_options("标准\n冷色温\n暖色温")
screen_4_ddlist_4.set_pos(601, 15)
screen_4_ddlist_4.set_size(174, 39)
# Set style for screen_4_ddlist_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_4.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_4, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_4_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_4_extra_list_selected_checked.init()
style_screen_4_ddlist_4_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_4_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_4_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_4_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_4_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_4.get_list().add_style(style_screen_4_ddlist_4_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_4_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_4_extra_list_main_default.init()
style_screen_4_ddlist_4_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_4_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_4_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_4_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_4_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_4_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_4_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_4_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_4_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_4.get_list().add_style(style_screen_4_ddlist_4_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_14
screen_4_cont_14 = lv.obj(screen_4_cont_9)
screen_4_cont_14.set_pos(34, 398)
screen_4_cont_14.set_size(792, 64)
screen_4_cont_14.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_14.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_14.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_5
screen_4_label_5 = lv.label(screen_4_cont_14)
screen_4_label_5.set_text("清晰度")
screen_4_label_5.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_5.set_width(lv.pct(100))
screen_4_label_5.set_pos(14, 17)
screen_4_label_5.set_size(204, 36)
# Set style for screen_4_label_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_4
screen_4_slider_4 = lv.slider(screen_4_cont_14)
screen_4_slider_4.set_range(0, 10)
screen_4_slider_4.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_4.set_value(5, lv.ANIM.OFF)
screen_4_slider_4.set_pos(277, 29)
screen_4_slider_4.set_size(388, 12)
# Set style for screen_4_slider_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_4.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_4, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_4.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_4, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_4.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_4.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_13
screen_4_cont_13 = lv.obj(screen_4_cont_9)
screen_4_cont_13.set_pos(34, 307)
screen_4_cont_13.set_size(792, 64)
screen_4_cont_13.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_4
screen_4_label_4 = lv.label(screen_4_cont_13)
screen_4_label_4.set_text("颜色")
screen_4_label_4.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_4.set_width(lv.pct(100))
screen_4_label_4.set_pos(14, 17)
screen_4_label_4.set_size(204, 36)
# Set style for screen_4_label_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_3
screen_4_slider_3 = lv.slider(screen_4_cont_13)
screen_4_slider_3.set_range(0, 100)
screen_4_slider_3.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_3.set_value(50, lv.ANIM.OFF)
screen_4_slider_3.set_pos(273, 29)
screen_4_slider_3.set_size(388, 12)
# Set style for screen_4_slider_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_3.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_3, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_3.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_3, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_3.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_3.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_12
screen_4_cont_12 = lv.obj(screen_4_cont_9)
screen_4_cont_12.set_pos(34, 216)
screen_4_cont_12.set_size(792, 64)
screen_4_cont_12.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_3
screen_4_label_3 = lv.label(screen_4_cont_12)
screen_4_label_3.set_text("亮度")
screen_4_label_3.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_3.set_width(lv.pct(100))
screen_4_label_3.set_pos(15, 16)
screen_4_label_3.set_size(204, 36)
# Set style for screen_4_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_2
screen_4_slider_2 = lv.slider(screen_4_cont_12)
screen_4_slider_2.set_range(0, 100)
screen_4_slider_2.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_2.set_value(50, lv.ANIM.OFF)
screen_4_slider_2.set_pos(276, 29)
screen_4_slider_2.set_size(388, 12)
# Set style for screen_4_slider_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_2.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_2, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_2.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_2, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_2.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_2.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_11
screen_4_cont_11 = lv.obj(screen_4_cont_9)
screen_4_cont_11.set_pos(34, 127)
screen_4_cont_11.set_size(792, 64)
screen_4_cont_11.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_2
screen_4_label_2 = lv.label(screen_4_cont_11)
screen_4_label_2.set_text("对比度")
screen_4_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_2.set_width(lv.pct(100))
screen_4_label_2.set_pos(15, 16)
screen_4_label_2.set_size(204, 36)
# Set style for screen_4_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_1
screen_4_slider_1 = lv.slider(screen_4_cont_11)
screen_4_slider_1.set_range(0, 100)
screen_4_slider_1.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_1.set_value(50, lv.ANIM.OFF)
screen_4_slider_1.set_pos(266, 28)
screen_4_slider_1.set_size(388, 12)
# Set style for screen_4_slider_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_1.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_1, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_1.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_1, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_1.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_1.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_10
screen_4_cont_10 = lv.obj(screen_4_cont_9)
screen_4_cont_10.set_pos(34, 34)
screen_4_cont_10.set_size(792, 64)
screen_4_cont_10.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_1
screen_4_label_1 = lv.label(screen_4_cont_10)
screen_4_label_1.set_text("图像模式")
screen_4_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_1.set_width(lv.pct(100))
screen_4_label_1.set_pos(15, 16)
screen_4_label_1.set_size(204, 36)
# Set style for screen_4_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_1
screen_4_ddlist_1 = lv.dropdown(screen_4_cont_10)
screen_4_ddlist_1.set_options("标准\n动态\n温和\n用户")
screen_4_ddlist_1.set_pos(603, 12)
screen_4_ddlist_1.set_size(174, 39)
# Set style for screen_4_ddlist_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_1.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_1, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_1_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_1_extra_list_selected_checked.init()
style_screen_4_ddlist_1_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_1_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_1_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_1_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_1_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_1.get_list().add_style(style_screen_4_ddlist_1_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_1_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_1_extra_list_main_default.init()
style_screen_4_ddlist_1_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_1_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_1_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_1_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_1_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_1_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_1_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_1_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_1_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_1.get_list().add_style(style_screen_4_ddlist_1_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_label_9
screen_4_label_9 = lv.label(screen_4)
screen_4_label_9.set_text("设置")
screen_4_label_9.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_9.set_width(lv.pct(100))
screen_4_label_9.set_pos(53, 41)
screen_4_label_9.set_size(129, 39)
# Set style for screen_4_label_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_18
screen_4_cont_18 = lv.obj(screen_4)
screen_4_cont_18.set_pos(364, 71)
screen_4_cont_18.set_size(868, 412)
screen_4_cont_18.add_flag(lv.obj.FLAG.HIDDEN)
screen_4_cont_18.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_18, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_18.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_bg_opa(45, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_18.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_cont_24
screen_4_cont_24 = lv.obj(screen_4_cont_18)
screen_4_cont_24.set_pos(36, 307)
screen_4_cont_24.set_size(792, 64)
screen_4_cont_24.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_24, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_24.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_24.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_15
screen_4_label_15 = lv.label(screen_4_cont_24)
screen_4_label_15.set_text("声音输出")
screen_4_label_15.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_15.set_width(lv.pct(100))
screen_4_label_15.set_pos(15, 16)
screen_4_label_15.set_size(204, 36)
# Set style for screen_4_label_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_7
screen_4_ddlist_7 = lv.dropdown(screen_4_cont_24)
screen_4_ddlist_7.set_options("标准\n喇叭\nHDMI ARC\n蓝牙\n耳机\nOWA")
screen_4_ddlist_7.set_pos(601, 15)
screen_4_ddlist_7.set_size(174, 39)
# Set style for screen_4_ddlist_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_7.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_7, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_7_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_7_extra_list_selected_checked.init()
style_screen_4_ddlist_7_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_7_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_7_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_7_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_7_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_7.get_list().add_style(style_screen_4_ddlist_7_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_7_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_7_extra_list_main_default.init()
style_screen_4_ddlist_7_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_7_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_7_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_7_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_7_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_7_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_7_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_7_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_7_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_7.get_list().add_style(style_screen_4_ddlist_7_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_21
screen_4_cont_21 = lv.obj(screen_4_cont_18)
screen_4_cont_21.set_pos(34, 216)
screen_4_cont_21.set_size(792, 64)
screen_4_cont_21.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_21, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_21.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_21.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_12
screen_4_label_12 = lv.label(screen_4_cont_21)
screen_4_label_12.set_text("高音")
screen_4_label_12.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_12.set_width(lv.pct(100))
screen_4_label_12.set_pos(15, 16)
screen_4_label_12.set_size(204, 36)
# Set style for screen_4_label_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_6
screen_4_slider_6 = lv.slider(screen_4_cont_21)
screen_4_slider_6.set_range(-10, 10)
screen_4_slider_6.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_6.set_value(0, lv.ANIM.OFF)
screen_4_slider_6.set_pos(276, 30)
screen_4_slider_6.set_size(388, 12)
# Set style for screen_4_slider_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_6.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_6, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_6.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_6, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_6.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_6.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_20
screen_4_cont_20 = lv.obj(screen_4_cont_18)
screen_4_cont_20.set_pos(34, 127)
screen_4_cont_20.set_size(792, 64)
screen_4_cont_20.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_20, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_20.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_20.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_11
screen_4_label_11 = lv.label(screen_4_cont_20)
screen_4_label_11.set_text("低音")
screen_4_label_11.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_11.set_width(lv.pct(100))
screen_4_label_11.set_pos(15, 16)
screen_4_label_11.set_size(204, 36)
# Set style for screen_4_label_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_slider_5
screen_4_slider_5 = lv.slider(screen_4_cont_20)
screen_4_slider_5.set_range(-10, 10)
screen_4_slider_5.set_mode(lv.slider.MODE.NORMAL)
screen_4_slider_5.set_value(0, lv.ANIM.OFF)
screen_4_slider_5.set_pos(266, 28)
screen_4_slider_5.set_size(388, 12)
# Set style for screen_4_slider_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_slider_5.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_slider_5, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_4_slider_5.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_4_slider_5, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_slider_5.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_slider_5.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_19
screen_4_cont_19 = lv.obj(screen_4_cont_18)
screen_4_cont_19.set_pos(34, 34)
screen_4_cont_19.set_size(792, 64)
screen_4_cont_19.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_19, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_19.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_19.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_10
screen_4_label_10 = lv.label(screen_4_cont_19)
screen_4_label_10.set_text("声音模式")
screen_4_label_10.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_10.set_width(lv.pct(100))
screen_4_label_10.set_pos(15, 15)
screen_4_label_10.set_size(204, 36)
# Set style for screen_4_label_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_6
screen_4_ddlist_6 = lv.dropdown(screen_4_cont_19)
screen_4_ddlist_6.set_options("标准\n音乐\n电影\n运动")
screen_4_ddlist_6.set_pos(602, 11)
screen_4_ddlist_6.set_size(174, 39)
# Set style for screen_4_ddlist_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_6.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_6, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_6_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_6_extra_list_selected_checked.init()
style_screen_4_ddlist_6_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_6_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_6_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_6_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_6_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_6.get_list().add_style(style_screen_4_ddlist_6_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_6_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_6_extra_list_main_default.init()
style_screen_4_ddlist_6_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_6_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_6_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_6_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_6_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_6_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_6_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_6_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_6_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_6.get_list().add_style(style_screen_4_ddlist_6_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_25
screen_4_cont_25 = lv.obj(screen_4)
screen_4_cont_25.set_pos(369, 112)
screen_4_cont_25.set_size(868, 502)
screen_4_cont_25.add_flag(lv.obj.FLAG.HIDDEN)
screen_4_cont_25.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_25, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_25.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_bg_opa(45, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_25.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_cont_31
screen_4_cont_31 = lv.obj(screen_4_cont_25)
screen_4_cont_31.set_pos(34, 125)
screen_4_cont_31.set_size(792, 64)
screen_4_cont_31.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_31, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_31.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_31.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_21
screen_4_label_21 = lv.label(screen_4_cont_31)
screen_4_label_21.set_text("投屏模式")
screen_4_label_21.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_21.set_width(lv.pct(100))
screen_4_label_21.set_pos(15, 16)
screen_4_label_21.set_size(204, 36)
# Set style for screen_4_label_21, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_21.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_21.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_9
screen_4_ddlist_9 = lv.dropdown(screen_4_cont_31)
screen_4_ddlist_9.set_options("桌上正投\n吊装正投\n桌上背投\n吊装背投")
screen_4_ddlist_9.set_pos(601, 15)
screen_4_ddlist_9.set_size(174, 39)
# Set style for screen_4_ddlist_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_9.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_9, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_9_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_9_extra_list_selected_checked.init()
style_screen_4_ddlist_9_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_9_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_9_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_9_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_9_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_9.get_list().add_style(style_screen_4_ddlist_9_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_9_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_9_extra_list_main_default.init()
style_screen_4_ddlist_9_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_9_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_9_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_9_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_9_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_9_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_9_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_9_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_9_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_9.get_list().add_style(style_screen_4_ddlist_9_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_26
screen_4_cont_26 = lv.obj(screen_4_cont_25)
screen_4_cont_26.set_pos(34, 34)
screen_4_cont_26.set_size(792, 64)
screen_4_cont_26.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_26, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_26.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_26.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_16
screen_4_label_16 = lv.label(screen_4_cont_26)
screen_4_label_16.set_text("语言设置")
screen_4_label_16.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_16.set_width(lv.pct(100))
screen_4_label_16.set_pos(15, 16)
screen_4_label_16.set_size(204, 36)
# Set style for screen_4_label_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_16.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_16.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_8
screen_4_ddlist_8 = lv.dropdown(screen_4_cont_26)
screen_4_ddlist_8.set_options("中文")
screen_4_ddlist_8.set_pos(603, 12)
screen_4_ddlist_8.set_size(174, 39)
# Set style for screen_4_ddlist_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_8.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_8, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_8_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_8_extra_list_selected_checked.init()
style_screen_4_ddlist_8_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_8_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_8_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_8_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_8_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_8.get_list().add_style(style_screen_4_ddlist_8_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_8_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_8_extra_list_main_default.init()
style_screen_4_ddlist_8_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_8_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_8_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_8_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_8_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_8_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_8_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_8_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_8_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_8.get_list().add_style(style_screen_4_ddlist_8_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_35
screen_4_cont_35 = lv.obj(screen_4_cont_25)
screen_4_cont_35.set_pos(34, 216)
screen_4_cont_35.set_size(792, 64)
screen_4_cont_35.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_35, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_35.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_35.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_25
screen_4_label_25 = lv.label(screen_4_cont_35)
screen_4_label_25.set_text("恢复出厂设置")
screen_4_label_25.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_25.set_width(lv.pct(100))
screen_4_label_25.set_pos(15, 16)
screen_4_label_25.set_size(204, 36)
# Set style for screen_4_label_25, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_25.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_25.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_34
screen_4_cont_34 = lv.obj(screen_4_cont_25)
screen_4_cont_34.set_pos(34, 307)
screen_4_cont_34.set_size(792, 64)
screen_4_cont_34.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_34, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_34.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_34.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_24
screen_4_label_24 = lv.label(screen_4_cont_34)
screen_4_label_24.set_text("软件升级")
screen_4_label_24.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_24.set_width(lv.pct(100))
screen_4_label_24.set_pos(15, 16)
screen_4_label_24.set_size(204, 36)
# Set style for screen_4_label_24, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_24.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_24.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_33
screen_4_cont_33 = lv.obj(screen_4_cont_25)
screen_4_cont_33.set_pos(34, 398)
screen_4_cont_33.set_size(792, 64)
screen_4_cont_33.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_33, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_33.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_33.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_23
screen_4_label_23 = lv.label(screen_4_cont_33)
screen_4_label_23.set_text("自动休眠")
screen_4_label_23.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_23.set_width(lv.pct(100))
screen_4_label_23.set_pos(15, 16)
screen_4_label_23.set_size(204, 36)
# Set style for screen_4_label_23, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_23.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_23.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_ddlist_11
screen_4_ddlist_11 = lv.dropdown(screen_4_cont_33)
screen_4_ddlist_11.set_options("10分钟\n20分钟\n30分钟")
screen_4_ddlist_11.set_pos(602, 15)
screen_4_ddlist_11.set_size(174, 39)
# Set style for screen_4_ddlist_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_ddlist_11.set_style_text_color(lv.color_hex(0x0D3055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_text_font(test_font("Regular", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_pad_left(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_pad_right(6, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_ddlist_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_ddlist_11, Part: lv.PART.SELECTED, State: lv.STATE.CHECKED.
style_screen_4_ddlist_11_extra_list_selected_checked = lv.style_t()
style_screen_4_ddlist_11_extra_list_selected_checked.init()
style_screen_4_ddlist_11_extra_list_selected_checked.set_border_width(0)
style_screen_4_ddlist_11_extra_list_selected_checked.set_radius(0)
style_screen_4_ddlist_11_extra_list_selected_checked.set_bg_opa(255)
style_screen_4_ddlist_11_extra_list_selected_checked.set_bg_color(lv.color_hex(0x999999))
style_screen_4_ddlist_11_extra_list_selected_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_11.get_list().add_style(style_screen_4_ddlist_11_extra_list_selected_checked, lv.PART.SELECTED|lv.STATE.CHECKED)
# Set style for screen_4_ddlist_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_ddlist_11_extra_list_main_default = lv.style_t()
style_screen_4_ddlist_11_extra_list_main_default.init()
style_screen_4_ddlist_11_extra_list_main_default.set_max_height(160)
style_screen_4_ddlist_11_extra_list_main_default.set_text_color(lv.color_hex(0x000000))
style_screen_4_ddlist_11_extra_list_main_default.set_text_font(test_font("Regular", 20))
style_screen_4_ddlist_11_extra_list_main_default.set_text_opa(255)
style_screen_4_ddlist_11_extra_list_main_default.set_border_width(0)
style_screen_4_ddlist_11_extra_list_main_default.set_radius(0)
style_screen_4_ddlist_11_extra_list_main_default.set_bg_opa(255)
style_screen_4_ddlist_11_extra_list_main_default.set_bg_color(lv.color_hex(0xefefef))
style_screen_4_ddlist_11_extra_list_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_ddlist_11.get_list().add_style(style_screen_4_ddlist_11_extra_list_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_36
screen_4_cont_36 = lv.obj(screen_4)
screen_4_cont_36.set_pos(369, 112)
screen_4_cont_36.set_size(868, 502)
screen_4_cont_36.add_flag(lv.obj.FLAG.HIDDEN)
screen_4_cont_36.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_36, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_36.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_bg_opa(45, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_36.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_cont_41
screen_4_cont_41 = lv.obj(screen_4_cont_36)
screen_4_cont_41.set_pos(34, 125)
screen_4_cont_41.set_size(792, 64)
screen_4_cont_41.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_41, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_41.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_41.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_30
screen_4_label_30 = lv.label(screen_4_cont_41)
screen_4_label_30.set_text("手动梯形校正")
screen_4_label_30.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_30.set_width(lv.pct(100))
screen_4_label_30.set_pos(15, 16)
screen_4_label_30.set_size(204, 36)
# Set style for screen_4_label_30, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_30.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_30.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_40
screen_4_cont_40 = lv.obj(screen_4_cont_36)
screen_4_cont_40.set_pos(34, 34)
screen_4_cont_40.set_size(792, 64)
screen_4_cont_40.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_40, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_40.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_40.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_29
screen_4_label_29 = lv.label(screen_4_cont_40)
screen_4_label_29.set_text("自动梯形校正")
screen_4_label_29.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_29.set_width(lv.pct(100))
screen_4_label_29.set_pos(15, 16)
screen_4_label_29.set_size(204, 36)
# Set style for screen_4_label_29, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_29.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_29.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_sw_1
screen_4_sw_1 = lv.switch(screen_4_cont_40)
screen_4_sw_1.set_pos(654, 21)
screen_4_sw_1.set_size(77, 28)
# Set style for screen_4_sw_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_sw_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_bg_color(lv.color_hex(0xe6e2e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_sw_1, Part: lv.PART.INDICATOR, State: lv.STATE.CHECKED.
screen_4_sw_1.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_4_sw_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_4_sw_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.CHECKED)
screen_4_sw_1.set_style_border_width(0, lv.PART.INDICATOR|lv.STATE.CHECKED)

# Set style for screen_4_sw_1, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_4_sw_1.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_border_width(0, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_4_sw_1.set_style_radius(10, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_4_cont_39
screen_4_cont_39 = lv.obj(screen_4_cont_36)
screen_4_cont_39.set_pos(35, 216)
screen_4_cont_39.set_size(792, 64)
screen_4_cont_39.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_39, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_39.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_bg_opa(115, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_39.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_label_28
screen_4_label_28 = lv.label(screen_4_cont_39)
screen_4_label_28.set_text("手动梯形校正")
screen_4_label_28.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_28.set_width(lv.pct(100))
screen_4_label_28.set_pos(15, 16)
screen_4_label_28.set_size(204, 36)
# Set style for screen_4_label_28, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_28.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_28.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_label_31
screen_4_label_31 = lv.label(screen_4_cont_39)
screen_4_label_31.set_text("重置")
screen_4_label_31.set_long_mode(lv.label.LONG.WRAP)
screen_4_label_31.set_width(lv.pct(100))
screen_4_label_31.set_pos(656, 15)
screen_4_label_31.set_size(100, 36)
# Set style for screen_4_label_31, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_label_31.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_label_31.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_cont_42
screen_4_cont_42 = lv.obj(screen_4)
screen_4_cont_42.set_pos(369, 112)
screen_4_cont_42.set_size(868, 609)
screen_4_cont_42.add_flag(lv.obj.FLAG.HIDDEN)
screen_4_cont_42.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4_cont_42, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_cont_42.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_bg_opa(45, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_cont_42.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_4_calendar_1
screen_4_calendar_1 = lv.calendar(screen_4_cont_42)
screen_4_calendar_1.set_today_date(time.localtime()[0], time.localtime()[1], time.localtime()[2])
screen_4_calendar_1.set_showed_date(time.localtime()[0], time.localtime()[1])
screen_4_calendar_1_highlighted_days=[
lv.calendar_date_t({'year':2026, 'month':4, 'day':24})
]
screen_4_calendar_1.set_highlighted_dates(screen_4_calendar_1_highlighted_days, len(screen_4_calendar_1_highlighted_days))
screen_4_calendar_1_header = lv.calendar_header_arrow(screen_4_calendar_1)
screen_4_calendar_1.add_event_cb(lambda e: calendar_event_handler(e,screen_4_calendar_1), lv.EVENT.ALL, None)
screen_4_calendar_1.set_pos(22, 10)
screen_4_calendar_1.set_size(832, 581)
# Set style for screen_4_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_calendar_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_calendar_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_calendar_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_4_calendar_1_extra_header_main_default = lv.style_t()
style_screen_4_calendar_1_extra_header_main_default.init()
style_screen_4_calendar_1_extra_header_main_default.set_text_color(lv.color_hex(0xffffff))
style_screen_4_calendar_1_extra_header_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_4_calendar_1_extra_header_main_default.set_text_opa(255)
style_screen_4_calendar_1_extra_header_main_default.set_bg_opa(255)
style_screen_4_calendar_1_extra_header_main_default.set_bg_color(lv.color_hex(0x2195f6))
style_screen_4_calendar_1_extra_header_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_4_calendar_1_header.add_style(style_screen_4_calendar_1_extra_header_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_4_calendar_1, Part: lv.PART.ITEMS, State: lv.STATE.DEFAULT.
screen_4_calendar_1.get_btnmatrix().set_style_bg_opa(0, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_border_width(1, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_border_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_border_color(lv.color_hex(0xc0c0c0), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_text_color(lv.color_hex(0x0D3055), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_text_font(test_font("montserratMedium", 12), lv.PART.ITEMS|lv.STATE.DEFAULT)
screen_4_calendar_1.get_btnmatrix().set_style_text_opa(255, lv.PART.ITEMS|lv.STATE.DEFAULT)

def screen_4_calendar_1_extra_ctrl_day_names_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id < 7:
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)

screen_4_calendar_1.get_btnmatrix().add_event_cb(screen_4_calendar_1_extra_ctrl_day_names_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_4_calendar_1_extra_ctrl_highlight_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.CUSTOM_2):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 255
        dsc.rect_dsc.bg_color = lv.color_hex(0x2195f6)

screen_4_calendar_1.get_btnmatrix().add_event_cb(screen_4_calendar_1_extra_ctrl_highlight_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_4_calendar_1_extra_ctrl_today_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.CUSTOM_1):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0x0D3055)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 255
        dsc.rect_dsc.bg_color = lv.color_hex(0x01a2b1)
        dsc.rect_dsc.border_width = 1
        dsc.rect_dsc.border_color = lv.color_hex(0xc0c0c0)
        dsc.rect_dsc.border_opa = 255

screen_4_calendar_1.get_btnmatrix().add_event_cb(screen_4_calendar_1_extra_ctrl_today_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

def screen_4_calendar_1_extra_ctrl_other_month_draw_event_cb(e):
    obj = lv.btnmatrix.__cast__(e.get_target())
    dsc = lv.obj_draw_part_dsc_t.__cast__(e.get_param())
    if dsc.id >= 7 and obj.has_btn_ctrl(dsc.id, lv.btnmatrix.CTRL.DISABLED):
        if dsc.label_dsc: dsc.label_dsc.color = lv.color_hex(0xA9A2A2)
        if dsc.label_dsc: dsc.label_dsc.font = test_font("montserratMedium", 12)
        dsc.rect_dsc.bg_opa = 0

screen_4_calendar_1.get_btnmatrix().add_event_cb(screen_4_calendar_1_extra_ctrl_other_month_draw_event_cb, lv.EVENT.DRAW_PART_BEGIN, None)

screen_4.update_layout()
# Create screen_log_in
screen_log_in = lv.obj()
g_kb_screen_log_in = lv.keyboard(screen_log_in)
g_kb_screen_log_in.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_log_in), lv.EVENT.ALL, None)
g_kb_screen_log_in.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_log_in.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in.set_size(1280, 800)
screen_log_in.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_log_in, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in.set_style_bg_color(lv.color_hex(0x454055), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_log_in_cont_1
screen_log_in_cont_1 = lv.obj(screen_log_in)
screen_log_in_cont_1.set_pos(225, 105)
screen_log_in_cont_1.set_size(822, 564)
screen_log_in_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_log_in_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_log_in_ta_1
screen_log_in_ta_1 = lv.textarea(screen_log_in_cont_1)
screen_log_in_ta_1.set_text("16666666666")
screen_log_in_ta_1.set_placeholder_text("")
screen_log_in_ta_1.set_password_bullet("*")
screen_log_in_ta_1.set_password_mode(False)
screen_log_in_ta_1.set_one_line(False)
screen_log_in_ta_1.set_accepted_chars("")
screen_log_in_ta_1.set_max_length(32)
screen_log_in_ta_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_log_in), lv.EVENT.ALL, None)
screen_log_in_ta_1.set_pos(25, 33)
screen_log_in_ta_1.set_size(400, 70)
# Set style for screen_log_in_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_ta_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_log_in_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_log_in_ta_1.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_log_in_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_log_in_btn_1
screen_log_in_btn_1 = lv.btn(screen_log_in_cont_1)
screen_log_in_btn_1_label = lv.label(screen_log_in_btn_1)
screen_log_in_btn_1_label.set_text("发送验证码")
screen_log_in_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_log_in_btn_1_label.set_width(lv.pct(100))
screen_log_in_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_log_in_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_log_in_btn_1.set_pos(480, 32)
screen_log_in_btn_1.set_size(200, 70)
# Set style for screen_log_in_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_bg_color(lv.color_hex(0xa02020), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_log_in_cont_2
screen_log_in_cont_2 = lv.obj(screen_log_in)
screen_log_in_cont_2.set_pos(225, 105)
screen_log_in_cont_2.set_size(822, 564)
screen_log_in_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
screen_log_in_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_log_in_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_log_in_btn_2
screen_log_in_btn_2 = lv.btn(screen_log_in_cont_2)
screen_log_in_btn_2_label = lv.label(screen_log_in_btn_2)
screen_log_in_btn_2_label.set_text("36s后重新获取")
screen_log_in_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
screen_log_in_btn_2_label.set_width(lv.pct(100))
screen_log_in_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_log_in_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_log_in_btn_2.set_pos(480, 32)
screen_log_in_btn_2.set_size(200, 70)
# Set style for screen_log_in_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_bg_color(lv.color_hex(0xa02020), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_log_in_btn_3
screen_log_in_btn_3 = lv.btn(screen_log_in_cont_2)
screen_log_in_btn_3_label = lv.label(screen_log_in_btn_3)
screen_log_in_btn_3_label.set_text("登录")
screen_log_in_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_log_in_btn_3_label.set_width(lv.pct(100))
screen_log_in_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_log_in_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_log_in_btn_3.set_pos(490, 259)
screen_log_in_btn_3.set_size(200, 70)
# Set style for screen_log_in_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_bg_color(lv.color_hex(0xa02020), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_log_in_ta_2
screen_log_in_ta_2 = lv.textarea(screen_log_in_cont_2)
screen_log_in_ta_2.set_text("")
screen_log_in_ta_2.set_placeholder_text("请输入验证码")
screen_log_in_ta_2.set_password_bullet("*")
screen_log_in_ta_2.set_password_mode(False)
screen_log_in_ta_2.set_one_line(False)
screen_log_in_ta_2.set_accepted_chars("")
screen_log_in_ta_2.set_max_length(32)
screen_log_in_ta_2.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_log_in), lv.EVENT.ALL, None)
screen_log_in_ta_2.set_pos(25, 33)
screen_log_in_ta_2.set_size(400, 70)
# Set style for screen_log_in_ta_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_ta_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_log_in_ta_2, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_log_in_ta_2.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_log_in_ta_2.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_log_in_btn_4
screen_log_in_btn_4 = lv.btn(screen_log_in)
screen_log_in_btn_4_label = lv.label(screen_log_in_btn_4)
screen_log_in_btn_4_label.set_text("X  删除")
screen_log_in_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_log_in_btn_4_label.set_width(lv.pct(100))
screen_log_in_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_log_in_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_log_in_btn_4.set_pos(279, 635)
screen_log_in_btn_4.set_size(161, 50)
# Set style for screen_log_in_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_bg_color(lv.color_hex(0xa02020), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_log_in_btn_5
screen_log_in_btn_5 = lv.btn(screen_log_in)
screen_log_in_btn_5_label = lv.label(screen_log_in_btn_5)
screen_log_in_btn_5_label.set_text("⬅️ 清空")
screen_log_in_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_log_in_btn_5_label.set_width(lv.pct(100))
screen_log_in_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_log_in_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_log_in_btn_5.set_pos(469, 635)
screen_log_in_btn_5.set_size(161, 50)
# Set style for screen_log_in_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_log_in_btn_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_bg_color(lv.color_hex(0xa02020), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_log_in_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_log_in.update_layout()
# Create screen_2
screen_2 = lv.obj()
g_kb_screen_2 = lv.keyboard(screen_2)
g_kb_screen_2.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_2), lv.EVENT.ALL, None)
g_kb_screen_2.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_2.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2.set_size(1280, 800)
screen_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_2.add_flag(lv.obj.FLAG.CLICKABLE)
# Set style for screen_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_cont_1
screen_2_cont_1 = lv.obj(screen_2)
screen_2_cont_1.set_pos(2, 686)
screen_2_cont_1.set_size(1388, 107)
screen_2_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_2_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_2_img_12
screen_2_img_12 = lv.img(screen_2_cont_1)
screen_2_img_12.set_src("B:MicroPython/_123124_1280x103.bin")
screen_2_img_12.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_12.set_pivot(50,50)
screen_2_img_12.set_angle(0)
screen_2_img_12.set_pos(0, 9)
screen_2_img_12.set_size(1280, 103)
# Set style for screen_2_img_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_12.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_12.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_12.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_10
screen_2_img_10 = lv.img(screen_2_cont_1)
screen_2_img_10.set_src("B:MicroPython/_DIV1_alpha_80x112.bin")
screen_2_img_10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_10.set_pivot(50,50)
screen_2_img_10.set_angle(0)
screen_2_img_10.set_pos(87, 9)
screen_2_img_10.set_size(80, 112)
# Set style for screen_2_img_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_9
screen_2_img_9 = lv.img(screen_2_cont_1)
screen_2_img_9.set_src("B:MicroPython/_DIV5_alpha_80x112.bin")
screen_2_img_9.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_9.set_pivot(50,50)
screen_2_img_9.set_angle(0)
screen_2_img_9.set_pos(659, 9)
screen_2_img_9.set_size(80, 112)
# Set style for screen_2_img_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_9.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_9.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_8
screen_2_img_8 = lv.img(screen_2_cont_1)
screen_2_img_8.set_src("B:MicroPython/_DIV6_alpha_80x112.bin")
screen_2_img_8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_8.set_pivot(50,50)
screen_2_img_8.set_angle(0)
screen_2_img_8.set_pos(802, 9)
screen_2_img_8.set_size(80, 112)
# Set style for screen_2_img_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_7
screen_2_img_7 = lv.img(screen_2_cont_1)
screen_2_img_7.set_src("B:MicroPython/_DIV4_alpha_80x112.bin")
screen_2_img_7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_7.set_pivot(50,50)
screen_2_img_7.set_angle(0)
screen_2_img_7.set_pos(230, 9)
screen_2_img_7.set_size(80, 112)
# Set style for screen_2_img_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_6
screen_2_img_6 = lv.img(screen_2_cont_1)
screen_2_img_6.set_src("B:MicroPython/_DIV3_alpha_80x112.bin")
screen_2_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_6.set_pivot(50,50)
screen_2_img_6.set_angle(0)
screen_2_img_6.set_pos(373, 9)
screen_2_img_6.set_size(80, 112)
# Set style for screen_2_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_5
screen_2_img_5 = lv.img(screen_2_cont_1)
screen_2_img_5.set_src("B:MicroPython/_DIV7_alpha_80x112.bin")
screen_2_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_5.set_pivot(50,50)
screen_2_img_5.set_angle(0)
screen_2_img_5.set_pos(945, 9)
screen_2_img_5.set_size(80, 112)
# Set style for screen_2_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_4
screen_2_img_4 = lv.img(screen_2_cont_1)
screen_2_img_4.set_src("B:MicroPython/_DIV2_alpha_80x112.bin")
screen_2_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_4.set_pivot(50,50)
screen_2_img_4.set_angle(0)
screen_2_img_4.set_pos(516, 9)
screen_2_img_4.set_size(80, 112)
# Set style for screen_2_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_img_3
screen_2_img_3 = lv.img(screen_2_cont_1)
screen_2_img_3.set_src("B:MicroPython/_DIV8_alpha_80x112.bin")
screen_2_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_2_img_3.set_pivot(50,50)
screen_2_img_3.set_angle(0)
screen_2_img_3.set_pos(1088, 9)
screen_2_img_3.set_size(80, 112)
# Set style for screen_2_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_img_3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_img_3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_13
screen_2_btn_13 = lv.btn(screen_2_cont_1)
screen_2_btn_13_label = lv.label(screen_2_btn_13)
screen_2_btn_13_label.set_text("")
screen_2_btn_13_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_13_label.set_width(lv.pct(100))
screen_2_btn_13_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_13.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_13.set_pos(495, 10)
screen_2_btn_13.set_size(122, 98)
# Set style for screen_2_btn_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_13.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_13.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_12
screen_2_btn_12 = lv.btn(screen_2_cont_1)
screen_2_btn_12_label = lv.label(screen_2_btn_12)
screen_2_btn_12_label.set_text("")
screen_2_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_12_label.set_width(lv.pct(100))
screen_2_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_12.set_pos(638, 11)
screen_2_btn_12.set_size(126, 98)
# Set style for screen_2_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_10
screen_2_btn_10 = lv.btn(screen_2_cont_1)
screen_2_btn_10_label = lv.label(screen_2_btn_10)
screen_2_btn_10_label.set_text("")
screen_2_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_10_label.set_width(lv.pct(100))
screen_2_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_10.set_pos(349, 10)
screen_2_btn_10.set_size(122, 98)
# Set style for screen_2_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_9
screen_2_btn_9 = lv.btn(screen_2_cont_1)
screen_2_btn_9_label = lv.label(screen_2_btn_9)
screen_2_btn_9_label.set_text("")
screen_2_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_9_label.set_width(lv.pct(100))
screen_2_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_9.set_pos(1068, 9)
screen_2_btn_9.set_size(126, 98)
# Set style for screen_2_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_8
screen_2_btn_8 = lv.btn(screen_2_cont_1)
screen_2_btn_8_label = lv.label(screen_2_btn_8)
screen_2_btn_8_label.set_text("")
screen_2_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_8_label.set_width(lv.pct(100))
screen_2_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_8.set_pos(921, 10)
screen_2_btn_8.set_size(121, 98)
# Set style for screen_2_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_14
screen_2_btn_14 = lv.btn(screen_2_cont_1)
screen_2_btn_14_label = lv.label(screen_2_btn_14)
screen_2_btn_14_label.set_text("")
screen_2_btn_14_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_14_label.set_width(lv.pct(100))
screen_2_btn_14_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_14.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_14.set_pos(67, 17)
screen_2_btn_14.set_size(122, 98)
# Set style for screen_2_btn_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_14.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_14.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_15
screen_2_btn_15 = lv.btn(screen_2)
screen_2_btn_15_label = lv.label(screen_2_btn_15)
screen_2_btn_15_label.set_text("")
screen_2_btn_15_label.set_long_mode(lv.label.LONG.WRAP)
screen_2_btn_15_label.set_width(lv.pct(100))
screen_2_btn_15_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_15.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_15.set_pos(2, 0)
screen_2_btn_15.set_size(1275, 689)
# Set style for screen_2_btn_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_15.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_15.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_2.update_layout()
# Create screen_7
screen_7 = lv.obj()
g_kb_screen_7 = lv.keyboard(screen_7)
g_kb_screen_7.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_7), lv.EVENT.ALL, None)
g_kb_screen_7.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_7.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_size(1280, 800)
screen_7.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_style_bg_grad_dir(lv.GRAD_DIR.VER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_style_bg_grad_color(lv.color_hex(0x103b5f), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_style_bg_main_stop(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7.set_style_bg_grad_stop(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_1
screen_7_img_1 = lv.img(screen_7)
screen_7_img_1.set_src("B:MicroPython/_png11_1280x111.bin")
screen_7_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_1.set_pivot(50,50)
screen_7_img_1.set_angle(0)
screen_7_img_1.set_pos(0, -6)
screen_7_img_1.set_size(1280, 111)
# Set style for screen_7_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_7
screen_7_btn_7 = lv.btn(screen_7)
screen_7_btn_7_label = lv.label(screen_7_btn_7)
screen_7_btn_7_label.set_text("")
screen_7_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_7_label.set_width(lv.pct(100))
screen_7_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_7.set_pos(302, 0)
screen_7_btn_7.set_size(193, 128)
# Set style for screen_7_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_8
screen_7_btn_8 = lv.btn(screen_7)
screen_7_btn_8_label = lv.label(screen_7_btn_8)
screen_7_btn_8_label.set_text("")
screen_7_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_8_label.set_width(lv.pct(100))
screen_7_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_8.set_pos(503, 0)
screen_7_btn_8.set_size(140, 132)
# Set style for screen_7_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_9
screen_7_btn_9 = lv.btn(screen_7)
screen_7_btn_9_label = lv.label(screen_7_btn_9)
screen_7_btn_9_label.set_text("")
screen_7_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_9_label.set_width(lv.pct(100))
screen_7_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_9.set_pos(646, 0)
screen_7_btn_9.set_size(172, 129)
# Set style for screen_7_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_10
screen_7_btn_10 = lv.btn(screen_7)
screen_7_btn_10_label = lv.label(screen_7_btn_10)
screen_7_btn_10_label.set_text("")
screen_7_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_10_label.set_width(lv.pct(100))
screen_7_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_10.set_pos(818, 0)
screen_7_btn_10.set_size(163, 129)
# Set style for screen_7_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_cont_1
screen_7_cont_1 = lv.obj(screen_7)
screen_7_cont_1.set_pos(514, 81)
screen_7_cont_1.set_size(108, 6)
screen_7_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_7_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_radius(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_label_1
screen_7_label_1 = lv.label(screen_7)
screen_7_label_1.set_text("12:00")
screen_7_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_7_label_1.set_width(lv.pct(100))
screen_7_label_1.set_pos(1006, 46)
screen_7_label_1.set_size(107, 50)
# Set style for screen_7_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_cont_2
screen_7_cont_2 = lv.obj(screen_7)
screen_7_cont_2.set_pos(0, 111)
screen_7_cont_2.set_size(1276, 683)
screen_7_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
screen_7_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_7_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_bg_opa(21, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_7_img_11
screen_7_img_11 = lv.img(screen_7_cont_2)
screen_7_img_11.set_src("B:MicroPython/_btn10_alpha_261x165.bin")
screen_7_img_11.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_11.set_pivot(50,50)
screen_7_img_11.set_angle(0)
screen_7_img_11.set_pos(984, 477)
screen_7_img_11.set_size(261, 165)
# Set style for screen_7_img_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_11.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_11.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_11.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_10
screen_7_img_10 = lv.img(screen_7_cont_2)
screen_7_img_10.set_src("B:MicroPython/_btn9_alpha_261x165.bin")
screen_7_img_10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_10.set_pivot(50,50)
screen_7_img_10.set_angle(0)
screen_7_img_10.set_pos(671, 477)
screen_7_img_10.set_size(261, 165)
# Set style for screen_7_img_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_9
screen_7_img_9 = lv.img(screen_7_cont_2)
screen_7_img_9.set_src("B:MicroPython/_btn8_alpha_261x165.bin")
screen_7_img_9.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_9.set_pivot(50,50)
screen_7_img_9.set_angle(0)
screen_7_img_9.set_pos(358, 477)
screen_7_img_9.set_size(261, 165)
# Set style for screen_7_img_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_9.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_9.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_8
screen_7_img_8 = lv.img(screen_7_cont_2)
screen_7_img_8.set_src("B:MicroPython/_btn7_alpha_261x165.bin")
screen_7_img_8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_8.set_pivot(50,50)
screen_7_img_8.set_angle(0)
screen_7_img_8.set_pos(45, 477)
screen_7_img_8.set_size(261, 165)
# Set style for screen_7_img_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_7
screen_7_img_7 = lv.img(screen_7_cont_2)
screen_7_img_7.set_src("B:MicroPython/_btn4_alpha_373x167.bin")
screen_7_img_7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_7.set_pivot(50,50)
screen_7_img_7.set_angle(0)
screen_7_img_7.set_pos(45, 258)
screen_7_img_7.set_size(373, 167)
# Set style for screen_7_img_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_6
screen_7_img_6 = lv.img(screen_7_cont_2)
screen_7_img_6.set_src("B:MicroPython/_btn5_alpha_366x165.bin")
screen_7_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_6.set_pivot(50,50)
screen_7_img_6.set_angle(0)
screen_7_img_6.set_pos(465, 265)
screen_7_img_6.set_size(366, 165)
# Set style for screen_7_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_5
screen_7_img_5 = lv.img(screen_7_cont_2)
screen_7_img_5.set_src("B:MicroPython/_btn6_alpha_366x165.bin")
screen_7_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_5.set_pivot(50,50)
screen_7_img_5.set_angle(0)
screen_7_img_5.set_pos(879, 269)
screen_7_img_5.set_size(366, 165)
# Set style for screen_7_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_4
screen_7_img_4 = lv.img(screen_7_cont_2)
screen_7_img_4.set_src("B:MicroPython/_btn3_alpha_366x165.bin")
screen_7_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_4.set_pivot(50,50)
screen_7_img_4.set_angle(0)
screen_7_img_4.set_pos(879, 48)
screen_7_img_4.set_size(366, 165)
# Set style for screen_7_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_3
screen_7_img_3 = lv.img(screen_7_cont_2)
screen_7_img_3.set_src("B:MicroPython/_btn2_alpha_366x165.bin")
screen_7_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_3.set_pivot(50,50)
screen_7_img_3.set_angle(0)
screen_7_img_3.set_pos(465, 48)
screen_7_img_3.set_size(366, 165)
# Set style for screen_7_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_2
screen_7_img_2 = lv.img(screen_7_cont_2)
screen_7_img_2.set_src("B:MicroPython/_btn1_alpha_373x167.bin")
screen_7_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_2.set_pivot(50,50)
screen_7_img_2.set_angle(0)
screen_7_img_2.set_pos(45, 44)
screen_7_img_2.set_size(373, 167)
# Set style for screen_7_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_cont_3
screen_7_cont_3 = lv.obj(screen_7)
screen_7_cont_3.set_pos(8, 111)
screen_7_cont_3.set_size(1276, 683)
screen_7_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_7_cont_3.add_flag(lv.obj.FLAG.GESTURE_BUBBLE)
screen_7_cont_3.add_flag(lv.obj.FLAG.EVENT_BUBBLE)
# Set style for screen_7_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_bg_opa(21, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_7_img_12
screen_7_img_12 = lv.img(screen_7_cont_3)
screen_7_img_12.set_src("B:MicroPython/_btn11_alpha_594x353.bin")
screen_7_img_12.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_12.set_pivot(50,50)
screen_7_img_12.set_angle(0)
screen_7_img_12.set_pos(25, 47)
screen_7_img_12.set_size(594, 353)
screen_7_img_12.add_flag(lv.obj.FLAG.CLICKABLE)
# Set style for screen_7_img_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_12.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_12.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_12.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_13
screen_7_img_13 = lv.img(screen_7_cont_3)
screen_7_img_13.set_src("B:MicroPython/_btn12_alpha_275x165.bin")
screen_7_img_13.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_13.set_pivot(50,50)
screen_7_img_13.set_angle(0)
screen_7_img_13.set_pos(30, 436)
screen_7_img_13.set_size(275, 165)
# Set style for screen_7_img_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_13.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_13.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_13.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_14
screen_7_img_14 = lv.img(screen_7_cont_3)
screen_7_img_14.set_src("B:MicroPython/_btn13_alpha_275x165.bin")
screen_7_img_14.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_14.set_pivot(50,50)
screen_7_img_14.set_angle(0)
screen_7_img_14.set_pos(341, 436)
screen_7_img_14.set_size(275, 165)
# Set style for screen_7_img_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_14.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_14.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_14.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_16
screen_7_img_16 = lv.img(screen_7_cont_3)
screen_7_img_16.set_src("B:MicroPython/_btn19_alpha_275x165.bin")
screen_7_img_16.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_16.set_pivot(50,50)
screen_7_img_16.set_angle(0)
screen_7_img_16.set_pos(963, 436)
screen_7_img_16.set_size(275, 165)
# Set style for screen_7_img_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_16.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_16.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_16.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_15
screen_7_img_15 = lv.img(screen_7_cont_3)
screen_7_img_15.set_src("B:MicroPython/_btn16_alpha_275x165.bin")
screen_7_img_15.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_15.set_pivot(50,50)
screen_7_img_15.set_angle(0)
screen_7_img_15.set_pos(652, 436)
screen_7_img_15.set_size(275, 165)
# Set style for screen_7_img_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_15.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_15.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_15.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_18
screen_7_img_18 = lv.img(screen_7_cont_3)
screen_7_img_18.set_src("B:MicroPython/_btn18_alpha_275x165.bin")
screen_7_img_18.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_18.set_pivot(50,50)
screen_7_img_18.set_angle(0)
screen_7_img_18.set_pos(964, 244)
screen_7_img_18.set_size(275, 165)
# Set style for screen_7_img_18, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_18.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_18.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_18.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_17
screen_7_img_17 = lv.img(screen_7_cont_3)
screen_7_img_17.set_src("B:MicroPython/_btn15_alpha_275x165.bin")
screen_7_img_17.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_17.set_pivot(50,50)
screen_7_img_17.set_angle(0)
screen_7_img_17.set_pos(652, 244)
screen_7_img_17.set_size(275, 165)
# Set style for screen_7_img_17, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_17.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_17.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_17.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_20
screen_7_img_20 = lv.img(screen_7_cont_3)
screen_7_img_20.set_src("B:MicroPython/_btn17_alpha_275x165.bin")
screen_7_img_20.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_20.set_pivot(50,50)
screen_7_img_20.set_angle(0)
screen_7_img_20.set_pos(964, 47)
screen_7_img_20.set_size(275, 165)
# Set style for screen_7_img_20, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_20.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_20.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_20.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_19
screen_7_img_19 = lv.img(screen_7_cont_3)
screen_7_img_19.set_src("B:MicroPython/_btn14_alpha_275x165.bin")
screen_7_img_19.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_19.set_pivot(50,50)
screen_7_img_19.set_angle(0)
screen_7_img_19.set_pos(653, 47)
screen_7_img_19.set_size(275, 165)
# Set style for screen_7_img_19, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_19.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_19.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_19.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_label_2
screen_7_label_2 = lv.label(screen_7_cont_3)
screen_7_label_2.set_text("分类点歌")
screen_7_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_7_label_2.set_width(lv.pct(100))
screen_7_label_2.set_pos(731, 157)
screen_7_label_2.set_size(152, 50)
# Set style for screen_7_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_12
screen_7_btn_12 = lv.btn(screen_7_cont_3)
screen_7_btn_12_label = lv.label(screen_7_btn_12)
screen_7_btn_12_label.set_text("")
screen_7_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_12_label.set_width(lv.pct(100))
screen_7_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_12.set_pos(25, 33)
screen_7_btn_12.set_size(593, 369)
# Set style for screen_7_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_cont_4
screen_7_cont_4 = lv.obj(screen_7)
screen_7_cont_4.set_pos(0, 111)
screen_7_cont_4.set_size(1276, 683)
screen_7_cont_4.add_flag(lv.obj.FLAG.HIDDEN)
screen_7_cont_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_7_cont_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_cont_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_7_img_29
screen_7_img_29 = lv.img(screen_7_cont_4)
screen_7_img_29.set_src("B:MicroPython/_btn21_alpha_437x257.bin")
screen_7_img_29.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_29.set_pivot(50,50)
screen_7_img_29.set_angle(0)
screen_7_img_29.set_pos(49, 55)
screen_7_img_29.set_size(437, 257)
# Set style for screen_7_img_29, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_29.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_29.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_29.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_21
screen_7_img_21 = lv.img(screen_7_cont_4)
screen_7_img_21.set_src("B:MicroPython/_btn25_alpha_193x257.bin")
screen_7_img_21.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_21.set_pivot(50,50)
screen_7_img_21.set_angle(0)
screen_7_img_21.set_pos(48, 360)
screen_7_img_21.set_size(193, 257)
# Set style for screen_7_img_21, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_21.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_21.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_21.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_30
screen_7_img_30 = lv.img(screen_7_cont_4)
screen_7_img_30.set_src("B:MicroPython/_btn26_alpha_193x257.bin")
screen_7_img_30.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_30.set_pivot(50,50)
screen_7_img_30.set_angle(0)
screen_7_img_30.set_pos(295, 360)
screen_7_img_30.set_size(193, 257)
# Set style for screen_7_img_30, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_30.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_30.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_30.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_31
screen_7_img_31 = lv.img(screen_7_cont_4)
screen_7_img_31.set_src("B:MicroPython/_btn27_alpha_430x257.bin")
screen_7_img_31.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_31.set_pivot(50,50)
screen_7_img_31.set_angle(0)
screen_7_img_31.set_pos(542, 360)
screen_7_img_31.set_size(430, 257)
# Set style for screen_7_img_31, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_31.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_31.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_31.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_33
screen_7_img_33 = lv.img(screen_7_cont_4)
screen_7_img_33.set_src("B:MicroPython/_btn28_alpha_206x257.bin")
screen_7_img_33.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_33.set_pivot(50,50)
screen_7_img_33.set_angle(0)
screen_7_img_33.set_pos(1023, 361)
screen_7_img_33.set_size(206, 257)
# Set style for screen_7_img_33, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_33.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_33.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_33.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_34
screen_7_img_34 = lv.img(screen_7_cont_4)
screen_7_img_34.set_src("B:MicroPython/_btn22_alpha_209x257.bin")
screen_7_img_34.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_34.set_pivot(50,50)
screen_7_img_34.set_angle(0)
screen_7_img_34.set_pos(532, 54)
screen_7_img_34.set_size(209, 257)
# Set style for screen_7_img_34, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_34.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_34.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_34.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_35
screen_7_img_35 = lv.img(screen_7_cont_4)
screen_7_img_35.set_src("B:MicroPython/_btn24_alpha_206x257.bin")
screen_7_img_35.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_35.set_pivot(50,50)
screen_7_img_35.set_angle(0)
screen_7_img_35.set_pos(1024, 55)
screen_7_img_35.set_size(206, 257)
# Set style for screen_7_img_35, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_35.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_35.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_35.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_img_36
screen_7_img_36 = lv.img(screen_7_cont_4)
screen_7_img_36.set_src("B:MicroPython/_btn23_alpha_209x257.bin")
screen_7_img_36.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_36.set_pivot(50,50)
screen_7_img_36.set_angle(0)
screen_7_img_36.set_pos(773, 56)
screen_7_img_36.set_size(209, 257)
# Set style for screen_7_img_36, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_36.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_36.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_36.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_cont_5
screen_7_cont_5 = lv.obj(screen_7)
screen_7_cont_5.set_pos(0, 111)
screen_7_cont_5.set_size(1276, 683)
screen_7_cont_5.add_flag(lv.obj.FLAG.HIDDEN)
screen_7_cont_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_7_cont_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_cont_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_cont_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_7_img_44
screen_7_img_44 = lv.img(screen_7_cont_5)
screen_7_img_44.set_src("B:MicroPython/_asfsdgbv_1250x610.bin")
screen_7_img_44.add_flag(lv.obj.FLAG.CLICKABLE)
screen_7_img_44.set_pivot(50,50)
screen_7_img_44.set_angle(0)
screen_7_img_44.set_pos(16, 33)
screen_7_img_44.set_size(1250, 610)
# Set style for screen_7_img_44, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_img_44.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_44.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_img_44.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_7_btn_11
screen_7_btn_11 = lv.btn(screen_7)
screen_7_btn_11_label = lv.label(screen_7_btn_11)
screen_7_btn_11_label.set_text("")
screen_7_btn_11_label.set_long_mode(lv.label.LONG.WRAP)
screen_7_btn_11_label.set_width(lv.pct(100))
screen_7_btn_11_label.align(lv.ALIGN.CENTER, 0, 0)
screen_7_btn_11.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_7_btn_11.set_pos(1182, 8)
screen_7_btn_11.set_size(91, 94)
# Set style for screen_7_btn_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_7_btn_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_7_btn_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_7.update_layout()
# Create screen_8
screen_8 = lv.obj()
g_kb_screen_8 = lv.keyboard(screen_8)
g_kb_screen_8.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_8), lv.EVENT.ALL, None)
g_kb_screen_8.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_8.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8.set_size(1280, 800)
screen_8.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_1
screen_8_img_1 = lv.img(screen_8)
screen_8_img_1.set_src("B:MicroPython/_lsls_1280x111.bin")
screen_8_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_1.set_pivot(50,50)
screen_8_img_1.set_angle(0)
screen_8_img_1.set_pos(1, 0)
screen_8_img_1.set_size(1280, 111)
# Set style for screen_8_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_7
screen_8_btn_7 = lv.btn(screen_8)
screen_8_btn_7_label = lv.label(screen_8_btn_7)
screen_8_btn_7_label.set_text("")
screen_8_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_7_label.set_width(lv.pct(100))
screen_8_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_7.set_pos(309, 0)
screen_8_btn_7.set_size(162, 117)
# Set style for screen_8_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_8
screen_8_btn_8 = lv.btn(screen_8)
screen_8_btn_8_label = lv.label(screen_8_btn_8)
screen_8_btn_8_label.set_text("")
screen_8_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_8_label.set_width(lv.pct(100))
screen_8_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_8.set_pos(471, 0)
screen_8_btn_8.set_size(141, 120)
# Set style for screen_8_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_9
screen_8_btn_9 = lv.btn(screen_8)
screen_8_btn_9_label = lv.label(screen_8_btn_9)
screen_8_btn_9_label.set_text("")
screen_8_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_9_label.set_width(lv.pct(100))
screen_8_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_9.set_pos(615, 0)
screen_8_btn_9.set_size(166, 126)
# Set style for screen_8_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_10
screen_8_btn_10 = lv.btn(screen_8)
screen_8_btn_10_label = lv.label(screen_8_btn_10)
screen_8_btn_10_label.set_text("")
screen_8_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_10_label.set_width(lv.pct(100))
screen_8_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_10.set_pos(781, 8)
screen_8_btn_10.set_size(163, 116)
# Set style for screen_8_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_1
screen_8_cont_1 = lv.obj(screen_8)
screen_8_cont_1.set_pos(344, 76)
screen_8_cont_1.set_size(108, 6)
screen_8_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_radius(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_11
screen_8_btn_11 = lv.btn(screen_8)
screen_8_btn_11_label = lv.label(screen_8_btn_11)
screen_8_btn_11_label.set_text("")
screen_8_btn_11_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_11_label.set_width(lv.pct(100))
screen_8_btn_11_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_11.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_11.set_pos(1182, 8)
screen_8_btn_11.set_size(91, 94)
# Set style for screen_8_btn_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_6
screen_8_cont_6 = lv.obj(screen_8)
screen_8_cont_6.set_pos(20, 126)
screen_8_cont_6.set_size(1301, 528)
screen_8_cont_6.add_flag(lv.obj.FLAG.HIDDEN)
screen_8_cont_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_6.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_border_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_border_color(lv.color_hex(0xff0027), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_img_97
screen_8_img_97 = lv.img(screen_8_cont_6)
screen_8_img_97.set_src("B:MicroPython/_esgsrg333_alpha_581x162.bin")
screen_8_img_97.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_97.set_pivot(50,50)
screen_8_img_97.set_angle(0)
screen_8_img_97.set_pos(311, 354)
screen_8_img_97.set_size(581, 162)
# Set style for screen_8_img_97, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_97.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_97.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_97.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_96
screen_8_img_96 = lv.img(screen_8_cont_6)
screen_8_img_96.set_src("B:MicroPython/_esgsrg222_alpha_291x506.bin")
screen_8_img_96.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_96.set_pivot(50,50)
screen_8_img_96.set_angle(0)
screen_8_img_96.set_pos(905, 11)
screen_8_img_96.set_size(291, 506)
# Set style for screen_8_img_96, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_96.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_96.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_96.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_80
screen_8_img_80 = lv.img(screen_8_cont_6)
screen_8_img_80.set_src("B:MicroPython/_esgsrg111_alpha_291x506.bin")
screen_8_img_80.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_80.set_pivot(50,50)
screen_8_img_80.set_angle(0)
screen_8_img_80.set_pos(8, 7)
screen_8_img_80.set_size(291, 506)
# Set style for screen_8_img_80, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_80.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_80.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_80.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_19
screen_8_btn_19 = lv.btn(screen_8_cont_6)
screen_8_btn_19_label = lv.label(screen_8_btn_19)
screen_8_btn_19_label.set_text("")
screen_8_btn_19_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_19_label.set_width(lv.pct(100))
screen_8_btn_19_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_19.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_19.set_pos(917, 16)
screen_8_btn_19.set_size(273, 147)
# Set style for screen_8_btn_19, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_19.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_19.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_18
screen_8_btn_18 = lv.btn(screen_8_cont_6)
screen_8_btn_18_label = lv.label(screen_8_btn_18)
screen_8_btn_18_label.set_text("")
screen_8_btn_18_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_18_label.set_width(lv.pct(100))
screen_8_btn_18_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_18.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_18.set_pos(920, 179)
screen_8_btn_18.set_size(273, 147)
# Set style for screen_8_btn_18, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_18.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_18.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_17
screen_8_btn_17 = lv.btn(screen_8_cont_6)
screen_8_btn_17_label = lv.label(screen_8_btn_17)
screen_8_btn_17_label.set_text("")
screen_8_btn_17_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_17_label.set_width(lv.pct(100))
screen_8_btn_17_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_17.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_17.set_pos(920, 367)
screen_8_btn_17.set_size(273, 147)
# Set style for screen_8_btn_17, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_17.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_17.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_16
screen_8_btn_16 = lv.btn(screen_8_cont_6)
screen_8_btn_16_label = lv.label(screen_8_btn_16)
screen_8_btn_16_label.set_text("")
screen_8_btn_16_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_16_label.set_width(lv.pct(100))
screen_8_btn_16_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_16.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_16.set_pos(619, 367)
screen_8_btn_16.set_size(273, 147)
# Set style for screen_8_btn_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_16.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_16.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_15
screen_8_btn_15 = lv.btn(screen_8_cont_6)
screen_8_btn_15_label = lv.label(screen_8_btn_15)
screen_8_btn_15_label.set_text("")
screen_8_btn_15_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_15_label.set_width(lv.pct(100))
screen_8_btn_15_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_15.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_15.set_pos(318, 367)
screen_8_btn_15.set_size(273, 147)
# Set style for screen_8_btn_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_15.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_15.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_14
screen_8_btn_14 = lv.btn(screen_8_cont_6)
screen_8_btn_14_label = lv.label(screen_8_btn_14)
screen_8_btn_14_label.set_text("")
screen_8_btn_14_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_14_label.set_width(lv.pct(100))
screen_8_btn_14_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_14.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_14.set_pos(16, 371)
screen_8_btn_14.set_size(273, 147)
# Set style for screen_8_btn_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_14.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_14.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_13
screen_8_btn_13 = lv.btn(screen_8_cont_6)
screen_8_btn_13_label = lv.label(screen_8_btn_13)
screen_8_btn_13_label.set_text("")
screen_8_btn_13_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_13_label.set_width(lv.pct(100))
screen_8_btn_13_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_13.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_13.set_pos(16, 182)
screen_8_btn_13.set_size(273, 147)
# Set style for screen_8_btn_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_13.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_13.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_12
screen_8_btn_12 = lv.btn(screen_8_cont_6)
screen_8_btn_12_label = lv.label(screen_8_btn_12)
screen_8_btn_12_label.set_text("")
screen_8_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_12_label.set_width(lv.pct(100))
screen_8_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_12.set_pos(20, 9)
screen_8_btn_12.set_size(273, 147)
# Set style for screen_8_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_30
screen_8_btn_30 = lv.btn(screen_8_cont_6)
screen_8_btn_30_label = lv.label(screen_8_btn_30)
screen_8_btn_30_label.set_text("")
screen_8_btn_30_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_30_label.set_width(lv.pct(100))
screen_8_btn_30_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_30.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_30.set_pos(307, 3)
screen_8_btn_30.set_size(599, 346)
# Set style for screen_8_btn_30, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_30.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_30.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_7
screen_8_cont_7 = lv.obj(screen_8)
screen_8_cont_7.set_pos(20, 129)
screen_8_cont_7.set_size(1301, 528)
screen_8_cont_7.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_7.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_border_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_border_color(lv.color_hex(0xff0027), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_img_82
screen_8_img_82 = lv.img(screen_8_cont_7)
screen_8_img_82.set_src("B:MicroPython/_esgsrg1_alpha_296x321.bin")
screen_8_img_82.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_82.set_pivot(50,50)
screen_8_img_82.set_angle(0)
screen_8_img_82.set_pos(7, 2)
screen_8_img_82.set_size(296, 321)
# Set style for screen_8_img_82, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_82.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_82.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_82.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_95
screen_8_img_95 = lv.img(screen_8_cont_7)
screen_8_img_95.set_src("B:MicroPython/_esgsrg2_alpha_276x158.bin")
screen_8_img_95.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_95.set_pivot(50,50)
screen_8_img_95.set_angle(0)
screen_8_img_95.set_pos(16, 348)
screen_8_img_95.set_size(276, 158)
# Set style for screen_8_img_95, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_95.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_95.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_95.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_24
screen_8_btn_24 = lv.btn(screen_8_cont_7)
screen_8_btn_24_label = lv.label(screen_8_btn_24)
screen_8_btn_24_label.set_text("")
screen_8_btn_24_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_24_label.set_width(lv.pct(100))
screen_8_btn_24_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_24.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_24.set_pos(15, 168)
screen_8_btn_24.set_size(273, 147)
# Set style for screen_8_btn_24, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_24.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_24.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_23
screen_8_btn_23 = lv.btn(screen_8_cont_7)
screen_8_btn_23_label = lv.label(screen_8_btn_23)
screen_8_btn_23_label.set_text("")
screen_8_btn_23_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_23_label.set_width(lv.pct(100))
screen_8_btn_23_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_23.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_23.set_pos(15, 9)
screen_8_btn_23.set_size(273, 147)
# Set style for screen_8_btn_23, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_23.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_23.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_28
screen_8_btn_28 = lv.btn(screen_8_cont_7)
screen_8_btn_28_label = lv.label(screen_8_btn_28)
screen_8_btn_28_label.set_text("")
screen_8_btn_28_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_28_label.set_width(lv.pct(100))
screen_8_btn_28_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_28.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_28.set_pos(16, 357)
screen_8_btn_28.set_size(273, 147)
# Set style for screen_8_btn_28, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_28.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_28.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_8
screen_8_cont_8 = lv.obj(screen_8)
screen_8_cont_8.set_pos(0, 100)
screen_8_cont_8.set_size(1309, 594)
screen_8_cont_8.add_flag(lv.obj.FLAG.HIDDEN)
screen_8_cont_8.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_8.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_border_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_border_color(lv.color_hex(0xff0027), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_bg_color(lv.color_hex(0x282828), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_img_83
screen_8_img_83 = lv.img(screen_8_cont_8)
screen_8_img_83.set_src("B:MicroPython/_wfafas_alpha_953x103.bin")
screen_8_img_83.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_83.set_pivot(50,50)
screen_8_img_83.set_angle(0)
screen_8_img_83.set_pos(138, 221)
screen_8_img_83.set_size(953, 103)
# Set style for screen_8_img_83, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_83.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_83.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_83.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_9
screen_8_cont_9 = lv.obj(screen_8)
screen_8_cont_9.set_pos(0, 100)
screen_8_cont_9.set_size(1313, 597)
screen_8_cont_9.add_flag(lv.obj.FLAG.HIDDEN)
screen_8_cont_9.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_9.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_border_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_border_color(lv.color_hex(0xff0027), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_bg_color(lv.color_hex(0x282828), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_img_84
screen_8_img_84 = lv.img(screen_8_cont_9)
screen_8_img_84.set_src("B:MicroPython/_wfafas_alpha_953x103.bin")
screen_8_img_84.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_84.set_pivot(50,50)
screen_8_img_84.set_angle(0)
screen_8_img_84.set_pos(138, 221)
screen_8_img_84.set_size(953, 103)
# Set style for screen_8_img_84, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_84.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_84.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_84.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_10
screen_8_cont_10 = lv.obj(screen_8)
screen_8_cont_10.set_pos(1, 687)
screen_8_cont_10.set_size(1388, 107)
screen_8_cont_10.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_img_85
screen_8_img_85 = lv.img(screen_8_cont_10)
screen_8_img_85.set_src("B:MicroPython/_123124_1280x103.bin")
screen_8_img_85.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_85.set_pivot(50,50)
screen_8_img_85.set_angle(0)
screen_8_img_85.set_pos(0, 9)
screen_8_img_85.set_size(1280, 103)
# Set style for screen_8_img_85, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_85.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_85.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_85.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_86
screen_8_img_86 = lv.img(screen_8_cont_10)
screen_8_img_86.set_src("B:MicroPython/_DIV_alpha_80x112.bin")
screen_8_img_86.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_86.set_pivot(50,50)
screen_8_img_86.set_angle(0)
screen_8_img_86.set_pos(71, 7)
screen_8_img_86.set_size(80, 112)
# Set style for screen_8_img_86, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_86.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_86.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_86.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_87
screen_8_img_87 = lv.img(screen_8_cont_10)
screen_8_img_87.set_src("B:MicroPython/_DIV1_alpha_80x112.bin")
screen_8_img_87.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_87.set_pivot(50,50)
screen_8_img_87.set_angle(0)
screen_8_img_87.set_pos(202, 7)
screen_8_img_87.set_size(80, 112)
# Set style for screen_8_img_87, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_87.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_87.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_87.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_88
screen_8_img_88 = lv.img(screen_8_cont_10)
screen_8_img_88.set_src("B:MicroPython/_DIV5_alpha_80x112.bin")
screen_8_img_88.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_88.set_pivot(50,50)
screen_8_img_88.set_angle(0)
screen_8_img_88.set_pos(726, 7)
screen_8_img_88.set_size(80, 112)
# Set style for screen_8_img_88, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_88.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_88.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_88.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_91
screen_8_img_91 = lv.img(screen_8_cont_10)
screen_8_img_91.set_src("B:MicroPython/_DIV6_alpha_80x112.bin")
screen_8_img_91.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_91.set_pivot(50,50)
screen_8_img_91.set_angle(0)
screen_8_img_91.set_pos(857, 7)
screen_8_img_91.set_size(80, 112)
# Set style for screen_8_img_91, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_91.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_91.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_91.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_90
screen_8_img_90 = lv.img(screen_8_cont_10)
screen_8_img_90.set_src("B:MicroPython/_DIV4_alpha_80x112.bin")
screen_8_img_90.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_90.set_pivot(50,50)
screen_8_img_90.set_angle(0)
screen_8_img_90.set_pos(333, 7)
screen_8_img_90.set_size(80, 112)
# Set style for screen_8_img_90, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_90.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_90.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_90.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_89
screen_8_img_89 = lv.img(screen_8_cont_10)
screen_8_img_89.set_src("B:MicroPython/_DIV3_alpha_80x112.bin")
screen_8_img_89.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_89.set_pivot(50,50)
screen_8_img_89.set_angle(0)
screen_8_img_89.set_pos(464, 7)
screen_8_img_89.set_size(80, 112)
# Set style for screen_8_img_89, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_89.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_89.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_89.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_94
screen_8_img_94 = lv.img(screen_8_cont_10)
screen_8_img_94.set_src("B:MicroPython/_DIV7_alpha_80x112.bin")
screen_8_img_94.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_94.set_pivot(50,50)
screen_8_img_94.set_angle(0)
screen_8_img_94.set_pos(988, 7)
screen_8_img_94.set_size(80, 112)
# Set style for screen_8_img_94, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_94.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_94.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_94.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_93
screen_8_img_93 = lv.img(screen_8_cont_10)
screen_8_img_93.set_src("B:MicroPython/_DIV2_alpha_80x112.bin")
screen_8_img_93.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_93.set_pivot(50,50)
screen_8_img_93.set_angle(0)
screen_8_img_93.set_pos(595, 7)
screen_8_img_93.set_size(80, 112)
# Set style for screen_8_img_93, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_93.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_93.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_93.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_img_92
screen_8_img_92 = lv.img(screen_8_cont_10)
screen_8_img_92.set_src("B:MicroPython/_DIV8_alpha_80x112.bin")
screen_8_img_92.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_img_92.set_pivot(50,50)
screen_8_img_92.set_angle(0)
screen_8_img_92.set_pos(1119, 7)
screen_8_img_92.set_size(80, 112)
# Set style for screen_8_img_92, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_img_92.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_92.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_img_92.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_20
screen_8_btn_20 = lv.btn(screen_8_cont_10)
screen_8_btn_20_label = lv.label(screen_8_btn_20)
screen_8_btn_20_label.set_text("")
screen_8_btn_20_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_20_label.set_width(lv.pct(100))
screen_8_btn_20_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_20.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_20.set_pos(577, 10)
screen_8_btn_20.set_size(122, 98)
# Set style for screen_8_btn_20, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_20.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_20.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_21
screen_8_btn_21 = lv.btn(screen_8_cont_10)
screen_8_btn_21_label = lv.label(screen_8_btn_21)
screen_8_btn_21_label.set_text("")
screen_8_btn_21_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_21_label.set_width(lv.pct(100))
screen_8_btn_21_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_21.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_21.set_pos(706, 10)
screen_8_btn_21.set_size(126, 98)
# Set style for screen_8_btn_21, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_21.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_21.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_22
screen_8_btn_22 = lv.btn(screen_8_cont_10)
screen_8_btn_22_label = lv.label(screen_8_btn_22)
screen_8_btn_22_label.set_text("")
screen_8_btn_22_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_22_label.set_width(lv.pct(100))
screen_8_btn_22_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_22.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_22.set_pos(44, 12)
screen_8_btn_22.set_size(124, 98)
# Set style for screen_8_btn_22, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_22.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_22.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_25
screen_8_btn_25 = lv.btn(screen_8_cont_10)
screen_8_btn_25_label = lv.label(screen_8_btn_25)
screen_8_btn_25_label.set_text("")
screen_8_btn_25_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_25_label.set_width(lv.pct(100))
screen_8_btn_25_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_25.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_25.set_pos(442, 13)
screen_8_btn_25.set_size(122, 98)
# Set style for screen_8_btn_25, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_25.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_25.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_26
screen_8_btn_26 = lv.btn(screen_8_cont_10)
screen_8_btn_26_label = lv.label(screen_8_btn_26)
screen_8_btn_26_label.set_text("")
screen_8_btn_26_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_26_label.set_width(lv.pct(100))
screen_8_btn_26_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_26.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_26.set_pos(1096, 13)
screen_8_btn_26.set_size(126, 98)
# Set style for screen_8_btn_26, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_26.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_26.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_27
screen_8_btn_27 = lv.btn(screen_8_cont_10)
screen_8_btn_27_label = lv.label(screen_8_btn_27)
screen_8_btn_27_label.set_text("")
screen_8_btn_27_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_27_label.set_width(lv.pct(100))
screen_8_btn_27_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_27.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_27.set_pos(966, 13)
screen_8_btn_27.set_size(121, 98)
# Set style for screen_8_btn_27, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_27.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_27.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_btn_29
screen_8_btn_29 = lv.btn(screen_8_cont_10)
screen_8_btn_29_label = lv.label(screen_8_btn_29)
screen_8_btn_29_label.set_text("")
screen_8_btn_29_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_btn_29_label.set_width(lv.pct(100))
screen_8_btn_29_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_btn_29.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_btn_29.set_pos(179, 9)
screen_8_btn_29.set_size(122, 98)
# Set style for screen_8_btn_29, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_btn_29.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_btn_29.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_cont_11
screen_8_cont_11 = lv.obj(screen_8)
screen_8_cont_11.set_pos(995, 321)
screen_8_cont_11.set_size(72, 366)
screen_8_cont_11.add_flag(lv.obj.FLAG.HIDDEN)
screen_8_cont_11.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_cont_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_cont_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_radius(70, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_cont_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_slider_1
screen_8_slider_1 = lv.slider(screen_8_cont_11)
screen_8_slider_1.set_range(0, 100)
screen_8_slider_1.set_mode(lv.slider.MODE.NORMAL)
screen_8_slider_1.set_value(50, lv.ANIM.OFF)
screen_8_slider_1.set_pos(26, 36)
screen_8_slider_1.set_size(17, 295)
# Set style for screen_8_slider_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_slider_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_color(lv.color_hex(0x626262), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_8_slider_1, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_8_slider_1.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_color(lv.color_hex(0xfff700), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_8_slider_1, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_8_slider_1.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_color(lv.color_hex(0xffe200), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_8_slider_1.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

screen_8.update_layout()
# Create screen_9
screen_9 = lv.obj()
g_kb_screen_9 = lv.keyboard(screen_9)
g_kb_screen_9.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_9), lv.EVENT.ALL, None)
g_kb_screen_9.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_9.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9.set_size(1280, 800)
screen_9.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9.set_style_bg_color(lv.color_hex(0x170d2a), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_9_btn_5
screen_9_btn_5 = lv.btn(screen_9)
screen_9_btn_5_label = lv.label(screen_9_btn_5)
screen_9_btn_5_label.set_text("退出")
screen_9_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_9_btn_5_label.set_width(lv.pct(100))
screen_9_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_9_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_9_btn_5.set_pos(14, 8)
screen_9_btn_5.set_size(136, 58)
# Set style for screen_9_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9_btn_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_9_img_1
screen_9_img_1 = lv.img(screen_9)
screen_9_img_1.set_src("B:MicroPython/_speake_412x625.bin")
screen_9_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_9_img_1.set_pivot(50,50)
screen_9_img_1.set_angle(0)
screen_9_img_1.set_pos(20, 123)
screen_9_img_1.set_size(412, 625)
# Set style for screen_9_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_9_img_2
screen_9_img_2 = lv.img(screen_9)
screen_9_img_2.set_src("B:MicroPython/_speaker1png_517x65.bin")
screen_9_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_9_img_2.set_pivot(50,50)
screen_9_img_2.set_angle(0)
screen_9_img_2.set_pos(700, 28)
screen_9_img_2.set_size(517, 65)
# Set style for screen_9_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_9_cont_1
screen_9_cont_1 = lv.obj(screen_9)
screen_9_cont_1.set_pos(428, 129)
screen_9_cont_1.set_size(893, 677)
screen_9_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_9_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_9_btn_6
screen_9_btn_6 = lv.btn(screen_9)
screen_9_btn_6_label = lv.label(screen_9_btn_6)
screen_9_btn_6_label.set_text("")
screen_9_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_9_btn_6_label.set_width(lv.pct(100))
screen_9_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_9_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_9_btn_6.set_pos(30, 192)
screen_9_btn_6.set_size(379, 549)
# Set style for screen_9_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_9_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_9_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_9.update_layout()
# Create screen_3
screen_3 = lv.obj()
g_kb_screen_3 = lv.keyboard(screen_3)
g_kb_screen_3.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_3), lv.EVENT.ALL, None)
g_kb_screen_3.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_3.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_size(1280, 800)
screen_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_3.add_flag(lv.obj.FLAG.CLICKABLE)
# Set style for screen_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3.set_style_bg_opa(209, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_style_bg_color(lv.color_hex(0xff6500), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_7
screen_3_cont_7 = lv.obj(screen_3)
screen_3_cont_7.set_pos(2, 686)
screen_3_cont_7.set_size(1388, 107)
screen_3_cont_7.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_img_11
screen_3_img_11 = lv.img(screen_3_cont_7)
screen_3_img_11.set_src("B:MicroPython/_123124_1280x103.bin")
screen_3_img_11.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_11.set_pivot(50,50)
screen_3_img_11.set_angle(0)
screen_3_img_11.set_pos(0, 9)
screen_3_img_11.set_size(1280, 103)
# Set style for screen_3_img_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_11.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_11.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_11.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_10
screen_3_img_10 = lv.img(screen_3_cont_7)
screen_3_img_10.set_src("B:MicroPython/_DIV1_alpha_80x112.bin")
screen_3_img_10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_10.set_pivot(50,50)
screen_3_img_10.set_angle(0)
screen_3_img_10.set_pos(87, 9)
screen_3_img_10.set_size(80, 112)
# Set style for screen_3_img_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_9
screen_3_img_9 = lv.img(screen_3_cont_7)
screen_3_img_9.set_src("B:MicroPython/_DIV5_alpha_80x112.bin")
screen_3_img_9.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_9.set_pivot(50,50)
screen_3_img_9.set_angle(0)
screen_3_img_9.set_pos(659, 9)
screen_3_img_9.set_size(80, 112)
# Set style for screen_3_img_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_9.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_9.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_8
screen_3_img_8 = lv.img(screen_3_cont_7)
screen_3_img_8.set_src("B:MicroPython/_DIV6_alpha_80x112.bin")
screen_3_img_8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_8.set_pivot(50,50)
screen_3_img_8.set_angle(0)
screen_3_img_8.set_pos(802, 9)
screen_3_img_8.set_size(80, 112)
# Set style for screen_3_img_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_7
screen_3_img_7 = lv.img(screen_3_cont_7)
screen_3_img_7.set_src("B:MicroPython/_DIV4_alpha_80x112.bin")
screen_3_img_7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_7.set_pivot(50,50)
screen_3_img_7.set_angle(0)
screen_3_img_7.set_pos(230, 9)
screen_3_img_7.set_size(80, 112)
# Set style for screen_3_img_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_6
screen_3_img_6 = lv.img(screen_3_cont_7)
screen_3_img_6.set_src("B:MicroPython/_DIV3_alpha_80x112.bin")
screen_3_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_6.set_pivot(50,50)
screen_3_img_6.set_angle(0)
screen_3_img_6.set_pos(373, 9)
screen_3_img_6.set_size(80, 112)
# Set style for screen_3_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_5
screen_3_img_5 = lv.img(screen_3_cont_7)
screen_3_img_5.set_src("B:MicroPython/_DIV7_alpha_80x112.bin")
screen_3_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_5.set_pivot(50,50)
screen_3_img_5.set_angle(0)
screen_3_img_5.set_pos(945, 9)
screen_3_img_5.set_size(80, 112)
# Set style for screen_3_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_4
screen_3_img_4 = lv.img(screen_3_cont_7)
screen_3_img_4.set_src("B:MicroPython/_DIV2_alpha_80x112.bin")
screen_3_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_4.set_pivot(50,50)
screen_3_img_4.set_angle(0)
screen_3_img_4.set_pos(516, 9)
screen_3_img_4.set_size(80, 112)
# Set style for screen_3_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_img_3
screen_3_img_3 = lv.img(screen_3_cont_7)
screen_3_img_3.set_src("B:MicroPython/_DIV8_alpha_80x112.bin")
screen_3_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_3.set_pivot(50,50)
screen_3_img_3.set_angle(0)
screen_3_img_3.set_pos(1088, 9)
screen_3_img_3.set_size(80, 112)
# Set style for screen_3_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_14
screen_3_btn_14 = lv.btn(screen_3_cont_7)
screen_3_btn_14_label = lv.label(screen_3_btn_14)
screen_3_btn_14_label.set_text("")
screen_3_btn_14_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_14_label.set_width(lv.pct(100))
screen_3_btn_14_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_14.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_14.set_pos(497, 10)
screen_3_btn_14.set_size(122, 98)
# Set style for screen_3_btn_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_14.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_14.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_13
screen_3_btn_13 = lv.btn(screen_3_cont_7)
screen_3_btn_13_label = lv.label(screen_3_btn_13)
screen_3_btn_13_label.set_text("")
screen_3_btn_13_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_13_label.set_width(lv.pct(100))
screen_3_btn_13_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_13.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_13.set_pos(639, 10)
screen_3_btn_13.set_size(126, 98)
# Set style for screen_3_btn_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_13.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_13.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_12
screen_3_btn_12 = lv.btn(screen_3_cont_7)
screen_3_btn_12_label = lv.label(screen_3_btn_12)
screen_3_btn_12_label.set_text("")
screen_3_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_12_label.set_width(lv.pct(100))
screen_3_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_12.set_pos(349, 10)
screen_3_btn_12.set_size(122, 98)
# Set style for screen_3_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_11
screen_3_btn_11 = lv.btn(screen_3_cont_7)
screen_3_btn_11_label = lv.label(screen_3_btn_11)
screen_3_btn_11_label.set_text("")
screen_3_btn_11_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_11_label.set_width(lv.pct(100))
screen_3_btn_11_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_11.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_11.set_pos(1068, 5)
screen_3_btn_11.set_size(126, 98)
# Set style for screen_3_btn_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_10
screen_3_btn_10 = lv.btn(screen_3_cont_7)
screen_3_btn_10_label = lv.label(screen_3_btn_10)
screen_3_btn_10_label.set_text("")
screen_3_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_10_label.set_width(lv.pct(100))
screen_3_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_10.set_pos(921, 10)
screen_3_btn_10.set_size(121, 98)
# Set style for screen_3_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_9
screen_3_btn_9 = lv.btn(screen_3_cont_7)
screen_3_btn_9_label = lv.label(screen_3_btn_9)
screen_3_btn_9_label.set_text("")
screen_3_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_9_label.set_width(lv.pct(100))
screen_3_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_9.set_pos(71, 10)
screen_3_btn_9.set_size(122, 98)
# Set style for screen_3_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_4
screen_3_cont_4 = lv.obj(screen_3)
screen_3_cont_4.set_pos(8, -220)
screen_3_cont_4.set_size(1280, 800)
screen_3_cont_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_bg_color(lv.color_hex(0x221936), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_img_2
screen_3_img_2 = lv.img(screen_3_cont_4)
screen_3_img_2.set_src("B:MicroPython/_speake1_1280x94.bin")
screen_3_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_3_img_2.set_pivot(50,50)
screen_3_img_2.set_angle(0)
screen_3_img_2.set_pos(0, 0)
screen_3_img_2.set_size(1280, 94)
# Set style for screen_3_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_2
screen_3_cont_2 = lv.obj(screen_3_cont_4)
screen_3_cont_2.set_pos(17, 94)
screen_3_cont_2.set_size(400, 584)
screen_3_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_btn_4
screen_3_btn_4 = lv.btn(screen_3_cont_2)
screen_3_btn_4_label = lv.label(screen_3_btn_4)
screen_3_btn_4_label.set_text("⬅️ 清空")
screen_3_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_4_label.set_width(lv.pct(100))
screen_3_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_4.set_pos(210, 113)
screen_3_btn_4.set_size(161, 50)
# Set style for screen_3_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_3
screen_3_btn_3 = lv.btn(screen_3_cont_2)
screen_3_btn_3_label = lv.label(screen_3_btn_3)
screen_3_btn_3_label.set_text("X  删除")
screen_3_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_3_label.set_width(lv.pct(100))
screen_3_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_3.set_pos(34, 113)
screen_3_btn_3.set_size(161, 50)
# Set style for screen_3_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_ta_1
screen_3_ta_1 = lv.textarea(screen_3_cont_2)
screen_3_ta_1.set_text("")
screen_3_ta_1.set_placeholder_text("请点选歌名首字母")
screen_3_ta_1.set_password_bullet("*")
screen_3_ta_1.set_password_mode(False)
screen_3_ta_1.set_one_line(False)
screen_3_ta_1.set_accepted_chars("")
screen_3_ta_1.set_max_length(32)
screen_3_ta_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_3), lv.EVENT.ALL, None)
screen_3_ta_1.set_pos(25, 22)
screen_3_ta_1.set_size(349, 62)
# Set style for screen_3_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_ta_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_radius(17, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_3_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_3_ta_1.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_3_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_3_btn_6
screen_3_btn_6 = lv.btn(screen_3_cont_4)
screen_3_btn_6_label = lv.label(screen_3_btn_6)
screen_3_btn_6_label.set_text("\n")
screen_3_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_6_label.set_width(lv.pct(100))
screen_3_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_6.set_pos(14, 8)
screen_3_btn_6.set_size(290, 91)
# Set style for screen_3_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_3
screen_3_cont_3 = lv.obj(screen_3_cont_4)
screen_3_cont_3.set_pos(429, 124)
screen_3_cont_3.set_size(974, 675)
screen_3_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_8
screen_3_cont_8 = lv.obj(screen_3_cont_4)
screen_3_cont_8.set_pos(8, 690)
screen_3_cont_8.set_size(232, 103)
screen_3_cont_8.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_qrcode_1
screen_3_qrcode_1 = lv.qrcode(screen_3_cont_8, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_3_qrcode_1_data = "16666666666"
screen_3_qrcode_1.update(screen_3_qrcode_1_data, len(screen_3_qrcode_1_data))
screen_3_qrcode_1.set_pos(13, 15)
screen_3_qrcode_1.set_size(75, 75)

# Create screen_3_label_1
screen_3_label_1 = lv.label(screen_3_cont_8)
screen_3_label_1.set_text("微信扫一扫\n点歌更方便")
screen_3_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_3_label_1.set_width(lv.pct(100))
screen_3_label_1.set_pos(98, 24)
screen_3_label_1.set_size(174, 57)
# Set style for screen_3_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_cont_5
screen_3_cont_5 = lv.obj(screen_3)
screen_3_cont_5.set_pos(2, 2)
screen_3_cont_5.set_size(1267, 795)
screen_3_cont_5.add_flag(lv.obj.FLAG.HIDDEN)
screen_3_cont_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_cont_6
screen_3_cont_6 = lv.obj(screen_3_cont_5)
screen_3_cont_6.set_pos(-307, 106)
screen_3_cont_6.set_size(296, 679)
screen_3_cont_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_cont_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_cont_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_cont_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_3_btn_8
screen_3_btn_8 = lv.btn(screen_3_cont_6)
screen_3_btn_8_label = lv.label(screen_3_btn_8)
screen_3_btn_8_label.set_text("收藏")
screen_3_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_8_label.set_width(lv.pct(100))
screen_3_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_8.set_pos(161, 621)
screen_3_btn_8.set_size(100, 50)
# Set style for screen_3_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_8.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_7
screen_3_btn_7 = lv.btn(screen_3_cont_6)
screen_3_btn_7_label = lv.label(screen_3_btn_7)
screen_3_btn_7_label.set_text("添加已点")
screen_3_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_7_label.set_width(lv.pct(100))
screen_3_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_7.set_pos(25, 621)
screen_3_btn_7.set_size(100, 50)
# Set style for screen_3_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_list_1
screen_3_list_1 = lv.list(screen_3_cont_6)
screen_3_list_1_item0 = screen_3_list_1.add_btn(lv.SYMBOL.SAVE, "聚会点唱")
screen_3_list_1_item1 = screen_3_list_1.add_btn(lv.SYMBOL.SAVE, "save_1")
screen_3_list_1.set_pos(10, 6)
screen_3_list_1.set_size(274, 597)
screen_3_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_pad_left(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_color(lv.color_hex(0x6b6b6b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_border_opa(188, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_border_color(lv.color_hex(0xe1e6ee), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_3_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_3_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_3_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_3_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_3_list_1_extra_btns_main_default = lv.style_t()
style_screen_3_list_1_extra_btns_main_default.init()
style_screen_3_list_1_extra_btns_main_default.set_pad_top(5)
style_screen_3_list_1_extra_btns_main_default.set_pad_left(5)
style_screen_3_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_3_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_3_list_1_extra_btns_main_default.set_border_width(0)
style_screen_3_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_3_list_1_extra_btns_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_3_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_3_list_1_extra_btns_main_default.set_radius(3)
style_screen_3_list_1_extra_btns_main_default.set_bg_opa(255)
style_screen_3_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_3_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_3_list_1_item1.add_style(style_screen_3_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_list_1_item0.add_style(style_screen_3_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_3_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_3_list_1_extra_texts_main_default = lv.style_t()
style_screen_3_list_1_extra_texts_main_default.init()
style_screen_3_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_3_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_3_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_3_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_3_list_1_extra_texts_main_default.set_border_width(0)
style_screen_3_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_3_list_1_extra_texts_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_3_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_3_list_1_extra_texts_main_default.set_radius(3)
style_screen_3_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_3_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_3_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_3_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

# Create screen_3_btn_15
screen_3_btn_15 = lv.btn(screen_3)
screen_3_btn_15_label = lv.label(screen_3_btn_15)
screen_3_btn_15_label.set_text("Button")
screen_3_btn_15_label.set_long_mode(lv.label.LONG.WRAP)
screen_3_btn_15_label.set_width(lv.pct(100))
screen_3_btn_15_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_15.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_15.set_pos(1090, 155)
screen_3_btn_15.set_size(100, 50)
# Set style for screen_3_btn_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_15.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_15.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_3.update_layout()
# Create screen_5
screen_5 = lv.obj()
g_kb_screen_5 = lv.keyboard(screen_5)
g_kb_screen_5.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_5), lv.EVENT.ALL, None)
g_kb_screen_5.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_5.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_size(1280, 800)
screen_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_style_bg_color(lv.color_hex(0x150c2b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_cont_2
screen_5_cont_2 = lv.obj(screen_5)
screen_5_cont_2.set_pos(20, 163)
screen_5_cont_2.set_size(400, 472)
screen_5_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_5_ta_2
screen_5_ta_2 = lv.textarea(screen_5_cont_2)
screen_5_ta_2.set_text("")
screen_5_ta_2.set_placeholder_text("请点选歌名首字母")
screen_5_ta_2.set_password_bullet("*")
screen_5_ta_2.set_password_mode(False)
screen_5_ta_2.set_one_line(False)
screen_5_ta_2.set_accepted_chars("")
screen_5_ta_2.set_max_length(32)
screen_5_ta_2.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_5), lv.EVENT.ALL, None)
screen_5_ta_2.set_pos(25, 22)
screen_5_ta_2.set_size(349, 62)
# Set style for screen_5_ta_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_ta_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_radius(17, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_5_ta_2, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_5_ta_2.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_5_ta_2.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_5_btn_3
screen_5_btn_3 = lv.btn(screen_5)
screen_5_btn_3_label = lv.label(screen_5_btn_3)
screen_5_btn_3_label.set_text("X  删除")
screen_5_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_3_label.set_width(lv.pct(100))
screen_5_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_3.set_pos(50, 266)
screen_5_btn_3.set_size(161, 50)
# Set style for screen_5_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_4
screen_5_btn_4 = lv.btn(screen_5)
screen_5_btn_4_label = lv.label(screen_5_btn_4)
screen_5_btn_4_label.set_text("清空")
screen_5_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_4_label.set_width(lv.pct(100))
screen_5_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_4.set_pos(224, 266)
screen_5_btn_4.set_size(161, 50)
# Set style for screen_5_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_5
screen_5_btn_5 = lv.btn(screen_5)
screen_5_btn_5_label = lv.label(screen_5_btn_5)
screen_5_btn_5_label.set_text("back\n")
screen_5_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_5_label.set_width(lv.pct(100))
screen_5_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_5.set_pos(14, 8)
screen_5_btn_5.set_size(136, 58)
# Set style for screen_5_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_img_2
screen_5_img_2 = lv.img(screen_5)
screen_5_img_2.set_src("B:MicroPython/_speaker11_1280x141.bin")
screen_5_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_5_img_2.set_pivot(50,50)
screen_5_img_2.set_angle(0)
screen_5_img_2.set_pos(0, 30)
screen_5_img_2.set_size(1280, 141)
# Set style for screen_5_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_6
screen_5_btn_6 = lv.btn(screen_5)
screen_5_btn_6_label = lv.label(screen_5_btn_6)
screen_5_btn_6_label.set_text("退出\n")
screen_5_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_6_label.set_width(lv.pct(100))
screen_5_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_6.set_pos(14, 8)
screen_5_btn_6.set_size(195, 80)
# Set style for screen_5_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_7
screen_5_btn_7 = lv.btn(screen_5)
screen_5_btn_7_label = lv.label(screen_5_btn_7)
screen_5_btn_7_label.set_text("")
screen_5_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_7_label.set_width(lv.pct(100))
screen_5_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_7.set_pos(282, 99)
screen_5_btn_7.set_size(225, 67)
# Set style for screen_5_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_8
screen_5_btn_8 = lv.btn(screen_5)
screen_5_btn_8_label = lv.label(screen_5_btn_8)
screen_5_btn_8_label.set_text("")
screen_5_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_8_label.set_width(lv.pct(100))
screen_5_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_8.set_pos(516, 102)
screen_5_btn_8.set_size(225, 67)
# Set style for screen_5_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_9
screen_5_btn_9 = lv.btn(screen_5)
screen_5_btn_9_label = lv.label(screen_5_btn_9)
screen_5_btn_9_label.set_text("")
screen_5_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_9_label.set_width(lv.pct(100))
screen_5_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_9.set_pos(758, 99)
screen_5_btn_9.set_size(225, 67)
# Set style for screen_5_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_10
screen_5_btn_10 = lv.btn(screen_5)
screen_5_btn_10_label = lv.label(screen_5_btn_10)
screen_5_btn_10_label.set_text("")
screen_5_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_10_label.set_width(lv.pct(100))
screen_5_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_10.set_pos(1009, 99)
screen_5_btn_10.set_size(225, 67)
# Set style for screen_5_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_11
screen_5_btn_11 = lv.btn(screen_5)
screen_5_btn_11_label = lv.label(screen_5_btn_11)
screen_5_btn_11_label.set_text("")
screen_5_btn_11_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_btn_11_label.set_width(lv.pct(100))
screen_5_btn_11_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_11.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_11.set_pos(41, 99)
screen_5_btn_11.set_size(225, 67)
# Set style for screen_5_btn_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_label_1
screen_5_label_1 = lv.label(screen_5)
screen_5_label_1.set_text("0")
screen_5_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_5_label_1.set_width(lv.pct(100))
screen_5_label_1.set_pos(1101, 49)
screen_5_label_1.set_size(63, 37)
# Set style for screen_5_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_cont_3
screen_5_cont_3 = lv.obj(screen_5)
screen_5_cont_3.set_pos(8, 690)
screen_5_cont_3.set_size(232, 103)
screen_5_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_5_qrcode_1
screen_5_qrcode_1 = lv.qrcode(screen_5_cont_3, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_5_qrcode_1_data = "16666666666"
screen_5_qrcode_1.update(screen_5_qrcode_1_data, len(screen_5_qrcode_1_data))
screen_5_qrcode_1.set_pos(13, 15)
screen_5_qrcode_1.set_size(75, 75)

# Create screen_5_label_2
screen_5_label_2 = lv.label(screen_5_cont_3)
screen_5_label_2.set_text("微信扫一扫\n点歌更方便")
screen_5_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_5_label_2.set_width(lv.pct(100))
screen_5_label_2.set_pos(99, 25)
screen_5_label_2.set_size(174, 57)
# Set style for screen_5_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_5.update_layout()
# Create screen_5_1
screen_5_1 = lv.obj()
g_kb_screen_5_1 = lv.keyboard(screen_5_1)
g_kb_screen_5_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_5_1), lv.EVENT.ALL, None)
g_kb_screen_5_1.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_5_1.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1.set_size(1280, 800)
screen_5_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1.set_style_bg_color(lv.color_hex(0x150c2b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_img_1
screen_5_1_img_1 = lv.img(screen_5_1)
screen_5_1_img_1.set_src("B:MicroPython/_speake1_1280x94.bin")
screen_5_1_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_5_1_img_1.set_pivot(50,50)
screen_5_1_img_1.set_angle(0)
screen_5_1_img_1.set_pos(0, 0)
screen_5_1_img_1.set_size(1280, 94)
# Set style for screen_5_1_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_btn_6
screen_5_1_btn_6 = lv.btn(screen_5_1)
screen_5_1_btn_6_label = lv.label(screen_5_1_btn_6)
screen_5_1_btn_6_label.set_text("\n")
screen_5_1_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_1_btn_6_label.set_width(lv.pct(100))
screen_5_1_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_1_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_1_btn_6.set_pos(3, 2)
screen_5_1_btn_6.set_size(246, 106)
# Set style for screen_5_1_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_cont_1
screen_5_1_cont_1 = lv.obj(screen_5_1)
screen_5_1_cont_1.set_pos(17, 94)
screen_5_1_cont_1.set_size(400, 551)
screen_5_1_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_1_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_5_1_ta_1
screen_5_1_ta_1 = lv.textarea(screen_5_1_cont_1)
screen_5_1_ta_1.set_text("")
screen_5_1_ta_1.set_placeholder_text("请点选歌名首字母")
screen_5_1_ta_1.set_password_bullet("*")
screen_5_1_ta_1.set_password_mode(False)
screen_5_1_ta_1.set_one_line(False)
screen_5_1_ta_1.set_accepted_chars("")
screen_5_1_ta_1.set_max_length(32)
screen_5_1_ta_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_5_1), lv.EVENT.ALL, None)
screen_5_1_ta_1.set_pos(26, 22)
screen_5_1_ta_1.set_size(349, 62)
# Set style for screen_5_1_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_ta_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_radius(17, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_5_1_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_5_1_ta_1.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_5_1_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_5_1_btn_8
screen_5_1_btn_8 = lv.btn(screen_5_1_cont_1)
screen_5_1_btn_8_label = lv.label(screen_5_1_btn_8)
screen_5_1_btn_8_label.set_text("清空")
screen_5_1_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_1_btn_8_label.set_width(lv.pct(100))
screen_5_1_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_1_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_1_btn_8.set_pos(210, 112)
screen_5_1_btn_8.set_size(161, 50)
# Set style for screen_5_1_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_btn_8.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_btn_7
screen_5_1_btn_7 = lv.btn(screen_5_1_cont_1)
screen_5_1_btn_7_label = lv.label(screen_5_1_btn_7)
screen_5_1_btn_7_label.set_text("X  删除")
screen_5_1_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_5_1_btn_7_label.set_width(lv.pct(100))
screen_5_1_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_1_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_1_btn_7.set_pos(25, 114)
screen_5_1_btn_7.set_size(161, 50)
# Set style for screen_5_1_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_btn_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_cont_2
screen_5_1_cont_2 = lv.obj(screen_5_1)
screen_5_1_cont_2.set_pos(428, 129)
screen_5_1_cont_2.set_size(893, 677)
screen_5_1_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_1_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_1_cont_3
screen_5_1_cont_3 = lv.obj(screen_5_1)
screen_5_1_cont_3.set_pos(8, 690)
screen_5_1_cont_3.set_size(232, 103)
screen_5_1_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5_1_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_5_1_qrcode_1
screen_5_1_qrcode_1 = lv.qrcode(screen_5_1_cont_3, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_5_1_qrcode_1_data = "16666666666"
screen_5_1_qrcode_1.update(screen_5_1_qrcode_1_data, len(screen_5_1_qrcode_1_data))
screen_5_1_qrcode_1.set_pos(13, 15)
screen_5_1_qrcode_1.set_size(75, 75)

# Create screen_5_1_label_1
screen_5_1_label_1 = lv.label(screen_5_1_cont_3)
screen_5_1_label_1.set_text("微信扫一扫\n点歌更方便")
screen_5_1_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_5_1_label_1.set_width(lv.pct(100))
screen_5_1_label_1.set_pos(99, 25)
screen_5_1_label_1.set_size(174, 57)
# Set style for screen_5_1_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_1_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_1_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_5_1.update_layout()
# Create screen_6
screen_6 = lv.obj()
g_kb_screen_6 = lv.keyboard(screen_6)
g_kb_screen_6.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_6), lv.EVENT.ALL, None)
g_kb_screen_6.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_6.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_size(1280, 800)
screen_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_style_bg_color(lv.color_hex(0x342c51), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_img_1
screen_6_img_1 = lv.img(screen_6)
screen_6_img_1.set_src("B:MicroPython/_speaker66_alpha_249x591.bin")
screen_6_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_6_img_1.set_pivot(50,50)
screen_6_img_1.set_angle(0)
screen_6_img_1.set_pos(14, 142)
screen_6_img_1.set_size(249, 591)
# Set style for screen_6_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_img_2
screen_6_img_2 = lv.img(screen_6)
screen_6_img_2.set_src("B:MicroPython/_speake44png_1004x158.bin")
screen_6_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_6_img_2.set_pivot(50,50)
screen_6_img_2.set_angle(0)
screen_6_img_2.set_pos(272, 6)
screen_6_img_2.set_size(1004, 158)
# Set style for screen_6_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_cont_1
screen_6_cont_1 = lv.obj(screen_6)
screen_6_cont_1.set_pos(272, 164)
screen_6_cont_1.set_size(1091, 634)
screen_6_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
screen_6_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_bg_color(lv.color_hex(0x2a2641), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_btn_1
screen_6_btn_1 = lv.btn(screen_6)
screen_6_btn_1_label = lv.label(screen_6_btn_1)
screen_6_btn_1_label.set_text("退出")
screen_6_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_btn_1_label.set_width(lv.pct(100))
screen_6_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_btn_1.set_pos(14, 6)
screen_6_btn_1.set_size(246, 101)
# Set style for screen_6_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_btn_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_btn_2
screen_6_btn_2 = lv.btn(screen_6)
screen_6_btn_2_label = lv.label(screen_6_btn_2)
screen_6_btn_2_label.set_text("")
screen_6_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_btn_2_label.set_width(lv.pct(100))
screen_6_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_btn_2.set_pos(21, 142)
screen_6_btn_2.set_size(238, 192)
# Set style for screen_6_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_btn_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_btn_3
screen_6_btn_3 = lv.btn(screen_6)
screen_6_btn_3_label = lv.label(screen_6_btn_3)
screen_6_btn_3_label.set_text("")
screen_6_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_btn_3_label.set_width(lv.pct(100))
screen_6_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_btn_3.set_pos(24, 337)
screen_6_btn_3.set_size(238, 192)
# Set style for screen_6_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_btn_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_btn_4
screen_6_btn_4 = lv.btn(screen_6)
screen_6_btn_4_label = lv.label(screen_6_btn_4)
screen_6_btn_4_label.set_text("")
screen_6_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_btn_4_label.set_width(lv.pct(100))
screen_6_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_btn_4.set_pos(21, 534)
screen_6_btn_4.set_size(238, 192)
# Set style for screen_6_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_btn_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_cont_2
screen_6_cont_2 = lv.obj(screen_6)
screen_6_cont_2.set_pos(272, 164)
screen_6_cont_2.set_size(1091, 634)
screen_6_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_bg_color(lv.color_hex(0x2a2641), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_6.update_layout()
# Create screen_10
screen_10 = lv.obj()
g_kb_screen_10 = lv.keyboard(screen_10)
g_kb_screen_10.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_10), lv.EVENT.ALL, None)
g_kb_screen_10.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_10.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10.set_size(1280, 800)
screen_10.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_10.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_10_btn_1
screen_10_btn_1 = lv.btn(screen_10)
screen_10_btn_1_label = lv.label(screen_10_btn_1)
screen_10_btn_1_label.set_text("退出点歌")
screen_10_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_10_btn_1_label.set_width(lv.pct(100))
screen_10_btn_1_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_10_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_10_btn_1.set_pos(27, -6)
screen_10_btn_1.set_size(267, 99)
# Set style for screen_10_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_10_btn_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_btn_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_10_cont_1
screen_10_cont_1 = lv.obj(screen_10)
screen_10_cont_1.set_pos(151, 120)
screen_10_cont_1.set_size(1084, 677)
screen_10_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_10_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_10_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_10_cont_2
screen_10_cont_2 = lv.obj(screen_10)
screen_10_cont_2.set_pos(8, 690)
screen_10_cont_2.set_size(232, 103)
screen_10_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_10_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_10_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_10_qrcode_1
screen_10_qrcode_1 = lv.qrcode(screen_10_cont_2, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_10_qrcode_1_data = "16666666666"
screen_10_qrcode_1.update(screen_10_qrcode_1_data, len(screen_10_qrcode_1_data))
screen_10_qrcode_1.set_pos(13, 15)
screen_10_qrcode_1.set_size(75, 75)

# Create screen_10_label_1
screen_10_label_1 = lv.label(screen_10_cont_2)
screen_10_label_1.set_text("微信扫一扫\n点歌更方便")
screen_10_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_10_label_1.set_width(lv.pct(100))
screen_10_label_1.set_pos(99, 25)
screen_10_label_1.set_size(174, 57)
# Set style for screen_10_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_10_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_10_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_10.update_layout()
# Create screen_11
screen_11 = lv.obj()
g_kb_screen_11 = lv.keyboard(screen_11)
g_kb_screen_11.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_11), lv.EVENT.ALL, None)
g_kb_screen_11.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_11.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11.set_size(1280, 800)
screen_11.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_11.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11.set_style_bg_color(lv.color_hex(0x170d2a), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_11_btn_5
screen_11_btn_5 = lv.btn(screen_11)
screen_11_btn_5_label = lv.label(screen_11_btn_5)
screen_11_btn_5_label.set_text("back\n")
screen_11_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_11_btn_5_label.set_width(lv.pct(100))
screen_11_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_11_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_11_btn_5.set_pos(15, 8)
screen_11_btn_5.set_size(248, 107)
# Set style for screen_11_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_11_btn_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_11_img_2
screen_11_img_2 = lv.img(screen_11)
screen_11_img_2.set_src("B:MicroPython/_speaker1png_517x65.bin")
screen_11_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_11_img_2.set_pivot(50,50)
screen_11_img_2.set_angle(0)
screen_11_img_2.set_pos(700, 28)
screen_11_img_2.set_size(517, 65)
# Set style for screen_11_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_11_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_11_cont_1
screen_11_cont_1 = lv.obj(screen_11)
screen_11_cont_1.set_pos(437, 130)
screen_11_cont_1.set_size(893, 677)
screen_11_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_11_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_11_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_11_list_1
screen_11_list_1 = lv.list(screen_11)
screen_11_list_1_item0 = screen_11_list_1.add_btn(lv.SYMBOL.SAVE, "聚会点唱")
screen_11_list_1_item1 = screen_11_list_1.add_btn(lv.SYMBOL.SAVE, "save_1")
screen_11_list_1.set_pos(22, 93)
screen_11_list_1.set_size(274, 677)
screen_11_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_11_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_11_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_color(lv.color_hex(0x6b6b6b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_border_color(lv.color_hex(0xe1e6ee), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_radius(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_11_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_11_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_11_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_11_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_11_list_1_extra_btns_main_default = lv.style_t()
style_screen_11_list_1_extra_btns_main_default.init()
style_screen_11_list_1_extra_btns_main_default.set_pad_top(5)
style_screen_11_list_1_extra_btns_main_default.set_pad_left(5)
style_screen_11_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_11_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_11_list_1_extra_btns_main_default.set_border_width(0)
style_screen_11_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_11_list_1_extra_btns_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_11_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_11_list_1_extra_btns_main_default.set_radius(3)
style_screen_11_list_1_extra_btns_main_default.set_bg_opa(255)
style_screen_11_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_11_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_11_list_1_item1.add_style(style_screen_11_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_11_list_1_item0.add_style(style_screen_11_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_11_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_11_list_1_extra_texts_main_default = lv.style_t()
style_screen_11_list_1_extra_texts_main_default.init()
style_screen_11_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_11_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_11_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_11_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_11_list_1_extra_texts_main_default.set_border_width(0)
style_screen_11_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_11_list_1_extra_texts_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_11_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_11_list_1_extra_texts_main_default.set_radius(3)
style_screen_11_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_11_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_11_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_11_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

screen_11.update_layout()
# Create screen_12
screen_12 = lv.obj()
g_kb_screen_12 = lv.keyboard(screen_12)
g_kb_screen_12.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_12), lv.EVENT.ALL, None)
g_kb_screen_12.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_12.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12.set_size(1280, 800)
screen_12.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_list_1
screen_12_list_1 = lv.list(screen_12)
screen_12_list_1_item0 = screen_12_list_1.add_btn(lv.SYMBOL.SAVE, "save")
screen_12_list_1.set_pos(10, 101)
screen_12_list_1.set_size(281, 575)
screen_12_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_12_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_border_color(lv.color_hex(0x6e6e6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_12_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_12_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_12_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_12_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_12_list_1_extra_btns_main_default = lv.style_t()
style_screen_12_list_1_extra_btns_main_default.init()
style_screen_12_list_1_extra_btns_main_default.set_pad_top(5)
style_screen_12_list_1_extra_btns_main_default.set_pad_left(5)
style_screen_12_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_12_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_12_list_1_extra_btns_main_default.set_border_width(0)
style_screen_12_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_12_list_1_extra_btns_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_12_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_12_list_1_extra_btns_main_default.set_radius(3)
style_screen_12_list_1_extra_btns_main_default.set_bg_opa(255)
style_screen_12_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_12_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_12_list_1_item0.add_style(style_screen_12_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_12_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_12_list_1_extra_texts_main_default = lv.style_t()
style_screen_12_list_1_extra_texts_main_default.init()
style_screen_12_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_12_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_12_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_12_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_12_list_1_extra_texts_main_default.set_border_width(0)
style_screen_12_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_12_list_1_extra_texts_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_12_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_12_list_1_extra_texts_main_default.set_radius(3)
style_screen_12_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_12_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_12_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_12_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

# Create screen_12_cont_1
screen_12_cont_1 = lv.obj(screen_12)
screen_12_cont_1.set_pos(1, 687)
screen_12_cont_1.set_size(1388, 107)
screen_12_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_12_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_12_img_10
screen_12_img_10 = lv.img(screen_12_cont_1)
screen_12_img_10.set_src("B:MicroPython/_123124_1280x103.bin")
screen_12_img_10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_10.set_pivot(50,50)
screen_12_img_10.set_angle(0)
screen_12_img_10.set_pos(0, 9)
screen_12_img_10.set_size(1280, 103)
# Set style for screen_12_img_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_8
screen_12_img_8 = lv.img(screen_12_cont_1)
screen_12_img_8.set_src("B:MicroPython/_DIV1_alpha_80x112.bin")
screen_12_img_8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_8.set_pivot(50,50)
screen_12_img_8.set_angle(0)
screen_12_img_8.set_pos(99, 4)
screen_12_img_8.set_size(80, 112)
# Set style for screen_12_img_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_7
screen_12_img_7 = lv.img(screen_12_cont_1)
screen_12_img_7.set_src("B:MicroPython/_DIV5_alpha_80x112.bin")
screen_12_img_7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_7.set_pivot(50,50)
screen_12_img_7.set_angle(0)
screen_12_img_7.set_pos(679, 4)
screen_12_img_7.set_size(80, 112)
# Set style for screen_12_img_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_6
screen_12_img_6 = lv.img(screen_12_cont_1)
screen_12_img_6.set_src("B:MicroPython/_DIV6_alpha_80x112.bin")
screen_12_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_6.set_pivot(50,50)
screen_12_img_6.set_angle(0)
screen_12_img_6.set_pos(824, 4)
screen_12_img_6.set_size(80, 112)
# Set style for screen_12_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_5
screen_12_img_5 = lv.img(screen_12_cont_1)
screen_12_img_5.set_src("B:MicroPython/_DIV4_alpha_80x112.bin")
screen_12_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_5.set_pivot(50,50)
screen_12_img_5.set_angle(0)
screen_12_img_5.set_pos(244, 4)
screen_12_img_5.set_size(80, 112)
# Set style for screen_12_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_4
screen_12_img_4 = lv.img(screen_12_cont_1)
screen_12_img_4.set_src("B:MicroPython/_DIV3_alpha_80x112.bin")
screen_12_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_4.set_pivot(50,50)
screen_12_img_4.set_angle(0)
screen_12_img_4.set_pos(389, 4)
screen_12_img_4.set_size(80, 112)
# Set style for screen_12_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_3
screen_12_img_3 = lv.img(screen_12_cont_1)
screen_12_img_3.set_src("B:MicroPython/_DIV7_alpha_80x112.bin")
screen_12_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_3.set_pivot(50,50)
screen_12_img_3.set_angle(0)
screen_12_img_3.set_pos(969, 4)
screen_12_img_3.set_size(80, 112)
# Set style for screen_12_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_2
screen_12_img_2 = lv.img(screen_12_cont_1)
screen_12_img_2.set_src("B:MicroPython/_DIV2_alpha_80x112.bin")
screen_12_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_2.set_pivot(50,50)
screen_12_img_2.set_angle(0)
screen_12_img_2.set_pos(534, 4)
screen_12_img_2.set_size(80, 112)
# Set style for screen_12_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_img_1
screen_12_img_1 = lv.img(screen_12_cont_1)
screen_12_img_1.set_src("B:MicroPython/_DIV8_alpha_80x112.bin")
screen_12_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_12_img_1.set_pivot(50,50)
screen_12_img_1.set_angle(0)
screen_12_img_1.set_pos(1114, 4)
screen_12_img_1.set_size(80, 112)
# Set style for screen_12_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_7
screen_12_btn_7 = lv.btn(screen_12_cont_1)
screen_12_btn_7_label = lv.label(screen_12_btn_7)
screen_12_btn_7_label.set_text("")
screen_12_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_7_label.set_width(lv.pct(100))
screen_12_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_7.set_pos(511, 12)
screen_12_btn_7.set_size(122, 98)
# Set style for screen_12_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_6
screen_12_btn_6 = lv.btn(screen_12_cont_1)
screen_12_btn_6_label = lv.label(screen_12_btn_6)
screen_12_btn_6_label.set_text("")
screen_12_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_6_label.set_width(lv.pct(100))
screen_12_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_6.set_pos(657, 11)
screen_12_btn_6.set_size(126, 98)
# Set style for screen_12_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_4
screen_12_btn_4 = lv.btn(screen_12_cont_1)
screen_12_btn_4_label = lv.label(screen_12_btn_4)
screen_12_btn_4_label.set_text("")
screen_12_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_4_label.set_width(lv.pct(100))
screen_12_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_4.set_pos(365, 10)
screen_12_btn_4.set_size(122, 98)
# Set style for screen_12_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_3
screen_12_btn_3 = lv.btn(screen_12_cont_1)
screen_12_btn_3_label = lv.label(screen_12_btn_3)
screen_12_btn_3_label.set_text("")
screen_12_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_3_label.set_width(lv.pct(100))
screen_12_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_3.set_pos(1092, 10)
screen_12_btn_3.set_size(126, 98)
# Set style for screen_12_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_2
screen_12_btn_2 = lv.btn(screen_12_cont_1)
screen_12_btn_2_label = lv.label(screen_12_btn_2)
screen_12_btn_2_label.set_text("")
screen_12_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_2_label.set_width(lv.pct(100))
screen_12_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_2.set_pos(949, 10)
screen_12_btn_2.set_size(121, 98)
# Set style for screen_12_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_1
screen_12_btn_1 = lv.btn(screen_12_cont_1)
screen_12_btn_1_label = lv.label(screen_12_btn_1)
screen_12_btn_1_label.set_text("")
screen_12_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_1_label.set_width(lv.pct(100))
screen_12_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_1.set_pos(78, 12)
screen_12_btn_1.set_size(122, 98)
# Set style for screen_12_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_label_1
screen_12_label_1 = lv.label(screen_12)
screen_12_label_1.set_text("我的mv")
screen_12_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_12_label_1.set_width(lv.pct(100))
screen_12_label_1.set_pos(74, 45)
screen_12_label_1.set_size(501, 50)
# Set style for screen_12_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_cont_2
screen_12_cont_2 = lv.obj(screen_12)
screen_12_cont_2.set_pos(971, 322)
screen_12_cont_2.set_size(72, 366)
screen_12_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
screen_12_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_12_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_radius(70, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_12_slider_1
screen_12_slider_1 = lv.slider(screen_12_cont_2)
screen_12_slider_1.set_range(0, 100)
screen_12_slider_1.set_mode(lv.slider.MODE.NORMAL)
screen_12_slider_1.set_value(50, lv.ANIM.OFF)
screen_12_slider_1.set_pos(26, 36)
screen_12_slider_1.set_size(17, 295)
# Set style for screen_12_slider_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_slider_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_color(lv.color_hex(0x626262), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_radius(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_outline_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_12_slider_1, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_12_slider_1.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_color(lv.color_hex(0xfff700), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_radius(8, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_12_slider_1, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_12_slider_1.set_style_bg_opa(255, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_color(lv.color_hex(0xffe200), lv.PART.KNOB|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_12_slider_1.set_style_radius(8, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_12_btn_8
screen_12_btn_8 = lv.btn(screen_12)
screen_12_btn_8_label = lv.label(screen_12_btn_8)
screen_12_btn_8_label.set_text("Button")
screen_12_btn_8_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_8_label.set_width(lv.pct(100))
screen_12_btn_8_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_8.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_8.set_pos(89, 628)
screen_12_btn_8.set_size(100, 41)
# Set style for screen_12_btn_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_8.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_12_btn_9
screen_12_btn_9 = lv.btn(screen_12)
screen_12_btn_9_label = lv.label(screen_12_btn_9)
screen_12_btn_9_label.set_text("\n")
screen_12_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_12_btn_9_label.set_width(lv.pct(100))
screen_12_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_12_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_12_btn_9.set_pos(4, 2)
screen_12_btn_9.set_size(273, 95)
# Set style for screen_12_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_12_btn_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_12_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_12.update_layout()
# Create screen_13
screen_13 = lv.obj()
g_kb_screen_13 = lv.keyboard(screen_13)
g_kb_screen_13.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_13), lv.EVENT.ALL, None)
g_kb_screen_13.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_13.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13.set_size(1280, 800)
screen_13.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_13.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_13_img_1
screen_13_img_1 = lv.img(screen_13)
screen_13_img_1.set_src("B:MicroPython/_feswgvwes_alpha_1248x744.bin")
screen_13_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_13_img_1.set_pivot(50,50)
screen_13_img_1.set_angle(0)
screen_13_img_1.set_pos(28, 7)
screen_13_img_1.set_size(1248, 744)
# Set style for screen_13_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_13_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_13_qrcode_1
screen_13_qrcode_1 = lv.qrcode(screen_13, 316, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_13_qrcode_1_data = "16666666666"
screen_13_qrcode_1.update(screen_13_qrcode_1_data, len(screen_13_qrcode_1_data))
screen_13_qrcode_1.set_pos(805, 233)
screen_13_qrcode_1.set_size(316, 316)

# Create screen_13_btn_1
screen_13_btn_1 = lv.btn(screen_13)
screen_13_btn_1_label = lv.label(screen_13_btn_1)
screen_13_btn_1_label.set_text("\n")
screen_13_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_13_btn_1_label.set_width(lv.pct(100))
screen_13_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_13_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_13_btn_1.set_pos(3, 2)
screen_13_btn_1.set_size(431, 175)
# Set style for screen_13_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_13_btn_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_13_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_13.update_layout()
# Create screen_8_1
screen_8_1 = lv.obj()
g_kb_screen_8_1 = lv.keyboard(screen_8_1)
g_kb_screen_8_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_8_1), lv.EVENT.ALL, None)
g_kb_screen_8_1.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_8_1.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1.set_size(1280, 800)
screen_8_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_6
screen_8_1_cont_6 = lv.obj(screen_8_1)
screen_8_1_cont_6.set_pos(2, 686)
screen_8_1_cont_6.set_size(1388, 107)
screen_8_1_cont_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_img_10
screen_8_1_img_10 = lv.img(screen_8_1_cont_6)
screen_8_1_img_10.set_src("B:MicroPython/_123124_1280x103.bin")
screen_8_1_img_10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_10.set_pivot(50,50)
screen_8_1_img_10.set_angle(0)
screen_8_1_img_10.set_pos(0, 9)
screen_8_1_img_10.set_size(1280, 103)
# Set style for screen_8_1_img_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_9
screen_8_1_img_9 = lv.img(screen_8_1_cont_6)
screen_8_1_img_9.set_src("B:MicroPython/_DIV1_alpha_80x112.bin")
screen_8_1_img_9.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_9.set_pivot(50,50)
screen_8_1_img_9.set_angle(0)
screen_8_1_img_9.set_pos(87, 9)
screen_8_1_img_9.set_size(80, 112)
# Set style for screen_8_1_img_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_9.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_9.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_8
screen_8_1_img_8 = lv.img(screen_8_1_cont_6)
screen_8_1_img_8.set_src("B:MicroPython/_DIV5_alpha_80x112.bin")
screen_8_1_img_8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_8.set_pivot(50,50)
screen_8_1_img_8.set_angle(0)
screen_8_1_img_8.set_pos(659, 9)
screen_8_1_img_8.set_size(80, 112)
# Set style for screen_8_1_img_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_7
screen_8_1_img_7 = lv.img(screen_8_1_cont_6)
screen_8_1_img_7.set_src("B:MicroPython/_DIV6_alpha_80x112.bin")
screen_8_1_img_7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_7.set_pivot(50,50)
screen_8_1_img_7.set_angle(0)
screen_8_1_img_7.set_pos(802, 9)
screen_8_1_img_7.set_size(80, 112)
# Set style for screen_8_1_img_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_6
screen_8_1_img_6 = lv.img(screen_8_1_cont_6)
screen_8_1_img_6.set_src("B:MicroPython/_DIV4_alpha_80x112.bin")
screen_8_1_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_6.set_pivot(50,50)
screen_8_1_img_6.set_angle(0)
screen_8_1_img_6.set_pos(230, 9)
screen_8_1_img_6.set_size(80, 112)
# Set style for screen_8_1_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_5
screen_8_1_img_5 = lv.img(screen_8_1_cont_6)
screen_8_1_img_5.set_src("B:MicroPython/_DIV3_alpha_80x112.bin")
screen_8_1_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_5.set_pivot(50,50)
screen_8_1_img_5.set_angle(0)
screen_8_1_img_5.set_pos(373, 9)
screen_8_1_img_5.set_size(80, 112)
# Set style for screen_8_1_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_4
screen_8_1_img_4 = lv.img(screen_8_1_cont_6)
screen_8_1_img_4.set_src("B:MicroPython/_DIV7_alpha_80x112.bin")
screen_8_1_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_4.set_pivot(50,50)
screen_8_1_img_4.set_angle(0)
screen_8_1_img_4.set_pos(945, 9)
screen_8_1_img_4.set_size(80, 112)
# Set style for screen_8_1_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_3
screen_8_1_img_3 = lv.img(screen_8_1_cont_6)
screen_8_1_img_3.set_src("B:MicroPython/_DIV2_alpha_80x112.bin")
screen_8_1_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_3.set_pivot(50,50)
screen_8_1_img_3.set_angle(0)
screen_8_1_img_3.set_pos(516, 9)
screen_8_1_img_3.set_size(80, 112)
# Set style for screen_8_1_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_img_2
screen_8_1_img_2 = lv.img(screen_8_1_cont_6)
screen_8_1_img_2.set_src("B:MicroPython/_DIV8_alpha_80x112.bin")
screen_8_1_img_2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_2.set_pivot(50,50)
screen_8_1_img_2.set_angle(0)
screen_8_1_img_2.set_pos(1088, 9)
screen_8_1_img_2.set_size(80, 112)
# Set style for screen_8_1_img_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_20
screen_8_1_btn_20 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_20_label = lv.label(screen_8_1_btn_20)
screen_8_1_btn_20_label.set_text("")
screen_8_1_btn_20_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_20_label.set_width(lv.pct(100))
screen_8_1_btn_20_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_20.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_20.set_pos(497, 10)
screen_8_1_btn_20.set_size(122, 98)
# Set style for screen_8_1_btn_20, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_20.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_20.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_19
screen_8_1_btn_19 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_19_label = lv.label(screen_8_1_btn_19)
screen_8_1_btn_19_label.set_text("")
screen_8_1_btn_19_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_19_label.set_width(lv.pct(100))
screen_8_1_btn_19_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_19.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_19.set_pos(638, 11)
screen_8_1_btn_19.set_size(126, 98)
# Set style for screen_8_1_btn_19, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_19.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_19.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_18
screen_8_1_btn_18 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_18_label = lv.label(screen_8_1_btn_18)
screen_8_1_btn_18_label.set_text("")
screen_8_1_btn_18_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_18_label.set_width(lv.pct(100))
screen_8_1_btn_18_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_18.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_18.set_pos(349, 10)
screen_8_1_btn_18.set_size(122, 98)
# Set style for screen_8_1_btn_18, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_18.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_18.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_17
screen_8_1_btn_17 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_17_label = lv.label(screen_8_1_btn_17)
screen_8_1_btn_17_label.set_text("")
screen_8_1_btn_17_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_17_label.set_width(lv.pct(100))
screen_8_1_btn_17_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_17.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_17.set_pos(1068, 7)
screen_8_1_btn_17.set_size(126, 98)
# Set style for screen_8_1_btn_17, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_17.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_17.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_16
screen_8_1_btn_16 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_16_label = lv.label(screen_8_1_btn_16)
screen_8_1_btn_16_label.set_text("")
screen_8_1_btn_16_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_16_label.set_width(lv.pct(100))
screen_8_1_btn_16_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_16.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_16.set_pos(921, 10)
screen_8_1_btn_16.set_size(121, 98)
# Set style for screen_8_1_btn_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_16.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_16.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_15
screen_8_1_btn_15 = lv.btn(screen_8_1_cont_6)
screen_8_1_btn_15_label = lv.label(screen_8_1_btn_15)
screen_8_1_btn_15_label.set_text("")
screen_8_1_btn_15_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_15_label.set_width(lv.pct(100))
screen_8_1_btn_15_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_15.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_15.set_pos(71, 10)
screen_8_1_btn_15.set_size(122, 98)
# Set style for screen_8_1_btn_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_15.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_text_font(test_font("montserratMedium", 34), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_15.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_5
screen_8_1_cont_5 = lv.obj(screen_8_1)
screen_8_1_cont_5.set_pos(2, 2)
screen_8_1_cont_5.set_size(1280, 800)
screen_8_1_cont_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_bg_color(lv.color_hex(0x221936), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_img_1
screen_8_1_img_1 = lv.img(screen_8_1_cont_5)
screen_8_1_img_1.set_src("B:MicroPython/_speake1_1280x94.bin")
screen_8_1_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_8_1_img_1.set_pivot(50,50)
screen_8_1_img_1.set_angle(0)
screen_8_1_img_1.set_pos(0, 0)
screen_8_1_img_1.set_size(1280, 94)
# Set style for screen_8_1_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_1
screen_8_1_cont_1 = lv.obj(screen_8_1_cont_5)
screen_8_1_cont_1.set_pos(23, 129)
screen_8_1_cont_1.set_size(400, 551)
screen_8_1_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_ta_2
screen_8_1_ta_2 = lv.textarea(screen_8_1_cont_1)
screen_8_1_ta_2.set_text("")
screen_8_1_ta_2.set_placeholder_text("请点选歌名首字母")
screen_8_1_ta_2.set_password_bullet("*")
screen_8_1_ta_2.set_password_mode(False)
screen_8_1_ta_2.set_one_line(False)
screen_8_1_ta_2.set_accepted_chars("")
screen_8_1_ta_2.set_max_length(32)
screen_8_1_ta_2.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_8_1), lv.EVENT.ALL, None)
screen_8_1_ta_2.set_pos(26, 22)
screen_8_1_ta_2.set_size(349, 62)
# Set style for screen_8_1_ta_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_ta_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_radius(17, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_8_1_ta_2, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_8_1_ta_2.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_8_1_ta_2.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_8_1_btn_10
screen_8_1_btn_10 = lv.btn(screen_8_1_cont_1)
screen_8_1_btn_10_label = lv.label(screen_8_1_btn_10)
screen_8_1_btn_10_label.set_text("清空")
screen_8_1_btn_10_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_10_label.set_width(lv.pct(100))
screen_8_1_btn_10_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_10.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_10.set_pos(210, 112)
screen_8_1_btn_10.set_size(161, 50)
# Set style for screen_8_1_btn_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_10.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_9
screen_8_1_btn_9 = lv.btn(screen_8_1_cont_1)
screen_8_1_btn_9_label = lv.label(screen_8_1_btn_9)
screen_8_1_btn_9_label.set_text("X  删除")
screen_8_1_btn_9_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_9_label.set_width(lv.pct(100))
screen_8_1_btn_9_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_9.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_9.set_pos(25, 114)
screen_8_1_btn_9.set_size(161, 50)
# Set style for screen_8_1_btn_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_9.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_6
screen_8_1_btn_6 = lv.btn(screen_8_1_cont_5)
screen_8_1_btn_6_label = lv.label(screen_8_1_btn_6)
screen_8_1_btn_6_label.set_text("\n")
screen_8_1_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_6_label.set_width(lv.pct(100))
screen_8_1_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_6.set_pos(5, 2)
screen_8_1_btn_6.set_size(246, 106)
# Set style for screen_8_1_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_2
screen_8_1_cont_2 = lv.obj(screen_8_1_cont_5)
screen_8_1_cont_2.set_pos(434, 123)
screen_8_1_cont_2.set_size(893, 677)
screen_8_1_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_7
screen_8_1_cont_7 = lv.obj(screen_8_1_cont_5)
screen_8_1_cont_7.set_pos(8, 690)
screen_8_1_cont_7.set_size(232, 103)
screen_8_1_cont_7.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_qrcode_1
screen_8_1_qrcode_1 = lv.qrcode(screen_8_1_cont_7, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_8_1_qrcode_1_data = "16666666666"
screen_8_1_qrcode_1.update(screen_8_1_qrcode_1_data, len(screen_8_1_qrcode_1_data))
screen_8_1_qrcode_1.set_pos(13, 15)
screen_8_1_qrcode_1.set_size(75, 75)

# Create screen_8_1_label_1
screen_8_1_label_1 = lv.label(screen_8_1_cont_7)
screen_8_1_label_1.set_text("微信扫一扫\n点歌更方便")
screen_8_1_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_label_1.set_width(lv.pct(100))
screen_8_1_label_1.set_pos(99, 25)
screen_8_1_label_1.set_size(174, 57)
# Set style for screen_8_1_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_cont_3
screen_8_1_cont_3 = lv.obj(screen_8_1)
screen_8_1_cont_3.set_pos(3, 2)
screen_8_1_cont_3.set_size(1267, 795)
screen_8_1_cont_3.add_flag(lv.obj.FLAG.HIDDEN)
screen_8_1_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_cont_4
screen_8_1_cont_4 = lv.obj(screen_8_1_cont_3)
screen_8_1_cont_4.set_pos(-307, 106)
screen_8_1_cont_4.set_size(296, 679)
screen_8_1_cont_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_cont_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_cont_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_cont_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_8_1_btn_13
screen_8_1_btn_13 = lv.btn(screen_8_1_cont_4)
screen_8_1_btn_13_label = lv.label(screen_8_1_btn_13)
screen_8_1_btn_13_label.set_text("收藏")
screen_8_1_btn_13_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_13_label.set_width(lv.pct(100))
screen_8_1_btn_13_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_13.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_13.set_pos(161, 621)
screen_8_1_btn_13.set_size(100, 50)
# Set style for screen_8_1_btn_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_13.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_13.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_btn_12
screen_8_1_btn_12 = lv.btn(screen_8_1_cont_4)
screen_8_1_btn_12_label = lv.label(screen_8_1_btn_12)
screen_8_1_btn_12_label.set_text("添加已点")
screen_8_1_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_8_1_btn_12_label.set_width(lv.pct(100))
screen_8_1_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_8_1_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_8_1_btn_12.set_pos(25, 621)
screen_8_1_btn_12.set_size(100, 50)
# Set style for screen_8_1_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_btn_12.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_8_1_list_1
screen_8_1_list_1 = lv.list(screen_8_1_cont_4)
screen_8_1_list_1_item0 = screen_8_1_list_1.add_btn(lv.SYMBOL.SAVE, "聚会点唱")
screen_8_1_list_1_item1 = screen_8_1_list_1.add_btn(lv.SYMBOL.SAVE, "save_1")
screen_8_1_list_1.set_pos(10, 6)
screen_8_1_list_1.set_size(274, 597)
screen_8_1_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_8_1_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_8_1_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_pad_left(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_color(lv.color_hex(0x6b6b6b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_border_opa(188, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_border_color(lv.color_hex(0xe1e6ee), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_8_1_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_8_1_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_8_1_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_8_1_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_8_1_list_1_extra_btns_main_default = lv.style_t()
style_screen_8_1_list_1_extra_btns_main_default.init()
style_screen_8_1_list_1_extra_btns_main_default.set_pad_top(5)
style_screen_8_1_list_1_extra_btns_main_default.set_pad_left(5)
style_screen_8_1_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_8_1_list_1_extra_btns_main_default.set_pad_bottom(5)
style_screen_8_1_list_1_extra_btns_main_default.set_border_width(0)
style_screen_8_1_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_8_1_list_1_extra_btns_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_8_1_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_8_1_list_1_extra_btns_main_default.set_radius(3)
style_screen_8_1_list_1_extra_btns_main_default.set_bg_opa(255)
style_screen_8_1_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_8_1_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_8_1_list_1_item1.add_style(style_screen_8_1_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_8_1_list_1_item0.add_style(style_screen_8_1_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_8_1_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_8_1_list_1_extra_texts_main_default = lv.style_t()
style_screen_8_1_list_1_extra_texts_main_default.init()
style_screen_8_1_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_8_1_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_8_1_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_8_1_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_8_1_list_1_extra_texts_main_default.set_border_width(0)
style_screen_8_1_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x0D3055))
style_screen_8_1_list_1_extra_texts_main_default.set_text_font(test_font("montserratMedium", 12))
style_screen_8_1_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_8_1_list_1_extra_texts_main_default.set_radius(3)
style_screen_8_1_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_8_1_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_8_1_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_8_1_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

screen_8_1.update_layout()
# Create screen_14
screen_14 = lv.obj()
g_kb_screen_14 = lv.keyboard(screen_14)
g_kb_screen_14.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_14), lv.EVENT.ALL, None)
g_kb_screen_14.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_14.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14.set_size(1280, 800)
screen_14.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_14.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_14_label_1
screen_14_label_1 = lv.label(screen_14)
screen_14_label_1.set_text("退出收藏")
screen_14_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_14_label_1.set_width(lv.pct(100))
screen_14_label_1.set_pos(41, 37)
screen_14_label_1.set_size(121, 50)
# Set style for screen_14_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_14_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_font(test_font("Regular", 31), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_14_btn_1
screen_14_btn_1 = lv.btn(screen_14)
screen_14_btn_1_label = lv.label(screen_14_btn_1)
screen_14_btn_1_label.set_text("\n")
screen_14_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_14_btn_1_label.set_width(lv.pct(100))
screen_14_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_14_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_14_btn_1.set_pos(6, 7)
screen_14_btn_1.set_size(246, 106)
# Set style for screen_14_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_14_btn_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_14_cont_1
screen_14_cont_1 = lv.obj(screen_14)
screen_14_cont_1.set_pos(162, 91)
screen_14_cont_1.set_size(974, 701)
screen_14_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_14_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_14_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_14_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_14.update_layout()
# Create screen_6_1
screen_6_1 = lv.obj()
g_kb_screen_6_1 = lv.keyboard(screen_6_1)
g_kb_screen_6_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_6_1), lv.EVENT.ALL, None)
g_kb_screen_6_1.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_6_1.set_style_text_font(test_font("SourceHanSerifSC_Regular", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1.set_size(1280, 800)
screen_6_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1.set_style_bg_color(lv.color_hex(0x150c2b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_img_1
screen_6_1_img_1 = lv.img(screen_6_1)
screen_6_1_img_1.set_src("B:MicroPython/_speake1_1280x94.bin")
screen_6_1_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_6_1_img_1.set_pivot(50,50)
screen_6_1_img_1.set_angle(0)
screen_6_1_img_1.set_pos(0, 2)
screen_6_1_img_1.set_size(1280, 94)
# Set style for screen_6_1_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_btn_6
screen_6_1_btn_6 = lv.btn(screen_6_1)
screen_6_1_btn_6_label = lv.label(screen_6_1_btn_6)
screen_6_1_btn_6_label.set_text("\n")
screen_6_1_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_1_btn_6_label.set_width(lv.pct(100))
screen_6_1_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_1_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_1_btn_6.set_pos(3, 2)
screen_6_1_btn_6.set_size(246, 106)
# Set style for screen_6_1_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_btn_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_cont_1
screen_6_1_cont_1 = lv.obj(screen_6_1)
screen_6_1_cont_1.set_pos(17, 94)
screen_6_1_cont_1.set_size(400, 551)
screen_6_1_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_1_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_6_1_ta_3
screen_6_1_ta_3 = lv.textarea(screen_6_1_cont_1)
screen_6_1_ta_3.set_text("")
screen_6_1_ta_3.set_placeholder_text("请点选歌名首字母")
screen_6_1_ta_3.set_password_bullet("*")
screen_6_1_ta_3.set_password_mode(False)
screen_6_1_ta_3.set_one_line(False)
screen_6_1_ta_3.set_accepted_chars("")
screen_6_1_ta_3.set_max_length(32)
screen_6_1_ta_3.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_6_1), lv.EVENT.ALL, None)
screen_6_1_ta_3.set_pos(26, 22)
screen_6_1_ta_3.set_size(349, 62)
# Set style for screen_6_1_ta_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_ta_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_pad_top(16, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_pad_left(12, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_radius(17, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_6_1_ta_3, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_6_1_ta_3.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_6_1_ta_3.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_6_1_btn_12
screen_6_1_btn_12 = lv.btn(screen_6_1_cont_1)
screen_6_1_btn_12_label = lv.label(screen_6_1_btn_12)
screen_6_1_btn_12_label.set_text("清空")
screen_6_1_btn_12_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_1_btn_12_label.set_width(lv.pct(100))
screen_6_1_btn_12_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_1_btn_12.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_1_btn_12.set_pos(210, 112)
screen_6_1_btn_12.set_size(161, 50)
# Set style for screen_6_1_btn_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_btn_12.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_btn_11
screen_6_1_btn_11 = lv.btn(screen_6_1_cont_1)
screen_6_1_btn_11_label = lv.label(screen_6_1_btn_11)
screen_6_1_btn_11_label.set_text("X  删除")
screen_6_1_btn_11_label.set_long_mode(lv.label.LONG.WRAP)
screen_6_1_btn_11_label.set_width(lv.pct(100))
screen_6_1_btn_11_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_1_btn_11.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_1_btn_11.set_pos(25, 114)
screen_6_1_btn_11.set_size(161, 50)
# Set style for screen_6_1_btn_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_btn_11.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_bg_color(lv.color_hex(0x1C1D1E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_border_color(lv.color_hex(0x3b3b3b), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_text_font(test_font("Regular", 23), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_btn_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_cont_2
screen_6_1_cont_2 = lv.obj(screen_6_1)
screen_6_1_cont_2.set_pos(428, 128)
screen_6_1_cont_2.set_size(893, 677)
screen_6_1_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_1_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_1_cont_3
screen_6_1_cont_3 = lv.obj(screen_6_1)
screen_6_1_cont_3.set_pos(8, 690)
screen_6_1_cont_3.set_size(232, 103)
screen_6_1_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6_1_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_6_1_qrcode_1
screen_6_1_qrcode_1 = lv.qrcode(screen_6_1_cont_3, 75, lv.color_hex(0x2C3224), lv.color_hex(0xffffff))
screen_6_1_qrcode_1_data = "16666666666"
screen_6_1_qrcode_1.update(screen_6_1_qrcode_1_data, len(screen_6_1_qrcode_1_data))
screen_6_1_qrcode_1.set_pos(13, 15)
screen_6_1_qrcode_1.set_size(75, 75)

# Create screen_6_1_label_1
screen_6_1_label_1 = lv.label(screen_6_1_cont_3)
screen_6_1_label_1.set_text("微信扫一扫\n点歌更方便")
screen_6_1_label_1.set_long_mode(lv.label.LONG.WRAP)
screen_6_1_label_1.set_width(lv.pct(100))
screen_6_1_label_1.set_pos(99, 25)
screen_6_1_label_1.set_size(174, 57)
# Set style for screen_6_1_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_1_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_font(test_font("Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_line_space(9, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_1_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_6_1.update_layout()

def screen_100_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_100_btn_1.add_event_cb(lambda e: screen_100_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_100_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_100_btn_2.add_event_cb(lambda e: screen_100_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_4_list_1_item1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_4_list_1_item1.add_event_cb(lambda e: screen_4_list_1_item1_event_handler(e), lv.EVENT.ALL, None)

def screen_4_list_1_item2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_4_list_1_item2.add_event_cb(lambda e: screen_4_list_1_item2_event_handler(e), lv.EVENT.ALL, None)

def screen_4_list_1_item3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_4_list_1_item3.add_event_cb(lambda e: screen_4_list_1_item3_event_handler(e), lv.EVENT.ALL, None)

def screen_4_list_1_item4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_4_list_1_item4.add_event_cb(lambda e: screen_4_list_1_item4_event_handler(e), lv.EVENT.ALL, None)

def screen_4_list_1_item5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_4_list_1_item5.add_event_cb(lambda e: screen_4_list_1_item5_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_7.add_event_cb(lambda e: screen_4_slider_7_event_handler(e), lv.EVENT.ALL, None)

def screen_4_ddlist_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_ddlist_5.add_event_cb(lambda e: screen_4_ddlist_5_event_handler(e), lv.EVENT.ALL, None)

def screen_4_ddlist_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_ddlist_4.add_event_cb(lambda e: screen_4_ddlist_4_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.RELEASED):
        pass
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_4.add_event_cb(lambda e: screen_4_slider_4_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.RELEASED):
        pass
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_3.add_event_cb(lambda e: screen_4_slider_3_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_2.add_event_cb(lambda e: screen_4_slider_2_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_1.add_event_cb(lambda e: screen_4_slider_1_event_handler(e), lv.EVENT.ALL, None)

def screen_4_ddlist_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_ddlist_1.add_event_cb(lambda e: screen_4_ddlist_1_event_handler(e), lv.EVENT.ALL, None)

def screen_4_ddlist_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_ddlist_7.add_event_cb(lambda e: screen_4_ddlist_7_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_6.add_event_cb(lambda e: screen_4_slider_6_event_handler(e), lv.EVENT.ALL, None)

def screen_4_slider_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_slider_5.add_event_cb(lambda e: screen_4_slider_5_event_handler(e), lv.EVENT.ALL, None)

def screen_4_ddlist_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_4_ddlist_6.add_event_cb(lambda e: screen_4_ddlist_6_event_handler(e), lv.EVENT.ALL, None)

def screen_log_in_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_log_in.add_event_cb(lambda e: screen_log_in_event_handler(e), lv.EVENT.ALL, None)

def screen_log_in_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        screen_log_in_cont_2.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_log_in_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
        
        

screen_log_in_btn_1.add_event_cb(lambda e: screen_log_in_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_log_in_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_7, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_log_in_btn_3.add_event_cb(lambda e: screen_log_in_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_log_in_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_log_in_btn_4.add_event_cb(lambda e: screen_log_in_btn_4_event_handler(e), lv.EVENT.ALL, None)

def screen_log_in_btn_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_log_in_btn_5.add_event_cb(lambda e: screen_log_in_btn_5_event_handler(e), lv.EVENT.ALL, None)

def screen_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_2.add_event_cb(lambda e: screen_2_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_13_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_2_btn_13.add_event_cb(lambda e: screen_2_btn_13_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_12_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_2_btn_12.add_event_cb(lambda e: screen_2_btn_12_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_10_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_2_btn_10.add_event_cb(lambda e: screen_2_btn_10_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_2_btn_9.add_event_cb(lambda e: screen_2_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_14_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_2_btn_14.add_event_cb(lambda e: screen_2_btn_14_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_15_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_2_btn_15.add_event_cb(lambda e: screen_2_btn_15_event_handler(e), lv.EVENT.ALL, None)

def screen_7_event_handler(e):
    code = e.get_code()
    indev = lv.indev_get_act()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.LEFT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        

    indev = lv.indev_get_act()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.RIGHT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        

screen_7.add_event_cb(lambda e: screen_7_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        #Write animation: screen_7_cont_1 x
        screen_7_cont_1_anim_x = lv.anim_t()
        screen_7_cont_1_anim_x.init()
        screen_7_cont_1_anim_x.set_var(screen_7_cont_1)
        screen_7_cont_1_anim_x.set_time(0)
        screen_7_cont_1_anim_x.set_delay(0)
        screen_7_cont_1_anim_x.set_custom_exec_cb(lambda e,val: anim_x_cb(screen_7_cont_1,val))
        screen_7_cont_1_anim_x.set_values(screen_7_cont_1.get_x(), 364)
        screen_7_cont_1_anim_x.set_path_cb(lv.anim_t.path_step)
        screen_7_cont_1_anim_x.set_repeat_count(0)
        screen_7_cont_1_anim_x.set_repeat_delay(0)
        screen_7_cont_1_anim_x.set_playback_time(0)
        screen_7_cont_1_anim_x.set_playback_delay(0)
        screen_7_cont_1_anim_x.start()
        screen_7_cont_5.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_4.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_3.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_2.clear_flag(lv.obj.FLAG.HIDDEN)
        
screen_7_btn_7.add_event_cb(lambda e: screen_7_btn_7_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_8_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        #Write animation: screen_7_cont_1 x
        screen_7_cont_1_anim_x = lv.anim_t()
        screen_7_cont_1_anim_x.init()
        screen_7_cont_1_anim_x.set_var(screen_7_cont_1)
        screen_7_cont_1_anim_x.set_time(0)
        screen_7_cont_1_anim_x.set_delay(0)
        screen_7_cont_1_anim_x.set_custom_exec_cb(lambda e,val: anim_x_cb(screen_7_cont_1,val))
        screen_7_cont_1_anim_x.set_values(screen_7_cont_1.get_x(), 517)
        screen_7_cont_1_anim_x.set_path_cb(lv.anim_t.path_step)
        screen_7_cont_1_anim_x.set_repeat_count(0)
        screen_7_cont_1_anim_x.set_repeat_delay(0)
        screen_7_cont_1_anim_x.set_playback_time(0)
        screen_7_cont_1_anim_x.set_playback_delay(0)
        screen_7_cont_1_anim_x.start()
        screen_7_cont_5.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_4.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_3.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
screen_7_btn_8.add_event_cb(lambda e: screen_7_btn_8_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        #Write animation: screen_7_cont_1 x
        screen_7_cont_1_anim_x = lv.anim_t()
        screen_7_cont_1_anim_x.init()
        screen_7_cont_1_anim_x.set_var(screen_7_cont_1)
        screen_7_cont_1_anim_x.set_time(0)
        screen_7_cont_1_anim_x.set_delay(0)
        screen_7_cont_1_anim_x.set_custom_exec_cb(lambda e,val: anim_x_cb(screen_7_cont_1,val))
        screen_7_cont_1_anim_x.set_values(screen_7_cont_1.get_x(), 675)
        screen_7_cont_1_anim_x.set_path_cb(lv.anim_t.path_step)
        screen_7_cont_1_anim_x.set_repeat_count(0)
        screen_7_cont_1_anim_x.set_repeat_delay(0)
        screen_7_cont_1_anim_x.set_playback_time(0)
        screen_7_cont_1_anim_x.set_playback_delay(0)
        screen_7_cont_1_anim_x.start()
        screen_7_cont_5.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_4.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_3.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
screen_7_btn_9.add_event_cb(lambda e: screen_7_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_10_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        #Write animation: screen_7_cont_1 x
        screen_7_cont_1_anim_x = lv.anim_t()
        screen_7_cont_1_anim_x.init()
        screen_7_cont_1_anim_x.set_var(screen_7_cont_1)
        screen_7_cont_1_anim_x.set_time(0)
        screen_7_cont_1_anim_x.set_delay(0)
        screen_7_cont_1_anim_x.set_custom_exec_cb(lambda e,val: anim_x_cb(screen_7_cont_1,val))
        screen_7_cont_1_anim_x.set_values(screen_7_cont_1.get_x(), 837)
        screen_7_cont_1_anim_x.set_path_cb(lv.anim_t.path_step)
        screen_7_cont_1_anim_x.set_repeat_count(0)
        screen_7_cont_1_anim_x.set_repeat_delay(0)
        screen_7_cont_1_anim_x.set_playback_time(0)
        screen_7_cont_1_anim_x.set_playback_delay(0)
        screen_7_cont_1_anim_x.start()
        screen_7_cont_5.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_4.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_3.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_7_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
screen_7_btn_10.add_event_cb(lambda e: screen_7_btn_10_event_handler(e), lv.EVENT.ALL, None)

def screen_7_cont_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
screen_7_cont_3.add_event_cb(lambda e: screen_7_cont_3_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_12_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_7_btn_12.add_event_cb(lambda e: screen_7_btn_12_event_handler(e), lv.EVENT.ALL, None)

def screen_7_btn_11_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_4, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_7_btn_11.add_event_cb(lambda e: screen_7_btn_11_event_handler(e), lv.EVENT.ALL, None)

def screen_8_event_handler(e):
    code = e.get_code()
    indev = lv.indev_get_act()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.LEFT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        

    indev = lv.indev_get_act()
    gestureDir = lv.DIR.NONE
    if indev is not None: gestureDir = indev.get_gesture_dir()
    if (code == lv.EVENT.GESTURE and lv.DIR.RIGHT == gestureDir):
        if indev is not None: indev.wait_release()
        pass
        

    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_8.add_event_cb(lambda e: screen_8_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_7.add_event_cb(lambda e: screen_8_btn_7_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_8_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_8.add_event_cb(lambda e: screen_8_btn_8_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_9.add_event_cb(lambda e: screen_8_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_10_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_10.add_event_cb(lambda e: screen_8_btn_10_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_11_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_4, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_11.add_event_cb(lambda e: screen_8_btn_11_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_19_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_5, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_8_btn_19.add_event_cb(lambda e: screen_8_btn_19_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_18_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_9, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_18.add_event_cb(lambda e: screen_8_btn_18_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_17_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_11, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_17.add_event_cb(lambda e: screen_8_btn_17_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_16_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_13, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_16.add_event_cb(lambda e: screen_8_btn_16_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_15_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_6, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_15.add_event_cb(lambda e: screen_8_btn_15_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_14_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_14.add_event_cb(lambda e: screen_8_btn_14_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_13_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_3, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_8_btn_13.add_event_cb(lambda e: screen_8_btn_13_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_12_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_10, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_12.add_event_cb(lambda e: screen_8_btn_12_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_30_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_2, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_30.add_event_cb(lambda e: screen_8_btn_30_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_24_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_24.add_event_cb(lambda e: screen_8_btn_24_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_23_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_10, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_23.add_event_cb(lambda e: screen_8_btn_23_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_28_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_12, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_28.add_event_cb(lambda e: screen_8_btn_28_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_20_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_20.add_event_cb(lambda e: screen_8_btn_20_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_21_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

screen_8_btn_21.add_event_cb(lambda e: screen_8_btn_21_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_22_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.scr_load_anim(screen_2, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_8_btn_22.add_event_cb(lambda e: screen_8_btn_22_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_25_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_25.add_event_cb(lambda e: screen_8_btn_25_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_26_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_7, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_8_btn_26.add_event_cb(lambda e: screen_8_btn_26_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_27_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_27.add_event_cb(lambda e: screen_8_btn_27_event_handler(e), lv.EVENT.ALL, None)

def screen_8_btn_29_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_btn_29.add_event_cb(lambda e: screen_8_btn_29_event_handler(e), lv.EVENT.ALL, None)

def screen_8_slider_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_8_slider_1.add_event_cb(lambda e: screen_8_slider_1_event_handler(e), lv.EVENT.ALL, None)

def screen_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_9.add_event_cb(lambda e: screen_9_event_handler(e), lv.EVENT.ALL, None)

def screen_9_btn_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 0, 0, False)
screen_9_btn_5.add_event_cb(lambda e: screen_9_btn_5_event_handler(e), lv.EVENT.ALL, None)

def screen_9_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_9_btn_6.add_event_cb(lambda e: screen_9_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3.add_event_cb(lambda e: screen_3_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_14_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_14.add_event_cb(lambda e: screen_3_btn_14_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_13_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_13.add_event_cb(lambda e: screen_3_btn_13_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_12_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_12.add_event_cb(lambda e: screen_3_btn_12_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_11_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_11.add_event_cb(lambda e: screen_3_btn_11_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_9.add_event_cb(lambda e: screen_3_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_4.add_event_cb(lambda e: screen_3_btn_4_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_3_btn_3.add_event_cb(lambda e: screen_3_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_3_btn_6.add_event_cb(lambda e: screen_3_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_15_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        #Write animation: screen_3_cont_6 x
        screen_3_cont_6_anim_x = lv.anim_t()
        screen_3_cont_6_anim_x.init()
        screen_3_cont_6_anim_x.set_var(screen_3_cont_6)
        screen_3_cont_6_anim_x.set_time(1000)
        screen_3_cont_6_anim_x.set_delay(0)
        screen_3_cont_6_anim_x.set_custom_exec_cb(lambda e,val: anim_x_cb(screen_3_cont_6,val))
        screen_3_cont_6_anim_x.set_values(screen_3_cont_6.get_x(), 27)
        screen_3_cont_6_anim_x.set_path_cb(lv.anim_t.path_bounce)
        screen_3_cont_6_anim_x.set_repeat_count(0)
        screen_3_cont_6_anim_x.set_repeat_delay(0)
        screen_3_cont_6_anim_x.set_playback_time(0)
        screen_3_cont_6_anim_x.set_playback_delay(0)
        screen_3_cont_6_anim_x.start()
screen_3_btn_15.add_event_cb(lambda e: screen_3_btn_15_event_handler(e), lv.EVENT.ALL, None)

def screen_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_5.add_event_cb(lambda e: screen_5_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_3.add_event_cb(lambda e: screen_5_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_4.add_event_cb(lambda e: screen_5_btn_4_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_5_btn_6.add_event_cb(lambda e: screen_5_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_7.add_event_cb(lambda e: screen_5_btn_7_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_8_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_8.add_event_cb(lambda e: screen_5_btn_8_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_9.add_event_cb(lambda e: screen_5_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_10_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_10.add_event_cb(lambda e: screen_5_btn_10_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_11_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_btn_11.add_event_cb(lambda e: screen_5_btn_11_event_handler(e), lv.EVENT.ALL, None)

def screen_5_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_5_1.add_event_cb(lambda e: screen_5_1_event_handler(e), lv.EVENT.ALL, None)

def screen_5_1_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_5, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_5_1_btn_6.add_event_cb(lambda e: screen_5_1_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_5_1_btn_8_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_1_btn_8.add_event_cb(lambda e: screen_5_1_btn_8_event_handler(e), lv.EVENT.ALL, None)

def screen_5_1_btn_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_5_1_btn_7.add_event_cb(lambda e: screen_5_1_btn_7_event_handler(e), lv.EVENT.ALL, None)

def screen_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_6.add_event_cb(lambda e: screen_6_event_handler(e), lv.EVENT.ALL, None)

def screen_6_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_6_btn_1.add_event_cb(lambda e: screen_6_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_6_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        screen_6_cont_2.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_6_cont_1.add_flag(lv.obj.FLAG.HIDDEN)
        
screen_6_btn_2.add_event_cb(lambda e: screen_6_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_6_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        screen_6_cont_2.add_flag(lv.obj.FLAG.HIDDEN)
        
        screen_6_cont_1.clear_flag(lv.obj.FLAG.HIDDEN)
        
screen_6_btn_3.add_event_cb(lambda e: screen_6_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_10_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_10.add_event_cb(lambda e: screen_10_event_handler(e), lv.EVENT.ALL, None)

def screen_10_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_10_btn_1.add_event_cb(lambda e: screen_10_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_11_btn_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_11_btn_5.add_event_cb(lambda e: screen_11_btn_5_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_7_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_12_btn_7.add_event_cb(lambda e: screen_12_btn_7_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_12_btn_6.add_event_cb(lambda e: screen_12_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_12_btn_4.add_event_cb(lambda e: screen_12_btn_4_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8_1, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_12_btn_3.add_event_cb(lambda e: screen_12_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_12_btn_2.add_event_cb(lambda e: screen_12_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_12_btn_1.add_event_cb(lambda e: screen_12_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_12_slider_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

screen_12_slider_1.add_event_cb(lambda e: screen_12_slider_1_event_handler(e), lv.EVENT.ALL, None)

def screen_12_btn_9_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8_1, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_12_btn_9.add_event_cb(lambda e: screen_12_btn_9_event_handler(e), lv.EVENT.ALL, None)

def screen_13_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_13_btn_1.add_event_cb(lambda e: screen_13_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1.add_event_cb(lambda e: screen_8_1_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_20_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_btn_20.add_event_cb(lambda e: screen_8_1_btn_20_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_19_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_btn_19.add_event_cb(lambda e: screen_8_1_btn_19_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_18_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_btn_18.add_event_cb(lambda e: screen_8_1_btn_18_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_17_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_btn_17.add_event_cb(lambda e: screen_8_1_btn_17_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_15_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_btn_15.add_event_cb(lambda e: screen_8_1_btn_15_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
        

screen_8_1_btn_6.add_event_cb(lambda e: screen_8_1_btn_6_event_handler(e), lv.EVENT.ALL, None)

def screen_8_1_cont_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_8_1_cont_3.add_event_cb(lambda e: screen_8_1_cont_3_event_handler(e), lv.EVENT.ALL, None)

def screen_14_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_14.add_event_cb(lambda e: screen_14_event_handler(e), lv.EVENT.ALL, None)

def screen_14_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_8, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_14_btn_1.add_event_cb(lambda e: screen_14_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_6_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_6_1.add_event_cb(lambda e: screen_6_1_event_handler(e), lv.EVENT.ALL, None)

def screen_6_1_btn_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        lv.scr_load_anim(screen_6, lv.SCR_LOAD_ANIM.NONE, 200, 200, False)
screen_6_1_btn_6.add_event_cb(lambda e: screen_6_1_btn_6_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.scr_load(screen_8_1)

while SDL.check():
    time.sleep_ms(5)

