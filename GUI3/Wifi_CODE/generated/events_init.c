/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */
#include "events_init.h"
#include <stdio.h>
#include "../lvgl.h"
#include "led.h"
#include "motor.h"
#include "beep.h"
#include "rgb.h"
void events_init(lv_ui *ui)
{
}

static void screen_cb_1_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		if(lv_obj_get_state(guider_ui.screen_cb_1) & LV_STATE_CHECKED)
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_4);  //����LED1
        }
        else GPIO_SetBits(GPIOC, GPIO_Pin_4);   //Ϩ��LED1
	}
		break;
	default:
		break;
	}
}


static void screen_cb_2_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		if(lv_obj_get_state(guider_ui.screen_cb_2) & LV_STATE_CHECKED)
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_5);  //����LED2
        }
        else GPIO_SetBits(GPIOC, GPIO_Pin_5);   //Ϩ��LED2
	}
		break;
	default:
		break;
	}
}

static void screen_cb_3_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		if(lv_obj_get_state(guider_ui.screen_cb_3) & LV_STATE_CHECKED)
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_0);  //����LED3
        }
        else GPIO_SetBits(GPIOB, GPIO_Pin_0);   //Ϩ��LED3
	}
		break;
	default:
		break;
	}
}

static void screen_sw_1_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		if(lv_obj_get_state(guider_ui.screen_sw_1) & LV_STATE_CHECKED)
        {
           MOTOR=1;  //�򿪷���
        }
        else  MOTOR=0;   //�رշ���
	}
		break;
	default:
		break;
	}
}


static void screen_sw_2_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
		if(lv_obj_get_state(guider_ui.screen_sw_2) & LV_STATE_CHECKED)
        {
           BEEP=1;
        }
        else BEEP=0;   //Ϩ��LED3
	}
		break;
	default:
		break;
	}
}


static void screen_sw_3_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
        if(lv_obj_get_state(guider_ui. screen_sw_3) & LV_STATE_CHECKED)
        {
            lv_obj_clear_flag(guider_ui.screen_cpicker_1, LV_OBJ_FLAG_HIDDEN);  //ȡ������
        }
        else 
        {
						for(int i=0; i<4; i++) 
						{
							RGB_SetColor(i,0);    //Ϩ��RGB��
						}
            lv_obj_add_flag(guider_ui.screen_cpicker_1, LV_OBJ_FLAG_HIDDEN);   //���ؿؼ�
        }
	}
		break;
	default:
		break;
	}
}


static void screen_cpicker_1_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	switch (code)
	{
	case LV_EVENT_VALUE_CHANGED:
	{
         lv_color_t current_color = lv_colorwheel_get_rgb(guider_ui.screen_cpicker_1);
        unsigned int LightRGB = (current_color.ch.red<<16) + 
                                (((current_color.ch.green_h<<3)+current_color.ch.green_l)<<8) + 
                                (current_color.ch.blue);
        for(int i=0; i<4; i++) RGB_SetColor(i,LightRGB);
	}
		break;
	default:
		break;
	}
}


void events_init_screen(lv_ui *ui)
{
	lv_obj_add_event_cb(ui->screen_cb_1, screen_cb_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_cb_2, screen_cb_2_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_cb_3, screen_cb_3_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_sw_1, screen_sw_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_sw_2, screen_sw_2_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_sw_3, screen_sw_3_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui->screen_cpicker_1, screen_cpicker_1_event_handler, LV_EVENT_ALL, NULL);
}
