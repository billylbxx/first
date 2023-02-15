/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "../lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "ADC.h"


static void Rtc_Updata_cb(lv_timer_t * tmr)
{
    static RTC_TimeTypeDef RTC_TimeStruct, RTC_TimeStructLog;
    
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
//    
//    if(RTC_TimeStruct.RTC_Hours != RTC_TimeStructLog.RTC_Hours)
//    {
//        lv_meter_set_indicator_end_value(guider_ui.screen_meter_Clock, screen_meter_Clock_scale_Hour_ndline_0, RTC_TimeStruct.RTC_Hours%12);
//        RTC_TimeStructLog.RTC_Hours = RTC_TimeStruct.RTC_Hours;
//    }
//    if(RTC_TimeStruct.RTC_Minutes != RTC_TimeStructLog.RTC_Minutes)
//    {
//        lv_meter_set_indicator_end_value(guider_ui.screen_meter_Clock, screen_meter_Clock_scale_Min_ndline_0, RTC_TimeStruct.RTC_Minutes);
//        RTC_TimeStructLog.RTC_Minutes = RTC_TimeStruct.RTC_Minutes;
//    }
//    if(RTC_TimeStruct.RTC_Seconds != RTC_TimeStructLog.RTC_Seconds)
//    {
//        lv_meter_set_indicator_end_value(guider_ui.screen_meter_Clock, screen_meter_Clock_scale_Sec_ndline_0, RTC_TimeStruct.RTC_Seconds);
//        RTC_TimeStructLog.RTC_Seconds = RTC_TimeStruct.RTC_Seconds;
//    }
    char rtcString[16] = {0};
    sprintf(rtcString, "%02d:%02d:%02d", RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
    printf("%s\r\n",rtcString);
    lv_label_set_text(guider_ui.screen_label_4, rtcString);
}

static void set_value(void* indic, int32_t v)
{
    unsigned int AdcValue = Get_value(ADC_Channel_10);
    
    lv_meter_set_indicator_value(guider_ui.screen_meter_1, indic, AdcValue*100/4095);  // 设置表头指示值
}

void setup_scr_screen(lv_ui *ui){

	//Write codes screen
	ui->screen = lv_obj_create(NULL);

	//Write style state: LV_STATE_DEFAULT for style_screen_main_main_default
	static lv_style_t style_screen_main_main_default;
	lv_style_reset(&style_screen_main_main_default);
	lv_style_set_bg_color(&style_screen_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_opa(&style_screen_main_main_default, 0);
	lv_obj_add_style(ui->screen, &style_screen_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_tabview_1
	ui->screen_tabview_1 = lv_tabview_create(ui->screen, LV_DIR_TOP, 38);
	lv_obj_set_pos(ui->screen_tabview_1, 0, 0);
	lv_obj_set_size(ui->screen_tabview_1, 240, 240);

	//title1
	ui->screen_tabview_1_title1 = lv_tabview_add_tab(ui->screen_tabview_1,"title1");

	//title2
	ui->screen_tabview_1_title2 = lv_tabview_add_tab(ui->screen_tabview_1,"title2");

	//title3
	ui->screen_tabview_1_title3 = lv_tabview_add_tab(ui->screen_tabview_1,"title3");


	//Write style state: LV_STATE_DEFAULT for style_screen_tabview_1_main_main_default
	static lv_style_t style_screen_tabview_1_main_main_default;
	lv_style_reset(&style_screen_tabview_1_main_main_default);
	lv_style_set_bg_color(&style_screen_tabview_1_main_main_default, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_color(&style_screen_tabview_1_main_main_default, lv_color_make(0xea, 0xef, 0xf3));
	lv_style_set_bg_grad_dir(&style_screen_tabview_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_tabview_1_main_main_default, 255);
	lv_style_set_border_color(&style_screen_tabview_1_main_main_default, lv_color_make(0xc0, 0xc0, 0xc0));
	lv_style_set_border_width(&style_screen_tabview_1_main_main_default, 0);
	lv_style_set_border_opa(&style_screen_tabview_1_main_main_default, 100);
	lv_style_set_text_color(&style_screen_tabview_1_main_main_default, lv_color_make(0x4d, 0x4d, 0x4d));
	lv_style_set_text_font(&style_screen_tabview_1_main_main_default, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_tabview_1_main_main_default, 2);
	lv_style_set_text_line_space(&style_screen_tabview_1_main_main_default, 16);
	lv_obj_add_style(ui->screen_tabview_1, &style_screen_tabview_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_tabview_1_extra_btnm_main_default
	static lv_style_t style_screen_tabview_1_extra_btnm_main_default;
	lv_style_reset(&style_screen_tabview_1_extra_btnm_main_default);
	lv_style_set_bg_color(&style_screen_tabview_1_extra_btnm_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_tabview_1_extra_btnm_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_tabview_1_extra_btnm_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_tabview_1_extra_btnm_main_default, 255);
	lv_style_set_border_color(&style_screen_tabview_1_extra_btnm_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_tabview_1_extra_btnm_main_default, 0);
	lv_style_set_border_opa(&style_screen_tabview_1_extra_btnm_main_default, 100);
	lv_style_set_pad_left(&style_screen_tabview_1_extra_btnm_main_default, 0);
	lv_style_set_pad_right(&style_screen_tabview_1_extra_btnm_main_default, 0);
	lv_style_set_pad_top(&style_screen_tabview_1_extra_btnm_main_default, 0);
	lv_style_set_pad_bottom(&style_screen_tabview_1_extra_btnm_main_default, 0);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_tabview_1), &style_screen_tabview_1_extra_btnm_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_tabview_1_extra_btnm_items_default
	static lv_style_t style_screen_tabview_1_extra_btnm_items_default;
	lv_style_reset(&style_screen_tabview_1_extra_btnm_items_default);
	lv_style_set_text_color(&style_screen_tabview_1_extra_btnm_items_default, lv_color_make(0x4d, 0x4d, 0x4d));
	lv_style_set_text_font(&style_screen_tabview_1_extra_btnm_items_default, &lv_font_simsun_12);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_tabview_1), &style_screen_tabview_1_extra_btnm_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_CHECKED for style_screen_tabview_1_extra_btnm_items_checked
	static lv_style_t style_screen_tabview_1_extra_btnm_items_checked;
	lv_style_reset(&style_screen_tabview_1_extra_btnm_items_checked);
	lv_style_set_bg_color(&style_screen_tabview_1_extra_btnm_items_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_tabview_1_extra_btnm_items_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_opa(&style_screen_tabview_1_extra_btnm_items_checked, 60);
	lv_style_set_border_color(&style_screen_tabview_1_extra_btnm_items_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_tabview_1_extra_btnm_items_checked, 4);
	lv_style_set_border_opa(&style_screen_tabview_1_extra_btnm_items_checked, 255);
	lv_style_set_border_side(&style_screen_tabview_1_extra_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_text_color(&style_screen_tabview_1_extra_btnm_items_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_text_font(&style_screen_tabview_1_extra_btnm_items_checked, &lv_font_simsun_12);
	lv_obj_add_style(lv_tabview_get_tab_btns(ui->screen_tabview_1), &style_screen_tabview_1_extra_btnm_items_checked, LV_PART_ITEMS|LV_STATE_CHECKED);

	//Write codes screen_sw_1
	ui->screen_sw_1 = lv_switch_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_sw_1, 170, 18);
	lv_obj_set_size(ui->screen_sw_1, 40, 20);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_1_main_main_default
	static lv_style_t style_screen_sw_1_main_main_default;
	lv_style_reset(&style_screen_sw_1_main_main_default);
	lv_style_set_radius(&style_screen_sw_1_main_main_default, 100);
	lv_style_set_bg_color(&style_screen_sw_1_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_color(&style_screen_sw_1_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_dir(&style_screen_sw_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_1_main_main_default, 255);
	lv_obj_add_style(ui->screen_sw_1, &style_screen_sw_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_CHECKED for style_screen_sw_1_main_indicator_checked
	static lv_style_t style_screen_sw_1_main_indicator_checked;
	lv_style_reset(&style_screen_sw_1_main_indicator_checked);
	lv_style_set_radius(&style_screen_sw_1_main_indicator_checked, 100);
	lv_style_set_bg_color(&style_screen_sw_1_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_sw_1_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_sw_1_main_indicator_checked, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_1_main_indicator_checked, 255);
	lv_obj_add_style(ui->screen_sw_1, &style_screen_sw_1_main_indicator_checked, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_1_main_knob_default
	static lv_style_t style_screen_sw_1_main_knob_default;
	lv_style_reset(&style_screen_sw_1_main_knob_default);
	lv_style_set_radius(&style_screen_sw_1_main_knob_default, 100);
	lv_style_set_bg_color(&style_screen_sw_1_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_sw_1_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_sw_1_main_knob_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_1_main_knob_default, 255);
	lv_obj_add_style(ui->screen_sw_1, &style_screen_sw_1_main_knob_default, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_cpicker_1
	ui->screen_cpicker_1 = lv_colorwheel_create(ui->screen_tabview_1_title1, true);
	lv_obj_set_pos(ui->screen_cpicker_1, 80, 106);
	lv_obj_set_size(ui->screen_cpicker_1, 60, 60);

	//Write style state: LV_STATE_DEFAULT for style_screen_cpicker_1_main_main_default
	static lv_style_t style_screen_cpicker_1_main_main_default;
	lv_style_reset(&style_screen_cpicker_1_main_main_default);
	lv_style_set_arc_width(&style_screen_cpicker_1_main_main_default, 10);
	lv_obj_add_style(ui->screen_cpicker_1, &style_screen_cpicker_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_cb_3
	ui->screen_cb_3 = lv_checkbox_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_cb_3, 20, 80);
	lv_checkbox_set_text(ui->screen_cb_3, "LED3");

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_3_main_main_default
	static lv_style_t style_screen_cb_3_main_main_default;
	lv_style_reset(&style_screen_cb_3_main_main_default);
	lv_style_set_radius(&style_screen_cb_3_main_main_default, 6);
	lv_style_set_bg_color(&style_screen_cb_3_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_3_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_3_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_3_main_main_default, 0);
	lv_style_set_text_color(&style_screen_cb_3_main_main_default, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_screen_cb_3_main_main_default, &lv_font_simsun_16);
	lv_style_set_text_letter_space(&style_screen_cb_3_main_main_default, 0);
	lv_obj_add_style(ui->screen_cb_3, &style_screen_cb_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_3_main_indicator_default
	static lv_style_t style_screen_cb_3_main_indicator_default;
	lv_style_reset(&style_screen_cb_3_main_indicator_default);
	lv_style_set_radius(&style_screen_cb_3_main_indicator_default, 6);
	lv_style_set_bg_color(&style_screen_cb_3_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_3_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_3_main_indicator_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_3_main_indicator_default, 255);
	lv_style_set_border_color(&style_screen_cb_3_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_cb_3_main_indicator_default, 2);
	lv_obj_add_style(ui->screen_cb_3, &style_screen_cb_3_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_cb_2
	ui->screen_cb_2 = lv_checkbox_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_cb_2, 20, 50);
	lv_checkbox_set_text(ui->screen_cb_2, "LED2");

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_2_main_main_default
	static lv_style_t style_screen_cb_2_main_main_default;
	lv_style_reset(&style_screen_cb_2_main_main_default);
	lv_style_set_radius(&style_screen_cb_2_main_main_default, 6);
	lv_style_set_bg_color(&style_screen_cb_2_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_2_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_2_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_2_main_main_default, 0);
	lv_style_set_text_color(&style_screen_cb_2_main_main_default, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_screen_cb_2_main_main_default, &lv_font_simsun_16);
	lv_style_set_text_letter_space(&style_screen_cb_2_main_main_default, 0);
	lv_obj_add_style(ui->screen_cb_2, &style_screen_cb_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_2_main_indicator_default
	static lv_style_t style_screen_cb_2_main_indicator_default;
	lv_style_reset(&style_screen_cb_2_main_indicator_default);
	lv_style_set_radius(&style_screen_cb_2_main_indicator_default, 6);
	lv_style_set_bg_color(&style_screen_cb_2_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_2_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_2_main_indicator_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_2_main_indicator_default, 255);
	lv_style_set_border_color(&style_screen_cb_2_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_cb_2_main_indicator_default, 2);
	lv_obj_add_style(ui->screen_cb_2, &style_screen_cb_2_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_cb_1
	ui->screen_cb_1 = lv_checkbox_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_cb_1, 20, 20);
	lv_checkbox_set_text(ui->screen_cb_1, "LED1");

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_1_main_main_default
	static lv_style_t style_screen_cb_1_main_main_default;
	lv_style_reset(&style_screen_cb_1_main_main_default);
	lv_style_set_radius(&style_screen_cb_1_main_main_default, 6);
	lv_style_set_bg_color(&style_screen_cb_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_1_main_main_default, 0);
	lv_style_set_text_color(&style_screen_cb_1_main_main_default, lv_color_make(0x0D, 0x30, 0x55));
	lv_style_set_text_font(&style_screen_cb_1_main_main_default, &lv_font_simsun_16);
	lv_style_set_text_letter_space(&style_screen_cb_1_main_main_default, 0);
	lv_obj_add_style(ui->screen_cb_1, &style_screen_cb_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_cb_1_main_indicator_default
	static lv_style_t style_screen_cb_1_main_indicator_default;
	lv_style_reset(&style_screen_cb_1_main_indicator_default);
	lv_style_set_radius(&style_screen_cb_1_main_indicator_default, 6);
	lv_style_set_bg_color(&style_screen_cb_1_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_cb_1_main_indicator_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_cb_1_main_indicator_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_cb_1_main_indicator_default, 255);
	lv_style_set_border_color(&style_screen_cb_1_main_indicator_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_border_width(&style_screen_cb_1_main_indicator_default, 2);
	lv_obj_add_style(ui->screen_cb_1, &style_screen_cb_1_main_indicator_default, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_label_3
	ui->screen_label_3 = lv_label_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_label_3, 135, 85);
	lv_obj_set_size(ui->screen_label_3, 40, 20);
	lv_label_set_text(ui->screen_label_3, "RGB");
	lv_label_set_long_mode(ui->screen_label_3, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->screen_label_3, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_3_main_main_default
	static lv_style_t style_screen_label_3_main_main_default;
	lv_style_reset(&style_screen_label_3_main_main_default);
	lv_style_set_radius(&style_screen_label_3_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_3_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_3_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_3_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_3_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_3_main_main_default, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_label_3_main_main_default, 2);
	lv_style_set_pad_left(&style_screen_label_3_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_3_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_3_main_main_default, 0);
	lv_style_set_pad_bottom(&style_screen_label_3_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_3, &style_screen_label_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_2
	ui->screen_label_2 = lv_label_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_label_2, 123, 54);
	lv_obj_set_size(ui->screen_label_2, 52, 20);
	lv_label_set_text(ui->screen_label_2, "蜂鸣器");
	lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->screen_label_2, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_2_main_main_default
	static lv_style_t style_screen_label_2_main_main_default;
	lv_style_reset(&style_screen_label_2_main_main_default);
	lv_style_set_radius(&style_screen_label_2_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_2_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_2_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_2_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_2_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_2_main_main_default, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_label_2_main_main_default, 2);
	lv_style_set_pad_left(&style_screen_label_2_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_2_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_2_main_main_default, 0);
	lv_style_set_pad_bottom(&style_screen_label_2_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_2, &style_screen_label_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_1
	ui->screen_label_1 = lv_label_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_label_1, 124, 22);
	lv_obj_set_size(ui->screen_label_1, 59, 13);
	lv_label_set_text(ui->screen_label_1, "风扇");
	lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->screen_label_1, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_1_main_main_default
	static lv_style_t style_screen_label_1_main_main_default;
	lv_style_reset(&style_screen_label_1_main_main_default);
	lv_style_set_radius(&style_screen_label_1_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_label_1_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_1_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_1_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_label_1_main_main_default, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_label_1_main_main_default, 2);
	lv_style_set_pad_left(&style_screen_label_1_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_1_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_1_main_main_default, 0);
	lv_style_set_pad_bottom(&style_screen_label_1_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_1, &style_screen_label_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_sw_3
	ui->screen_sw_3 = lv_switch_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_sw_3, 170, 82);
	lv_obj_set_size(ui->screen_sw_3, 40, 20);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_3_main_main_default
	static lv_style_t style_screen_sw_3_main_main_default;
	lv_style_reset(&style_screen_sw_3_main_main_default);
	lv_style_set_radius(&style_screen_sw_3_main_main_default, 100);
	lv_style_set_bg_color(&style_screen_sw_3_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_color(&style_screen_sw_3_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_dir(&style_screen_sw_3_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_3_main_main_default, 255);
	lv_obj_add_style(ui->screen_sw_3, &style_screen_sw_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_CHECKED for style_screen_sw_3_main_indicator_checked
	static lv_style_t style_screen_sw_3_main_indicator_checked;
	lv_style_reset(&style_screen_sw_3_main_indicator_checked);
	lv_style_set_radius(&style_screen_sw_3_main_indicator_checked, 100);
	lv_style_set_bg_color(&style_screen_sw_3_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_sw_3_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_sw_3_main_indicator_checked, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_3_main_indicator_checked, 255);
	lv_obj_add_style(ui->screen_sw_3, &style_screen_sw_3_main_indicator_checked, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_3_main_knob_default
	static lv_style_t style_screen_sw_3_main_knob_default;
	lv_style_reset(&style_screen_sw_3_main_knob_default);
	lv_style_set_radius(&style_screen_sw_3_main_knob_default, 100);
	lv_style_set_bg_color(&style_screen_sw_3_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_sw_3_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_sw_3_main_knob_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_3_main_knob_default, 255);
	lv_obj_add_style(ui->screen_sw_3, &style_screen_sw_3_main_knob_default, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_sw_2
	ui->screen_sw_2 = lv_switch_create(ui->screen_tabview_1_title1);
	lv_obj_set_pos(ui->screen_sw_2, 170, 50);
	lv_obj_set_size(ui->screen_sw_2, 40, 20);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_2_main_main_default
	static lv_style_t style_screen_sw_2_main_main_default;
	lv_style_reset(&style_screen_sw_2_main_main_default);
	lv_style_set_radius(&style_screen_sw_2_main_main_default, 100);
	lv_style_set_bg_color(&style_screen_sw_2_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_color(&style_screen_sw_2_main_main_default, lv_color_make(0xe6, 0xe2, 0xe6));
	lv_style_set_bg_grad_dir(&style_screen_sw_2_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_2_main_main_default, 255);
	lv_obj_add_style(ui->screen_sw_2, &style_screen_sw_2_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_CHECKED for style_screen_sw_2_main_indicator_checked
	static lv_style_t style_screen_sw_2_main_indicator_checked;
	lv_style_reset(&style_screen_sw_2_main_indicator_checked);
	lv_style_set_radius(&style_screen_sw_2_main_indicator_checked, 100);
	lv_style_set_bg_color(&style_screen_sw_2_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_color(&style_screen_sw_2_main_indicator_checked, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_sw_2_main_indicator_checked, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_2_main_indicator_checked, 255);
	lv_obj_add_style(ui->screen_sw_2, &style_screen_sw_2_main_indicator_checked, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style state: LV_STATE_DEFAULT for style_screen_sw_2_main_knob_default
	static lv_style_t style_screen_sw_2_main_knob_default;
	lv_style_reset(&style_screen_sw_2_main_knob_default);
	lv_style_set_radius(&style_screen_sw_2_main_knob_default, 100);
	lv_style_set_bg_color(&style_screen_sw_2_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_sw_2_main_knob_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_sw_2_main_knob_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_sw_2_main_knob_default, 255);
	lv_obj_add_style(ui->screen_sw_2, &style_screen_sw_2_main_knob_default, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes screen_meter_1
	ui->screen_meter_1 = lv_meter_create(ui->screen_tabview_1_title2);
	lv_obj_set_pos(ui->screen_meter_1, 20, 15);
	lv_obj_set_size(ui->screen_meter_1, 167, 167);

	//add scale screen_meter_1_scale_1
	lv_meter_scale_t *screen_meter_1_scale_1 = lv_meter_add_scale(ui->screen_meter_1);
	lv_meter_set_scale_ticks(ui->screen_meter_1, screen_meter_1_scale_1, 41, 2, 10, lv_color_make(0x00, 0xbf, 0xff));
	lv_meter_set_scale_major_ticks(ui->screen_meter_1, screen_meter_1_scale_1, 8, 5, 15, lv_color_make(0xff, 0x00, 0x19), 8);
	lv_meter_set_scale_range(ui->screen_meter_1, screen_meter_1_scale_1, 0, 100, 300, 90);

	//add arc for screen_meter_1_scale_1
	lv_meter_indicator_t *screen_meter_1_scale_1_arc_0;
	screen_meter_1_scale_1_arc_0 = lv_meter_add_arc(ui->screen_meter_1, screen_meter_1_scale_1, 2, lv_color_make(0x11, 0xff, 0x00), 2);
	lv_meter_set_indicator_start_value(ui->screen_meter_1, screen_meter_1_scale_1_arc_0, 0);
	lv_meter_set_indicator_end_value(ui->screen_meter_1, screen_meter_1_scale_1_arc_0, 360);

	//add needle line for screen_meter_1_scale_1
	lv_meter_indicator_t *screen_meter_1_scale_1_ndline_0;
	screen_meter_1_scale_1_ndline_0 = lv_meter_add_needle_line(ui->screen_meter_1, screen_meter_1_scale_1, 2, lv_color_make(0x00, 0x00, 0x00), -24);
	lv_meter_set_indicator_value(ui->screen_meter_1, screen_meter_1_scale_1_ndline_0, 0);

	//Write style state: LV_STATE_DEFAULT for style_screen_meter_1_main_main_default
	static lv_style_t style_screen_meter_1_main_main_default;
	lv_style_reset(&style_screen_meter_1_main_main_default);
	lv_style_set_bg_color(&style_screen_meter_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_meter_1_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_meter_1_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_meter_1_main_main_default, 255);
	lv_obj_add_style(ui->screen_meter_1, &style_screen_meter_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style state: LV_STATE_DEFAULT for style_screen_meter_1_main_ticks_default
	static lv_style_t style_screen_meter_1_main_ticks_default;
	lv_style_reset(&style_screen_meter_1_main_ticks_default);
	lv_style_set_text_color(&style_screen_meter_1_main_ticks_default, lv_color_make(0xff, 0x00, 0x00));
	lv_style_set_text_font(&style_screen_meter_1_main_ticks_default, &lv_font_simsun_12);
	lv_obj_add_style(ui->screen_meter_1, &style_screen_meter_1_main_ticks_default, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Update pos for widget screen_meter_1
	lv_obj_update_layout(ui->screen_meter_1);


    lv_anim_t a;
    lv_anim_init(&a);  
    lv_anim_set_exec_cb(&a, set_value); //关联动画执行函数
    lv_anim_set_var(&a, screen_meter_1_scale_1_ndline_0); // 设置关联变量到动画
    lv_anim_set_values(&a, 0, 100);  // 设置动画起始值0，结束值100
    lv_anim_set_time(&a, 2000);  // 调置动画duration时间
    lv_anim_set_repeat_delay(&a, 100); // 设置重复动画前的延时时间
    lv_anim_set_playback_time(&a, 500); // 设置动画回播时间
    lv_anim_set_playback_delay(&a, 100); // 设置开始回播前的延时时间
	    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 无限循环重复播放 
		lv_anim_start(&a);  //启动动画
		
	//Write codes screen_label_4
	ui->screen_label_4 = lv_label_create(ui->screen_tabview_1_title3);
	lv_obj_set_pos(ui->screen_label_4, 52, 69);
	lv_obj_set_size(ui->screen_label_4, 148, 41);
	lv_label_set_text(ui->screen_label_4, "");
	lv_label_set_long_mode(ui->screen_label_4, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(ui->screen_label_4, LV_TEXT_ALIGN_CENTER, 0);

	//Write style state: LV_STATE_DEFAULT for style_screen_label_4_main_main_default
	static lv_style_t style_screen_label_4_main_main_default;
	lv_style_reset(&style_screen_label_4_main_main_default);
	lv_style_set_radius(&style_screen_label_4_main_main_default, 0);
	lv_style_set_bg_color(&style_screen_label_4_main_main_default, lv_color_make(0x08, 0x8e, 0xfd));
	lv_style_set_bg_grad_color(&style_screen_label_4_main_main_default, lv_color_make(0x21, 0x95, 0xf6));
	lv_style_set_bg_grad_dir(&style_screen_label_4_main_main_default, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label_4_main_main_default, 0);
	lv_style_set_text_color(&style_screen_label_4_main_main_default, lv_color_make(0xfc, 0x03, 0x03));
	lv_style_set_text_letter_space(&style_screen_label_4_main_main_default, 2);
	lv_style_set_pad_left(&style_screen_label_4_main_main_default, 0);
	lv_style_set_pad_right(&style_screen_label_4_main_main_default, 0);
	lv_style_set_pad_top(&style_screen_label_4_main_main_default, 0);
	lv_style_set_pad_bottom(&style_screen_label_4_main_main_default, 0);
	lv_obj_add_style(ui->screen_label_4, &style_screen_label_4_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);


		lv_timer_t * RTC_Task = lv_timer_create(Rtc_Updata_cb, 100, 0);
    lv_timer_set_repeat_count(RTC_Task,-1);

	//Init events for screen
	events_init_screen(ui);
}