#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "sys.h"
#include "includes.h"

#define menu_left	0
#define menu_top	lcddev.height*6/7
#define menu_width	lcddev.width
#define menu_height	lcddev.height/7
#define menu_id		0

#define menu_button_top			lcddev.height*11/12		
#define menu_button_width		lcddev.width/3-5
#define menu_button_height		lcddev.height/14
#define menu_button_id			0
u8 setting_play(void);
u8 alarm_play(u8 count);
u8 alarm_add_notice(u16 x,u16 y,u8 mode,u8 *caption);
u8 alarm_set(u16 x,u16 y,u8 *hour,u8 *min,u8 *caption);
#endif
