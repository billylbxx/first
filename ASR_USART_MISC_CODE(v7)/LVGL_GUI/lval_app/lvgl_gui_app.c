#include "lvgl_app.h"
#include "app_task.h"
static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_ON);
}
static void event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj= lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_white();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(&txt_area, dsc->clip_area, &label_dsc, buf, NULL);
}

static void event_cb1(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj= lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[12];
    lv_snprintf(buf, sizeof(buf), "%02d:%02d/%02d:%02d", wavctrl.cursec/60, wavctrl.cursec%60, wavctrl.totsec/60, wavctrl.totsec%60);

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_white();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(&txt_area, dsc->clip_area, &label_dsc, buf, NULL);
}


void LVGL_GUI_APP(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	
	//Write codes screen_tileview
	ui->screen_tileview = lv_tileview_create(ui->screen);
	
	//Write style LV_TILEVIEW_PART_BG for screen_tileview
	static lv_style_t style_screen_tileview_bg;
	lv_style_init(&style_screen_tileview_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_tileview_bg
	lv_style_set_radius(&style_screen_tileview_bg, 0);
	lv_style_set_bg_color(&style_screen_tileview_bg, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_style_set_bg_grad_color(&style_screen_tileview_bg, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_tileview_bg, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_tileview_bg, 255);
	lv_obj_add_style(ui->screen_tileview, &style_screen_tileview_bg, LV_PART_MAIN);

	//Write style LV_TILEVIEW_PART_EDGE_FLASH for screen_tileview
	static lv_style_t style_screen_tileview_edge_flash;
	lv_style_init(&style_screen_tileview_edge_flash);
	
	//Write style state: LV_STATE_DEFAULT for style_screen_tileview_edge_flash
	lv_style_set_radius(&style_screen_tileview_edge_flash, 0);
	lv_style_set_bg_color(&style_screen_tileview_edge_flash, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_color(&style_screen_tileview_edge_flash, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_dir(&style_screen_tileview_edge_flash, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_tileview_edge_flash, 255);
	lv_obj_add_style(ui->screen_tileview, &style_screen_tileview_edge_flash, LV_PART_MAIN);

	//Write style LV_TILEVIEW_PART_SCROLLBAR for screen_tileview
	static lv_style_t style_screen_tileview_scrollbar;
	lv_style_init(&style_screen_tileview_scrollbar);
	
	//Write style state: LV_STATE_DEFAULT for style_screen_tileview_scrollbar
	lv_style_set_radius(&style_screen_tileview_scrollbar, 0);
	lv_style_set_bg_color(&style_screen_tileview_scrollbar, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_color(&style_screen_tileview_scrollbar, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_dir(&style_screen_tileview_scrollbar, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_tileview_scrollbar, 255);
	lv_obj_add_style(ui->screen_tileview, &style_screen_tileview_scrollbar, LV_PART_MAIN);
	lv_obj_set_pos(ui->screen_tileview, 0, 0);
	lv_obj_set_size(ui->screen_tileview, 240, 240);
	
	//Write codes screen_tileview_tileview
	ui->screen_tileview_tileview = lv_obj_create(ui->screen_tileview);
	lv_obj_set_pos(ui->screen_tileview_tileview, 0*240, 0);
	lv_obj_set_size(ui->screen_tileview_tileview, 240, 240);

//	//Write codes screen_labSong
//	ui->screen_labSong = lv_label_create(ui->screen_tileview_tileview);
//	lv_label_set_text(ui->screen_labSong, "Singer - Song Name");
//	lv_label_set_long_mode(ui->screen_labSong, LV_LABEL_LONG_WRAP);
//	lv_obj_align(ui->screen_labSong, LV_ALIGN_BOTTOM_LEFT, -10, -20);
	
	//Write codes screen_labTitle
	ui->screen_labTitle = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labTitle, "Intelligent life");
	lv_label_set_long_mode(ui->screen_labTitle, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_labTitle, LV_ALIGN_CENTER, 0, -105);

	//Write codes screen_labState
	ui->screen_labState = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labState, "Network state");
	lv_label_set_long_mode(ui->screen_labState, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->screen_labState, LV_ALIGN_BOTTOM_LEFT, -10, 10);
	
	static lv_style_t style_bg;	
	lv_style_init(&style_bg);	//初始化样式
	lv_style_set_anim_time(&style_bg, 1000);	//设置动画样式，1000ms的过渡时间

	static lv_style_t style_indic;
	lv_style_init(&style_indic);	//初始化样式
	lv_style_set_bg_opa(&style_indic, LV_OPA_50);	//透明度50
	lv_style_set_bg_color(&style_indic, lv_color_make(0x01, 0xa2, 0xb1));	//主题颜色
	lv_style_set_bg_grad_color(&style_indic, lv_color_make(0xff, 0x00, 0x00));	//混合颜色
	lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_HOR);	//样式行走方向

	ui->screen_barTemp = lv_bar_create(ui->screen_tileview_tileview);
	lv_obj_add_event_cb(ui->screen_barTemp, event_cb, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barTemp, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barTemp, &style_bg, 0);
	lv_obj_set_size(ui->screen_barTemp, 70, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barTemp, 50, 20);		//显示位置
	lv_bar_set_range(ui->screen_barTemp, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_barHum = lv_bar_create(ui->screen_tileview_tileview);	//创建进度条
	lv_obj_add_event_cb(ui->screen_barHum, event_cb, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barHum, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barHum, &style_bg, 0);
	lv_obj_set_size(ui->screen_barHum, 70, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barHum, 50, 50);		//显示位置
	lv_bar_set_range(ui->screen_barHum, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_barLight = lv_bar_create(ui->screen_tileview_tileview);	//创建进度条
	lv_obj_add_event_cb(ui->screen_barLight, event_cb, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barLight, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barLight, &style_bg, 0);
	lv_obj_set_size(ui->screen_barLight, 70, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barLight, 50, 80);		//显示位置
	lv_bar_set_range(ui->screen_barLight, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_barFlame = lv_bar_create(ui->screen_tileview_tileview);	//创建进度条
	lv_obj_add_event_cb(ui->screen_barFlame, event_cb, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barFlame, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barFlame, &style_bg, 0);
	lv_obj_set_size(ui->screen_barFlame, 70, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barFlame, 50, 110);		//显示位置
	lv_bar_set_range(ui->screen_barFlame, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_barMusic = lv_bar_create(ui->screen_tileview_tileview);	//创建进度条
	lv_obj_add_event_cb(ui->screen_barMusic, event_cb1, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barMusic, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barMusic, &style_bg, 0);
	lv_obj_set_size(ui->screen_barMusic, 160, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barMusic, -5, 140);		//显示位置
	lv_bar_set_range(ui->screen_barMusic, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_barState = lv_bar_create(ui->screen_tileview_tileview);	//创建进度条
	lv_obj_add_event_cb(ui->screen_barState, event_cb, LV_EVENT_DRAW_PART_END, NULL);	//添加事件回调，显示进度值
	lv_obj_add_style(ui->screen_barState, &style_indic, LV_PART_INDICATOR);	//设置指示器的样式
	lv_obj_add_style(ui->screen_barState, &style_bg, 0);
	lv_obj_set_size(ui->screen_barState, 90, 20);	//w>h为水平进度条，反之为垂直进度条
	lv_obj_set_pos(ui->screen_barState, 115, 200);		//显示位置
	lv_bar_set_range(ui->screen_barState, 0, 100);	//修改进度条范围，默认值0-100
	
	ui->screen_labTemp = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labTemp, "Temp");
	lv_label_set_long_mode(ui->screen_labTemp, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_labTemp, -5, 20);
	
	ui->screen_labTemp = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labTemp, "Hum");
	lv_label_set_long_mode(ui->screen_labTemp, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_labTemp, -5, 50);
	
	ui->screen_labTemp = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labTemp, "Light");
	lv_label_set_long_mode(ui->screen_labTemp, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_labTemp, -5, 80);
	
	ui->screen_labTemp = lv_label_create(ui->screen_tileview_tileview);
	lv_label_set_text(ui->screen_labTemp, "Body");
	lv_label_set_long_mode(ui->screen_labTemp, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_labTemp, -5, 110);
	
	ui->screen_ledAlarms = lv_led_create(ui->screen_tileview_tileview);
	lv_led_set_color(ui->screen_ledAlarms, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_obj_set_pos(ui->screen_ledAlarms, 185, 10);
	lv_obj_set_size(ui->screen_ledAlarms, 20, 20);
	lv_led_off(ui->screen_ledAlarms);
	
	ui->screen_ledFinestra = lv_led_create(ui->screen_tileview_tileview);
	lv_led_set_color(ui->screen_ledFinestra, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_obj_set_pos(ui->screen_ledFinestra, 185, 45);
	lv_obj_set_size(ui->screen_ledFinestra, 20, 20);
	lv_led_off(ui->screen_ledFinestra);
	
	ui->screen_ledBedroom = lv_led_create(ui->screen_tileview_tileview);
	lv_led_set_color(ui->screen_ledBedroom, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_obj_set_pos(ui->screen_ledBedroom, 185, 80);
	lv_obj_set_size(ui->screen_ledBedroom, 20, 20);
	lv_led_off(ui->screen_ledBedroom);
	
	ui->screen_ledKitchen = lv_led_create(ui->screen_tileview_tileview);
	lv_led_set_color(ui->screen_ledKitchen, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_obj_set_pos(ui->screen_ledKitchen, 185, 115);
	lv_obj_set_size(ui->screen_ledKitchen, 20, 20);
	lv_led_off(ui->screen_ledKitchen);
	
	ui->screen_ledSaloon = lv_led_create(ui->screen_tileview_tileview);
	lv_led_set_color(ui->screen_ledSaloon, lv_color_make(0xbf, 0xe5, 0xfd));
	lv_obj_set_pos(ui->screen_ledSaloon, 185, 150);
	lv_obj_set_size(ui->screen_ledSaloon, 20, 20);
	lv_led_off(ui->screen_ledSaloon);
	
	static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 5);
    lv_style_set_line_color(&style_line, lv_color_make(0xbf, 0xe5, 0xfd));
    lv_style_set_line_rounded(&style_line, true);
	
	ui->screen_line_x = lv_line_create(ui->screen_tileview_tileview);
	lv_obj_set_pos(ui->screen_line_x, 130, 110);
	lv_obj_set_size(ui->screen_line_x, 50, 50);
	static lv_point_t screen_line_x[] ={{0, 0}, {50, 50}};
	lv_line_set_points(ui->screen_line_x,screen_line_x, 2);
	lv_obj_add_style(ui->screen_line_x, &style_line, 0);
	
	static lv_style_t style_arc;
	lv_style_set_arc_width(&style_arc, 10);
	ui->screen_cpicker_color = lv_colorwheel_create(ui->screen_tileview_tileview, true);
	lv_obj_add_style(ui->screen_cpicker_color, &style_arc, LV_PART_MAIN);
	lv_obj_set_pos(ui->screen_cpicker_color, 130, 40);
	lv_obj_set_size(ui->screen_cpicker_color, 50, 50);

	lv_scr_load(ui->screen);
}

