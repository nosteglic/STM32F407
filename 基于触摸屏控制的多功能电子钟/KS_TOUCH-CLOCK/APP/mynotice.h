#ifndef __MYNOTICE_H
#define __MYNOTICE_H

#include "includes.h"
#define ERROR_GUI				0X01			//GUI==NULL
#define ERROR_DOUBLE_CLICK		0X02
#define ERROR_ITEM_SIZE			0X03
#define ERROR_MALLOC			0X04

#define SWITCH_CLOCK_SETTING	0X00			//切换时钟/设置界面
#define HOLD_CLOCK_SETTING		0X40			//保持时钟/设置界面
#define CLOSE_CLOCK				0X20
void myerror_notice(u8 rval);
u8 gui_notice(u16 x,u16 y,u8 mode,u8 *caption,u8 *bcaption);
#endif
