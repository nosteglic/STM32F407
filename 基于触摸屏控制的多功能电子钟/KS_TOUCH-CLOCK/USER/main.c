#include "includes.h"
#include "display.h"
#include "file.h"
#include "calendar.h"
#include "alarm.h"

vu8 system_task_return;		//任务强制返回标志.

extern RTC_TimeTypeDef RTC_TimeStruct;     //时间结构体：时、分、秒、ampm
extern RTC_DateTypeDef RTC_DateStruct;			//日期结构体：年、月、日、星期

void system_init(){
	/******MALLOC*****/		    
	FSMC_SRAM_Init();				//初始化外部SRAM  
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	/*******GUI******/
	piclib_init();					//piclib初始化	  
	exfuns_init();					//FATFS 申请内存
	/******************************初始化*********************************/
	delay_init(168);     		//初始化延时函数
	uart_init(115200);	 		//初始化串口波特率为115200
	LED_Init();					 		//初始化LED
	TPAD_Init(8);						//初始化触摸按键,以84/4=21Mhz频率计数
	LCD_Init();							//LCD初始化
	LCD_Clear(BLACK);				//清屏函数
	gui_init();							//GUI插件初始化  
	tp_dev.init();					//触摸屏初始化
	My_RTC_Init();		 			//初始化RTC
	usmart_dev.init(84); 		//初始化USMART
	Adc_Init();							//ADC初始化
	BEEP_Init();						//蜂鸣器初始化
	AT24CXX_Init(); 				//EEPROM初始化
	WM8978_Init();					//WM8978初始化
	WM8978_HPvol_Set(40,40);//耳机音量设置
	WM8978_SPKvol_Set(50);	//喇叭音量设置;
	/**********************************************************************/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
	/*******************************汉字***********************************/
	font_init();						//汉字字体初始化	
	W25QXX_Init();					//W25QXX初始化
	f_mount(fs[1],"1:",1);	//挂载SPI FLASH
	/*******************************数码管字体*****************************/
	font_set();							//设置特殊字体
	/********************************AT24CXX检测**********************************/
	AT24CXX_Check();				//检测IIC器件是否正常
	/*****************************闹钟*****************************************/
	alarm_init();						//闹钟初始化
}

//主函数
int main(void){
	/**********变量***********/
	u8 rval=HOLD_CLOCK_SETTING;
	u8 temp=0;
	/**********函数***********/
	system_init();							//系统各种初始化
	while(1){
		rval=calendar_play(0);				//显示时钟界面
		printf("calendar_play:%d\n",rval);	//一切printf语句都是为了调试而设
		if(rval==SWITCH_CLOCK_SETTING){		//进入设置界面
			do{
				rval=setting_play();		//菜单栏函数
				printf("setting_play:%d\n",rval);
				BACK_COLOR=BLACK;			//背景色为黑色
				LCD_Clear(BLACK);			//清屏函数
				temp=rval;
				rval=calendar_play(2);//运行时钟
				rval=temp;
			}while(rval==HOLD_CLOCK_SETTING);
			//设置时候闹钟不响
			Alarm_FLAG=0;
			//判断单次闹钟是否过期、过期了则设置该单次闹钟到下一天
			if(alarm_one_delay())
			{
				printf("\r\n------------------上一个设置的单次闹钟过期！将该单次闹钟顺延到下一天！-----------------\r\n");
				Alarm[alarm_one_id].state = 0X50;
				Alarm[alarm_one_id].weekmask = 1<<((RTC_DateStruct.RTC_WeekDay+1)%7);
				alarm_save(SYSTEM_Alarm_SAVE_BASE + alarm_one_id * sizeof(_alarm_obj),alarm_one_id);		//保存到系统内存中
			}
		}
		alarm_ring();						//判断闹钟是否要响
	}
}

