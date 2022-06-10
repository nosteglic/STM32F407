#ifndef __FILE_H
#define __FILE_H
#include "sys.h"
#include "includes.h"
//----------------------------------------------------------------
//                 file：SPI Flash文件读取
//----------------------------------------------------------------
//*****变量*****
extern u8*const APP_ASCII_S6030;	//数码管大字体路径
extern u8*const APP_ASCII_5427;		//普通大字体路径
extern u8*const APP_ASCII_3618;		//普通大字体路径
extern u8*const APP_ASCII_2814;		//普通大字体路径

extern u8* asc2_s6030;				//数码管字体60*30大字体点阵集
extern u8* asc2_5427;				//普通字体54*27大字体点阵集
extern u8* asc2_3618;				//普通字体36*18大字体点阵集
extern u8* asc2_2814;				//普通字体28*14大字体点阵集

extern u8*const APP_MUSIC;		//音乐文件的路径

//*****函数声明*****
u8 font_set(void);				//读取特殊字体
				    				   
#endif

