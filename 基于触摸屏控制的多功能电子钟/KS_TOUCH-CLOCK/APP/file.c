#include "file.h"
//----------------------------------------------------------------
//                 file：SPI Flash文件读取
//----------------------------------------------------------------
//**********************************************变量***********************************************************
//*****文件路径定义*****
//PC2LCD2002字体取模方法:逐列式,顺向(高位在前),阴码.C51格式.																		    
//特殊字体:
//数码管字体:ASCII集+℃(' '+95)
//普通字体:ASCII集
u8*const APP_ASCII_S6030="1:/SYSTEM/APP/COMMON/fonts60.fon";	//数码管字体60*30大数字字体路径 
u8*const APP_ASCII_5427="1:/SYSTEM/APP/COMMON/font54.fon";		//普通字体54*27大数字字体路径 
u8*const APP_ASCII_3618="1:/SYSTEM/APP/COMMON/font36.fon";		//普通字体36*18大数字字体路径
u8*const APP_ASCII_2814="1:/SYSTEM/APP/COMMON/font28.fon";		//普通字体28*14大数字字体路径 

u8*const APP_MUSIC="1:/测试用文件/这条街.mp3";								//音乐路径

//*****字体点阵变量定义*****
u8* asc2_s6030=0;	//数码管字体60*30大字体点阵集
u8* asc2_5427=0;	//普通字体54*27大字体点阵集
u8* asc2_3618=0;	//普通字体36*18大字体点阵集
u8* asc2_2814=0;	//普通字体28*14大字体点阵集

//**********************************************函数***********************************************************
//*******************************************************************
//读取系统中的字体到变量中
//返回类型：u8
//使用参数：无
//*******************************************************************
u8 font_set(void)
{
	u8 res;
	u8 rval=0;			//设置返回值
	FIL* f_rec=0;		//定义内存区域
	f_rec=(FIL *)gui_memin_malloc(sizeof(FIL));			//开辟FIL字节的内存区域  
	//加载特殊字体
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_S6030,FA_READ);//打开文件夹:APP_ASCII_S6030
	if(res==FR_OK)					//当加载字体成功
	{
		asc2_s6030=(u8*)gui_memex_malloc(f_rec->fsize);	//为大字体开辟缓存地址
		if(asc2_s6030==0)								//当字体点阵为空
			rval=1;
		else 
			res=f_read(f_rec,asc2_s6030,f_rec->fsize,(UINT*)&br);	//一次读取整个文件
	}
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_2814,FA_READ);//打开文件:APP_ASCII_2814
	if(res==FR_OK)					//当加载字体成功
	{
			asc2_2814=(u8*)gui_memex_malloc(f_rec->fsize);	//为大字体开辟缓存地址
			if(asc2_2814==0)//当字体点阵为空
				rval=1;
			else 
				res=f_read(f_rec,asc2_2814,f_rec->fsize,(UINT*)&br);	//一次读取整个文件
	}
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_3618,FA_READ);//打开文件:APP_ASCII_3618
	if(res==FR_OK)					//当加载字体成功
	{
			asc2_3618=(u8*)gui_memex_malloc(f_rec->fsize);	//为大字体开辟缓存地址
			if(asc2_3618==0)//当字体点阵为空
				rval=1;
			else 
				res=f_read(f_rec,asc2_3618,f_rec->fsize,(UINT*)&br);	//一次读取整个文件
	} 
	res=f_open(f_rec,(const TCHAR*)APP_ASCII_5427,FA_READ);//打开文件:APP_ASCII_5427
	if(res==FR_OK)					//当加载字体成功
	{
			asc2_5427=(u8*)gui_memex_malloc(f_rec->fsize);	//为大字体开辟缓存地址
			if(asc2_5427==0)//当字体点阵为空
				rval=1;
			else 
				res=f_read(f_rec,asc2_5427,f_rec->fsize,(UINT*)&br);	//一次读取整个文件
	}  
	rval=0;
	f_close(f_rec);	//关闭文件流
	return rval;
}

