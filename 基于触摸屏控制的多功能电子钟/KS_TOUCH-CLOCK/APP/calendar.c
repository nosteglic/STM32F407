#include "calendar.h" 	      						  
#include "math.h"
#include "alarm.h"
//----------------------------------------------------------------
//                 calendar：时钟
//----------------------------------------------------------------
//**********************************************变量***********************************************************
//变量
short temp;						//定义温度值
u8*const week_table[8]={(u8 *)" ",(u8 *)"一",(u8 *)"二",(u8 *)"三",(u8 *)"四",(u8 *)"五",(u8 *)"六",(u8 *)"日"};

//定义结构体
RTC_TimeTypeDef RTC_TimeStruct;     //时间结构体：时、分、秒、ampm
RTC_DateTypeDef RTC_DateStruct;			//日期结构体：年、月、日、星期
//**********************************************函数***********************************************************
//*******************************************************************
//根据当前的日期,更新显示到屏幕上的日历信息
//返回类型：void
//使用参数：无
//*******************************************************************
void calendar_date_refresh(void) 
{
	
 	//显示阳历年月日
	POINT_COLOR=WHITE;//白色画笔
	gui_show_num(DATE_TOPX,DATE_TOPY,2,WHITE,36,((RTC_DateStruct.RTC_Year+2000)/100)%100,0X80);		//显示年  20/19 
	gui_show_num(DATE_TOPX+36,DATE_TOPY,2,WHITE,36,(RTC_DateStruct.RTC_Year+2000)%100,0X80);		 	//显示年 
	gui_show_ptchar(DATE_TOPX+72,DATE_TOPY,lcddev.width,lcddev.height,0,WHITE,36,'-',0);					//"-"
	gui_show_num(DATE_TOPX+90,DATE_TOPY,2,WHITE,36,RTC_DateStruct.RTC_Month,0X80);		 						//显示月
	gui_show_ptchar(DATE_TOPX+126,DATE_TOPY,lcddev.width,lcddev.height,0,WHITE,36,'-',0);					//"-"   
	gui_show_num(DATE_TOPX+144,DATE_TOPY,2,WHITE,36,RTC_DateStruct.RTC_Date,0X80);	 							//显示日
	//显示星期几
	POINT_COLOR=RED;	
	Show_Str(DATE_TOPX+24,DATE_TOPY+50,lcddev.width,lcddev.height,(u8 *)"星期",24,0);//显示汉字“星期”
	Show_Str(DATE_TOPX+72,DATE_TOPY+50,lcddev.width,lcddev.height,week_table[RTC_DateStruct.RTC_WeekDay],24,0);//显示汉字
}
//*******************************************************************
//画圆形指针表盘
//返回类型：void
//使用参数：x,y:坐标中心点
//          size:表盘大小(直径)
//          d:表盘分割,秒钟的高度
//*******************************************************************
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;				//得到半径 
	u16 sx=x-r;					//左边界
	u16 sy=y-r;					//上边界
	u16 px0,px1;				//秒钟、时钟格起点的坐标
	u16 py0,py1; 				//秒钟、时钟格终点的坐标
	u16 i; 
	gui_fill_circle(x,y,r,RED);		//画外圈
	gui_fill_circle(x,y,r-6,BLACK);		//画内圈
	for(i=0;i<60;i++)//画秒钟格
	{ 
		px0=sx+r+(r-7)*sin((PI/30)*i); 
		py0=sy+r-(r-7)*cos((PI/30)*i); 
		px1=sx+r+(r-d)*sin((PI/30)*i); 
		py1=sy+r-(r-d)*cos((PI/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,WHITE);		//画一条粗线
	}
	for(i=0;i<12;i++)//画小时格
	{ 
		px0=sx+r+(r-7)*sin((PI/6)*i); 
		py0=sy+r-(r-7)*cos((PI/6)*i); 
		px1=sx+r+(r-d-2)*sin((PI/6)*i); 
		py1=sy+r-(r-d-2)*cos((PI/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);	//画一条粗线
		//显示表盘刻度
		px1=sx+r+(r-2*d-8)*sin((PI/6)*i)-d-2; 
		py1=sy+r-(r-2*d-8)*cos((PI/6)*i)-d-2;  
		if(i==0) LCD_ShowNum(px1,py1,12,2,24);
		else LCD_ShowNum(px1,py1,i,2,24); 
	}
	for(i=0;i<4;i++)//画3小时格
	{ 
		px0=sx+r+(r-7)*sin((PI/2)*i); 
		py0=sy+r-(r-7)*cos((PI/2)*i); 
		px1=sx+r+(r-d-7)*sin((PI/2)*i); 
		py1=sy+r-(r-d-7)*cos((PI/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);	//画一条粗线	
	}
	gui_fill_circle(x,y,d/2,RED);		//画中心圈
}
//*******************************************************************
//显示钟表盘上的时针、分针、秒针
//返回类型：void
//使用参数：x,y:坐标中心点
//          size:表盘大小(直径)
//          d:表盘分割,秒钟的高度
//          hour:时钟、min:分钟、sec:秒钟
//*******************************************************************
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec)
{
	static u8 oldhour=0;	//最近一次进入该函数的时分秒信息
	static u8 oldmin=0;
	static u8 oldsec=0;
	float temp;
	u16 r=size/2;		//得到半径 
	u16 sx=x-r;			//左边界
	u16 sy=y-r;			//上边界
	u16 px0,px1;		//起点
	u16 py0,py1;  	//终点
	u8 r1=d/2+3; 		//指针终点
	if(hour>11)hour-=12;//超过12小时时重新循环
  ////////////////清除上一次的数据////////////////////
	//清除小时
	temp=(float)oldmin/60;
	temp+=oldhour;
	px0=sx+r+(r-6*d)*sin((PI/6)*temp); 
	py0=sy+r-(r-6*d)*cos((PI/6)*temp); 
	px1=sx+r+r1*sin((PI/6)*temp); 
	py1=sy+r-r1*cos((PI/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,BLACK);
	//清除分钟
	temp=(float)oldsec/60;
	temp+=oldmin;
	px0=sx+r+(r-5*d)*sin((PI/30)*temp); 
	py0=sy+r-(r-5*d)*cos((PI/30)*temp); 
	px1=sx+r+r1*sin((PI/30)*temp); 
	py1=sy+r-r1*cos((PI/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,BLACK); 
	//清除秒钟 
	px0=sx+r+(r-4*d)*sin((PI/30)*oldsec); 
	py0=sy+r-(r-4*d)*cos((PI/30)*oldsec); 
	px1=sx+r+r1*sin((PI/30)*oldsec); 
	py1=sy+r-r1*cos((PI/30)*oldsec); 
	gui_draw_bline1(px0,py0,px1,py1,0,BLACK); 
  //////////////////显示////////////////////////
	//显示新的时钟
	temp=(float)min/60;
	temp+=hour;
	px0=sx+r+(r-6*d)*sin((PI/6)*temp); 
	py0=sy+r-(r-6*d)*cos((PI/6)*temp); 
	px1=sx+r+r1*sin((PI/6)*temp); 
	py1=sy+r-r1*cos((PI/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,YELLOW); 
	//显示新的分钟
	temp=(float)sec/60;
	temp+=min;
	px0=sx+r+(r-5*d)*sin((PI/30)*temp); 
	py0=sy+r-(r-5*d)*cos((PI/30)*temp); 
	px1=sx+r+r1*sin((PI/30)*temp); 
	py1=sy+r-r1*cos((PI/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,GREEN); 	 
	//显示新的秒钟
	px0=sx+r+(r-4*d)*sin((PI/30)*sec); 
	py0=sy+r-(r-4*d)*cos((PI/30)*sec); 
	px1=sx+r+r1*sin((PI/30)*sec); 
	py1=sy+r-r1*cos((PI/30)*sec); 
	gui_draw_bline1(px0,py0,px1,py1,0,GBLUE); 
	oldhour=hour;	//保存时
	oldmin=min;		//保存分
	oldsec=sec;		//保存秒
}	 
//*******************************************************************
//运行时钟：不断更新时间(包括 闹钟调度/校时校分/闹钟设置 的调用)
//返回类型：void
//使用参数：无
//*******************************************************************
u8 calendar_play(u8 tag)
{
	u8 rval=HOLD_CLOCK_SETTING;
	u8 second=0;							//临时秒数
	u8 minute=0;							//临时分钟数
	u8 hour=0;								//临时小时数
	u8 tempdate=0;						//临时天数值
	u8 tempmonth=0;						//临时月份
	u8 tempyear=0;						//临时年份
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);		//获取时间  
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);		//获取日期
	calendar_date_refresh();						//更新日期
	POINT_COLOR=WHITE;									//白色画笔
	calendar_circle_clock_drawpanel(center_x,center_y,size,d);														//显示指针时钟表盘 
	gui_show_ptchar(TIME_TOPX+80,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":"
	gui_show_ptchar(TIME_TOPX+170,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":" 
	second=RTC_TimeStruct.RTC_Seconds;			//暂存当前秒数
	minute=30;															//用于设置30min时报时
	hour=RTC_TimeStruct.RTC_Hours;					//暂存小时数
	tempdate=RTC_DateStruct.RTC_Date;				//暂存当前日期
	tempmonth=RTC_DateStruct.RTC_Month;			//暂存当前月份
	tempyear=RTC_DateStruct.RTC_Year;				//暂存当前年份
	while(1)
	{	
		//判断TPAD返回按键
		if(TPAD_Scan(0)){
			rval=SWITCH_CLOCK_SETTING;
			break;
		}
		/////////////////////时钟设置///////////////////////////////
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);//获取时间  
		if(second!=RTC_TimeStruct.RTC_Seconds||tag==2)//上一次秒数和当前不同(在这里判断tag ==2 达到设置界面时间也显示的效果)
		{
			second=RTC_TimeStruct.RTC_Seconds;//更新秒数
			
			calendar_circle_clock_showtime(center_x,center_y,size,d,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);//显示时分秒指针    
			gui_show_num(TIME_TOPX+20,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Hours,0X80);	//显示时
			gui_show_num(TIME_TOPX+110,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Minutes,0X80);	//显示分
			gui_show_num(TIME_TOPX+200,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Seconds,0X80);	//显示秒	
			temperature_show();
			
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);	//更新时间结构体
			RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);	//更新日期结构体
			//半小时报时
			if(RTC_TimeStruct.RTC_Seconds == 0)
				if(minute == RTC_TimeStruct.RTC_Minutes)	//是30分
				{
					calendar_ring(1);												
				}
			//整点报时
			if(hour!=RTC_TimeStruct.RTC_Hours)							//上一次小时和现在不同
			{
				calendar_ring(3);												
				hour=RTC_TimeStruct.RTC_Hours;								//防止重复进入
			}
			//当年、月、日任意一个被改变都立即更新屏幕上的日期
			if((RTC_DateStruct.RTC_Date!=tempdate)|(tempmonth!=RTC_DateStruct.RTC_Month)|(tempyear!=RTC_DateStruct.RTC_Year))
			{
				calendar_date_refresh();	//  更新日期
				tempdate=RTC_DateStruct.RTC_Date;	//防止重复进入
				tempmonth=RTC_DateStruct.RTC_Date;	//
	      tempyear=RTC_DateStruct.RTC_Date;	//
			}
		}
		if(tag==2)
			break;
		////////////////闹钟的相关设置///////////////////////////
		alarm_schedule();	//闹钟调度	
		if(Alarm_FLAG == 1)//当有闹钟要响时
			break;
 	}
	return rval;
}

//*******************************************************************
//显示温度到屏幕指定位置
//返回类型：void
//使用参数：无
//*******************************************************************
void temperature_show(void)
{
	POINT_COLOR=WHITE;				//白色画笔
	Show_Str(TEMP_TOPX,TEMP_TOPY+20,lcddev.width,lcddev.height,(u8 *)"温度",24,0);  									//显示汉字“温度”
	gui_show_ptchar(TEMP_TOPX+48,TEMP_TOPY+20,lcddev.width,lcddev.height,0,WHITE,24,':',0);						//":" 
	temp=Get_Temprate();	//得到温度值 
		if(temp<0)						//当温度为负时
		{
			temp=-temp;					//取反
			gui_show_ptchar(TEMP_TOPX+72,TEMP_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,'-',0);					//显示负号"-"
		}
		else 
			gui_show_ptchar(TEMP_TOPX+72,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,' ',0);					//无符号
		gui_show_num(TEMP_TOPX+92,TEMP_TOPY,2,BRRED,60,temp/100,0X80);																	//整数部分			   
		gui_show_ptchar(TEMP_TOPX+152,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,'.',0);						//"." 
		gui_show_num(TEMP_TOPX+182,TEMP_TOPY,2,BRRED,60,temp%100,0X80);																	//小数部分
		gui_show_ptchar(TEMP_TOPX+252,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,95+' ',0);				//单位“摄氏度”
}
//*******************************************************************
//整点报时
//返回类型：void
//使用参数ype:闹铃类型
//0,滴.
//1,滴.滴.
//2,滴.滴.滴
//4,滴.滴.滴.滴
//*******************************************************************
void calendar_ring(u8 type)
{
	u8 i;	 										//临时变量
	for(i=0;i<(type+1);i++)		//循环
	{
		BEEP=1;									//蜂鸣器响
		delay_ms(50);						
		BEEP=0;									//蜂鸣器停
		delay_ms(70);
	}	 
}













