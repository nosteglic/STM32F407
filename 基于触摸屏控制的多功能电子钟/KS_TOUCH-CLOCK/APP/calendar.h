#ifndef __CALENDAR_H
#define __CALENDAR_H
#include "sys.h"
#include "includes.h"
//----------------------------------------------------------------
//                 calendar：时钟
//---------------------------------------------------------------- 

//******宏定义*******   
#define	PI	3.1415926535897932384626433832795      //π值定义

//******相关位置变量******
static u16 TIME_TOPX=100;  	//时间x坐标位置
static u16 TIME_TOPY=500;		//时间y坐标位置
static u16 DATE_TOPX=50;  	//日期x坐标位置
static u16 DATE_TOPY=10;		//日期y坐标位置

//******钟表盘变量******
static u16 center_x=240;		//表盘中心x坐标
static u16 center_y=300;		//表盘中心y坐标
static u16 size=340;				//表盘直径大小
static u16 d=12;						//表盘外圈宽度

//******温度变量******
static u16 TEMP_TOPX=70;  	//温度x坐标位置
static u16 TEMP_TOPY=600;		//温度y坐标位置

//******函数声明******
void calendar_date_refresh(void);																											//更新显示屏上的时间
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d);											//画表盘
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec);//显示时间
u8 calendar_play(u8 tag);																															//运行时钟
void temperature_show(void);																													//显示温度		
void calendar_ring(u8 type);																													//整点报时

#endif












