/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl/lvgl.h"
#include "guider_fonts.h"

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *screen_tabview_1;
	lv_obj_t *screen_tabview_1_title1;
	lv_obj_t *screen_sw_1;
	lv_obj_t *screen_cpicker_1;
	lv_obj_t *screen_cb_3;
	lv_obj_t *screen_cb_2;
	lv_obj_t *screen_cb_1;
	lv_obj_t *screen_label_3;
	lv_obj_t *screen_label_2;
	lv_obj_t *screen_label_1;
	lv_obj_t *screen_sw_3;
	lv_obj_t *screen_sw_2;
	lv_obj_t *screen_tabview_1_title2;
	lv_obj_t *screen_meter_1;
	lv_obj_t *screen_tabview_1_title3;
	lv_obj_t *screen_label_4;
}lv_ui;

void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_screen(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif