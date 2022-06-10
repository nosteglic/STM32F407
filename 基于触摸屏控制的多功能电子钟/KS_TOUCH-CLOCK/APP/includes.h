/*
************************************************************************************************
主要的包含文件

文 件: INCLUDES.C ucos包含文件
作 者: Jean J. Labrosse
************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "beep.h"	
#include "lcd.h"
#include "key.h"  
#include "24cxx.h"   
#include "w25qxx.h" 
#include "usmart.h" 
#include "sram.h"   
#include "tpad.h"
#include "rtc.h"  
#include "touch.h"


#include "sdio_sdcard.h"

#include "ff.h"  
#include "exfuns.h"    

#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	
#include "string.h"	
#include "math.h"	
#include "gui.h"

#include "adc.h"
#include "spblcd.h"
#include "mynotice.h"
#include "wm8978.h"
#include "i2s.h"

extern volatile u8 system_task_return;		//任务强制返回标志.

#endif































