/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */
#include "events_init.h"
#include <stdio.h>
#include "../lvgl.h"
void events_init(lv_ui *ui)
{
}
static void screen_btn_1_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_CLICKED:
	{
		lv_obj_set_style_text_font(guider_ui.screen_ta_1, &lv_font_simsun_16, 0);
		lv_textarea_set_text(guider_ui.screen_ta_1, "按键按下");
	}
		break;
	default:
		break;
	}
}
void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_btn_1, screen_btn_1_event_handler, LV_EVENT_ALL, NULL);
}
