#ifndef __SETTING_H
#define __SETTING_H
#include "sys.h"
#include "includes.h" 	   	 

#define SYSSET_BTN1_WIDTH			100			//一类按键宽度(加减按钮)
#define SYSSET_BTN1_HEIGHT			80			//一类按键高度(加减按钮)
#define SYSSET_BTN2_WIDTH			120			//二类按键宽度(确认/取消按钮)
#define SYSSET_BTN2_HEIGHT			60			//二类按键高度(确认/取消按钮)
#define SYSSET_BTN3_WIDTH			100			//三类按键宽度(单个按钮的宽度)
#define SYSSET_EDIT_WIDTH			100			//文字编辑处的宽度
#define SYSSET_EDIT_HEIGHT			50			//文字编辑处的高度
#define SYSSET_EDIT_BACK_COLOR		GBLUE		//显示内容区背景色

//弹出窗口选择条目的设置信息
#define APP_ITEM_BTN1_WIDTH		60	  		//有2个按键时的宽度
#define APP_ITEM_BTN2_WIDTH		100			//只有1个按键时的宽度
#define APP_ITEM_BTN_HEIGHT		30			//按键高度
#define APP_ITEM_ICO_SIZE		32			//ICO图标的尺寸

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//选择时的背景色
#define APP_WIN_BACK_COLOR	 	0XC618		//窗体背景色

//π值定义
#define	app_pi	3.1415926535897932384626433832795

u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption);
u8 sysset_is_leap_year(u16 year);
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_filebrower(u8 *topname,u8 mode);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 * set_search_caption(const u8 *mcaption);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *hour,u8 *min,u8 *selx,u8 mode,u8*caption,u8 *is_open80);
#endif
