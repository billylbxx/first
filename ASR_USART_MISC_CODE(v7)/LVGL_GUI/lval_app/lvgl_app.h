#ifndef __LVGL_APP_H
#define __LVGL_APP_H

#include "io_bit.h"
#include "lvgl.h"

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *screen_tileview;
	lv_obj_t *screen_tileview_tileview;
	lv_obj_t *screen_labSong;
	lv_obj_t *screen_labTitle;
	lv_obj_t *screen_labState;
	lv_obj_t *screen_barFlame;
	lv_obj_t *screen_barLight;
	lv_obj_t *screen_barHum;
	lv_obj_t *screen_barTemp;
	lv_obj_t *screen_labTemp;
	lv_obj_t *screen_labHum;
	lv_obj_t *screen_labLight;
	lv_obj_t *screen_labFlame;
	lv_obj_t *screen_barState;
	lv_obj_t *screen_ledState;
	lv_obj_t *screen_ledAlarms;
	lv_obj_t *screen_ledFinestra;
	lv_obj_t *screen_barMusic;
	lv_obj_t *screen_labMusic;
	lv_obj_t *screen_ledBedroom;
	lv_obj_t *screen_ledSaloon;
	lv_obj_t *screen_ledKitchen;
	lv_obj_t *screen_line_x;
	lv_obj_t *screen_cpicker_color;
}lv_ui;

extern lv_ui guider_ui;

void LVGL_GUI_APP(lv_ui *ui);


#endif


