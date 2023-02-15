#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lvgl_app.h"
void Lvgl_Lable_Demo(void)
{
		lv_obj_t *scr = lv_scr_act();
		lv_obj_t * label1 = lv_label_create(scr);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);    
    lv_label_set_recolor(label1, true);                      
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label,align the lines to the center "
                              "and wrap long text automatically.");	
    lv_obj_set_width(label1, 150);  
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);	
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);	


    lv_obj_t * label2 = lv_label_create(scr);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     
    lv_obj_set_width(label2, 150);	
    lv_label_set_text(label2, "长风破浪会有时,直挂云帆济沧海");	
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);	
}
