#include "alarm.h"
#include "includes.h"
#include "file.h"
#include "mp3play.h"
#include "mynotice.h"
//----------------------------------------------------------------
//                 alarm：闹钟
//----------------------------------------------------------------
//**********************************************变量***********************************************************
//*****调用calendar的变量*****
extern RTC_TimeTypeDef RTC_TimeStruct;		//时间结构体：时、分、秒、ampm
extern RTC_DateTypeDef RTC_DateStruct;		//日期结构体：年、月、日、星期
//*****系统地址变量*****
u8 SYSTEM_Alarm_SAVE_BASE;					//闹钟存储起始地址
u8 SYSTEM_Alarm_SAVE_END;					//闹钟存储结束地址

//*****alarm相关变量*****
_alarm_obj Alarm[RTC_Alarm_MAX_NUM];		//多个闹钟结构体
u8 SYSTEM_Alarm_NUM;						//系统中的闹钟个数
u8 wait_alarm[RTC_Alarm_MAX_NUM];			//等待闹钟的顺序（以在数组中的序号为索引）
u8 ring_id;									//当前设置到Alarm_A的闹钟号
u8 Alarm_FLAG;								//闹钟响铃的标志
u8 alarm_one_id;							//上一次设置的单次闹钟的id(=0XFF:表示它已经响过了)
u8 is_first_alarm;							//是否是第一次设置闹钟
u8 is_sort;									//是否需要重新排序
//**********************************************函数***********************************************************
//*******************************************************************
//初始化闹钟：读取系统中的闹钟信息	
//返回类型：void
//使用参数：无
//*******************************************************************
void alarm_init(void) 
{	
	u8 i;				//循环变量
	u16 addr;		//某个闹钟在系统中的起始地址
	//初始化系统AT24CXX内容
	alarm_system();
	//初始化读取闹钟的地址/闹钟个数
	SYSTEM_Alarm_SAVE_BASE = SYSTEM_PARA_SAVE_BASE +sizeof(SYSTEM_Alarm_SAVE_END);				//闹钟存储起始地址
	SYSTEM_Alarm_SAVE_END = AT24CXX_ReadOneByte(SYSTEM_PARA_SAVE_BASE);//获取系统存储结束地址	
	//初始化闹钟等待数组的序号
	for(i=0;i<RTC_Alarm_MAX_NUM;i++)
		wait_alarm[i] = i;
	//当闹钟存储的结束地址符合要求时
	printf("\r\nAlarm_endaddr: %d\r\n",SYSTEM_Alarm_SAVE_END);
	if(SYSTEM_Alarm_SAVE_END >= 0X64)
	{
		SYSTEM_Alarm_NUM = (SYSTEM_Alarm_SAVE_END - SYSTEM_Alarm_SAVE_BASE) / sizeof(_alarm_obj);		//获取系统中已有的闹钟个数
		//当系统闹钟个数比（最大闹钟个数）大时
		if(SYSTEM_Alarm_NUM>RTC_Alarm_MAX_NUM) 
		{
			SYSTEM_Alarm_NUM = RTC_Alarm_MAX_NUM;		//修改系统闹钟个数为最大闹钟个数
			SYSTEM_Alarm_SAVE_END = RTC_Alarm_MAX_NUM * sizeof(_alarm_obj);	//修改闹钟存储结束地址
			AT24CXX_WriteOneByte(SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//存入AT24CXX
		}
		//随机初始化几个闹钟用于测试
//		alarm_Set(0,23,59,0X80,0XFE,0);				//设置一个闹钟：打开状态
//		alarm_Set(1,17,20,0X80,0XFE,0);		   	//设置一个闹钟：打开状态
//		alarm_Set(2,12,50,0X80,0XFE,0);	
//		alarm_Set(3,11,58,0X80,0XFE,0);	
//		alarm_Set(4,19,10,0X80,0XFE,0);	
		printf("\r\nAlarm_num: %d\r\n",SYSTEM_Alarm_NUM);//测试输出闹钟个数
		//当系统中有闹钟时
		if (SYSTEM_Alarm_NUM > 0) 
		{
			//读取闹钟信息到数组中
			for(i=0;i<SYSTEM_Alarm_NUM;i++)
			{
				addr=SYSTEM_Alarm_SAVE_BASE + i * sizeof(_alarm_obj);	//即将读取的闹钟的起始地址
				alarm_read(addr,i);		//读取闹钟
				//测试：输出系统中现有的闹钟
				printf("\r\nAlarm_AT24CXX[%d]:%d,%d,%d,%d,%d,\r\n",i,Alarm[i].hour,Alarm[i].min,Alarm[i].state,Alarm[i].weekmask,Alarm[i].saveflag);//测试从系统中读出来的闹钟信息
			}
			//闹钟调度
			alarm_schedule();
		}
		else
			printf("\r\n----------------------目前系统中并没有任何闹钟信息！请在闹钟设置界面设置！-----------------------\r\n");
	}
	else
		printf("\r\nAlarm初始化失败\r\n");
} 
//*******************************************************************
//闹钟调度：将排序后的闹钟设置到RTC的Alarm_A中(前提是已经有闹钟信息)
//          每次应该在各种闹钟设置之后调用该函数
//返回类型：void
//使用参数：无
//*******************************************************************
void alarm_schedule(void)
{
	u8 id;					//id号
	//闹钟排序:当有 增、删、改 闹钟时排序
	if(is_sort == 1)
	{
		alarm_sort();
		is_sort =0;		//修改标志位
	}
	//判断排序过的闹钟中下一个需要响的闹钟的id号
	id = alarm_compare();
	//当是第一个闹钟时
	if((SYSTEM_Alarm_NUM==1)&(is_first_alarm==0))
	{
		if(Alarm[id].state == 0X80)			//开启状态
		{
			printf("\r\n进入Alarm_A的设置，要设置的闹钟是：[%d],时间为：%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);
			alarm_one_id = 0XFF;					//当上一个单个闹钟已经响过了，不用管它
		}
		else if(Alarm[id].state == 0X50)			//当闹钟是“单次闹钟时”
		{
			alarm_one_id = id;					//记录单次闹钟的id号
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);//设置闹铃时间到RTC的内置闹钟A中
			Alarm[id].state = 0X00;				//设置一次之后闹钟关闭
			Alarm[id].weekmask = 0X00;		//掩码设置为 0 
			alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);		//保存到系统内存中
			printf("\r\n进入Alarm_A的设置，要设置的闹钟是：[%d],时间为：%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
		}
		is_first_alarm = 1;//已执行过一次，防止重复进入
	}
	//不是第一个闹钟时
	//当需要设置的闹钟与已经在Alarm_A中的闹钟重复时，不需要重新设置
	if(id != ring_id)
	{
		//当闹钟是开启状态时，将其设置到Alarm_A中，等待响铃
		//printf("\r\nAlarm[%d]: %d\r\n",id,Alarm[id].state);//测试输出闹钟状态 
		if(Alarm[id].state == 0X80)
		{	
			printf("\r\n进入Alarm_A的设置，要设置的闹钟是：[%d],时间为：%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);
			alarm_one_id = 0XFF;					//当上一个单个闹钟已经响过了，不用管它
		}
		else if(Alarm[id].state == 0X50)			//当闹钟是“单次闹钟时”
		{
			alarm_one_id = id;					//记录单次闹钟的id号
			RTC_Set_AlarmA(Alarm[id].weekmask,Alarm[id].hour,Alarm[id].min,0);//设置闹铃时间到RTC的内置闹钟A中
			Alarm[id].state = 0X00;				//设置一次之后闹钟关闭
			Alarm[id].weekmask = 0X00;		//掩码设置为 0    
			alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);		//保存到系统内存中
			printf("\r\n进入Alarm_A的设置，要设置的闹钟是：[%d],时间为：%d:%d\r\n",id,Alarm[id].hour,Alarm[id].min);
		}
		ring_id = id;	//更新当前的闹钟标号
	}
}

//*******************************************************************
//闹钟排序：将现有的闹钟按时间排序从早到晚
//返回类型：void
//使用参数：无
//*******************************************************************
void alarm_sort(void)
{
	u8 i,j,temp;								//临时循环变量
	//先排min
	for(i=0;i<SYSTEM_Alarm_NUM-1;i++)
	{
		for(j=0;j<SYSTEM_Alarm_NUM-1-i;j++)
		{
			if(Alarm[wait_alarm[j]].min > Alarm[wait_alarm[j+1]].min)
			{
				temp = wait_alarm[j];
				wait_alarm[j] = wait_alarm[j+1];
				wait_alarm[j+1] = temp;
			}	
		}
	}
	//再排hour
	for(i=0;i<SYSTEM_Alarm_NUM-1;i++)
	{
		for(j=0;j<SYSTEM_Alarm_NUM-1-i;j++)
		{
			if(Alarm[wait_alarm[j]].hour > Alarm[wait_alarm[j+1]].hour)
			{
				temp = wait_alarm[j];
				wait_alarm[j] = wait_alarm[j+1];
				wait_alarm[j+1] = temp;
			}	
		}
	}
	//测试：输出排序的结果
	printf("\r\n系统中已存在的闹钟的顺序wait_alarm[%d]:\r",SYSTEM_Alarm_NUM);
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
		printf("\r%d,\r",wait_alarm[i]+1);//输出闹钟排序顺序
	printf("\r\n");
}

//*******************************************************************
//闹钟比较：当前Time与已排好序的闹钟信息比较，得到下一个要放入的闹钟
//返回类型：u8:返回下一个该设置的闹钟的id号
//使用参数：无
//*******************************************************************
u8   alarm_compare(void)
{
	u8 i;			//临时变量
	//循环已排序好的闹钟与当前时间比较，
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
	{
		if(RTC_TimeStruct.RTC_Hours == Alarm[wait_alarm[i]].hour)//同小时
		{
			if(RTC_TimeStruct.RTC_Minutes < Alarm[wait_alarm[i]].min)//比较分钟
				return wait_alarm[i];
		}
		else if(RTC_TimeStruct.RTC_Hours < Alarm[wait_alarm[i]].hour)
			return wait_alarm[i];	
	}
	//循环结束，没有，说明今天已没有闹钟需要设置
	//当下一个闹钟应该为第二天的时,设置为第一个闹钟
	return wait_alarm[0];
}
//*******************************************************************
//闹钟初始化：设置闹钟信息(包括add/modify)
//返回类型：void
//使用参数：id：闹钟数组的标号
//          hour,min,state,weekmask,saveflag:闹钟结构体的信息
//*******************************************************************
void alarm_Set(u8 id,u8 hour,u8 min,u8 state,u8 weekmask,u8 saveflag)
{
	//表示设置为单次
	if( weekmask == 0)
	{
		state = 0X50;		//设置为单次闹钟的状态
	    weekmask = 1<<RTC_DateStruct.RTC_WeekDay;		//修改闹钟掩码
	}
	is_sort = 1;		//需要对闹钟重新排序
	//保存到数组中
	Alarm[id].hour = hour;
	Alarm[id].min = min;
	Alarm[id].state = state;
	Alarm[id].weekmask = weekmask;
	Alarm[id].saveflag = saveflag;
	printf("\r\nAlarm[%d]:%d,%d,%d,%d,%d,\r\n",id,Alarm[id].hour,Alarm[id].min,Alarm[id].state,Alarm[id].weekmask,Alarm[id].saveflag);//输出设置的闹钟信息
	//保存到AT24CXX中
	alarm_save(SYSTEM_Alarm_SAVE_BASE + id * sizeof(_alarm_obj),id);
	//当添加新的闹钟时：修改相关信息
	if(id == SYSTEM_Alarm_NUM)
	{		
		SYSTEM_Alarm_NUM+=1;			//增加已设置闹钟个数
		SYSTEM_Alarm_SAVE_END += sizeof(_alarm_obj);		//更新系统闹钟结束地址
		AT24CXX_WriteOneByte( SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//存入AT24CXX
	}
}	
//*******************************************************************
//获取闹钟信息:用于传输到闹钟设置界面显示
//返回类型：_alarm_obj *   (结构体指针)
//使用参数：id：闹钟数组的标号
//*******************************************************************
_alarm_obj alarm_Get(u8 id)
{
	return Alarm[id];
}
//*******************************************************************
//闹钟删除：更新ALarm[],AT24CXX,闹钟个数等信息
//返回类型：void
//使用参数：id：闹钟数组的标号
//*******************************************************************
void alarm_delete(u8 id)
{
	u8 i;				//临时变量
	is_sort = 1;		//需要对闹钟重新排序
	//修改数组
	for(i = id;i < SYSTEM_Alarm_NUM-1;i++)
	{	
//		Alarm[i].hour = Alarm[i+1].hour;
//		Alarm[i].min = Alarm[i+1].min;
//		Alarm[i].state = Alarm[i+1].state;
//		Alarm[i].weekmask = Alarm[i+1].weekmask;
//		Alarm[i].saveflag = Alarm[i+1].saveflag;	
		Alarm[i] = Alarm[i+1];
	}
	//修改系统存储
	for(i = id;i< SYSTEM_Alarm_NUM-1;i++)
		alarm_save(SYSTEM_Alarm_SAVE_BASE + i * sizeof(_alarm_obj),i);  //修改系统中闹钟的信息
	//修改闹钟存储结束地址/闹钟个数
	SYSTEM_Alarm_NUM -= 1;			//减少已设置闹钟个数
	SYSTEM_Alarm_SAVE_END -= sizeof(_alarm_obj);		//更新系统闹钟结束地址
	AT24CXX_WriteOneByte( SYSTEM_PARA_SAVE_BASE,SYSTEM_Alarm_SAVE_END);//存入AT24CXX
	//输出调试信息
	printf("\r\n删除闹钟：Alarm[%d]---------\r\n",id);
	printf("\r\n-----------------此时系统中还剩余  %d  个闹钟如下：----------------\r\n",SYSTEM_Alarm_NUM);
	for(i =0 ;i <SYSTEM_Alarm_NUM;i++)
		printf("\r\nAlarm[%d]:%d,%d,%d,%d,%d,\r\n",i,Alarm[i].hour,Alarm[i].min,Alarm[i].state,Alarm[i].weekmask,Alarm[i].saveflag);//输出设置的闹钟信息;
}
//*******************************************************************
//闹钟响闹铃
//返回类型：void
//使用参数：无
//*******************************************************************
void alarm_ring(void)
{
	if(Alarm_FLAG == 1)
	{
		printf("\r\n【闹 钟 响】---------------------------------------------------------\r\n");
		mp3PlayerDemo(APP_MUSIC);
		Alarm_FLAG=0;
//		rval=gui_notice(0,0,1,"关闭闹钟","确认");
//		if(rval==SWITCH_CLOCK_SETTING){
//			Alarm_FLAG =0;
//			BACK_COLOR=BLACK;			//背景色为黑色
//			LCD_Clear(BLACK);			//清屏函数
//			mp3PlayerDemo(APP_MUSIC,rval);
//		}
	}
}
//*******************************************************************
//读取AT24CXX里的闹钟信息到数组中
//返回类型：void
//使用参数：id：闹钟数组的标号
//          addr：写入该闹钟的系统起始地址
//*******************************************************************
void alarm_read(u16 addr,u8 id)
{
	AT24CXX_Read(addr,&Alarm[id].hour,1);//读取闹钟信息到数组中
	AT24CXX_Read(addr+1,&Alarm[id].min,1);
	AT24CXX_Read(addr+2,&Alarm[id].state,1);
	AT24CXX_Read(addr+3,&Alarm[id].weekmask,1);
	AT24CXX_Read(addr+4,&Alarm[id].saveflag,1);
}
//*******************************************************************
//写入闹钟信息到AT24CXX中
//返回类型：void
//使用参数：id：闹钟数组的标号
//          addr：写入该闹钟的系统起始地址
//*******************************************************************
void alarm_save(u16 addr,u8 id)
{
	Alarm[id].saveflag&=0X0A;								//设置闹钟为已保存状态
	AT24CXX_Write(addr,&Alarm[id].hour,1);	//写入闹钟信息到AT24CXX
	AT24CXX_Write(addr+1,&Alarm[id].min,1);
	AT24CXX_Write(addr+2,&Alarm[id].state,1);
	AT24CXX_Write(addr+3,&Alarm[id].weekmask,1);
	AT24CXX_Write(addr+4,&Alarm[id].saveflag,1);
} 
//*******************************************************************
//初始化系统中的闹钟信息(设置AT24CXX的第254个字节为初始化标志)
//返回类型：void
//使用参数：无
//*******************************************************************
void alarm_system(void)
{
	u8 temp;			//临时变量
	u8 i;					//循环变量
	u8 set_value=0X55;		//第254字节设置值
	temp=AT24CXX_ReadOneByte(254);//避免每次开机都写闹钟的地址  
	if(temp==set_value) ;		   
	else//排除第一次初始化的情况
	{
		for(i=99;i<250;i++)
			AT24CXX_WriteOneByte(i,0X00);			//清空
		AT24CXX_WriteOneByte(254,set_value);	//初始化标志
		AT24CXX_WriteOneByte(99,0X64);	//初始填入的结束地址与起始地址相同100
	  temp=AT24CXX_ReadOneByte(254);	  
	}
}

//*******************************************************************
//闹钟比较：闹钟比较得到已存在的闹钟中是否已有相同的闹钟存在
//返回类型：u8:返回值为 1 说明已存在相同的闹钟则不重复设置该闹钟
//使用参数：hour:小时、min:分钟、id：当前设置的闹钟时间
//*******************************************************************
u8   alarm_equal(u8 id,u8 hour,u8 min)
{
	u8 i;			//临时变量
	for(i=0;i<SYSTEM_Alarm_NUM;i++)
	{
	  if(i != id)
	  {
		 if(Alarm[i].hour == hour)		//同小时
		 {
			if(Alarm[i].min == min)		//同分钟
				return 1;								//存在相同闹钟
		 }
	  }
	}
	//循环已有的闹钟不存在相同的闹钟
	return 0;
}

//*******************************************************************
//如果单次闹钟在设置界面有效，则推迟该单次闹钟到下一天
//返回类型：u8:返回值为 1 说明已存在相同的闹钟则不重复设置该闹钟
//使用参数：无
//*******************************************************************
u8   alarm_one_delay(void)
{
	//当上一个单次闹钟已经响过了
	if(alarm_one_id == 0XFF)//已经到下一个闹钟了
		return 0;
	//判断该单次闹钟有没有过期
	if(RTC_TimeStruct.RTC_Hours > Alarm[alarm_one_id].hour)
	{
		return 1;						//闹钟过期了
	}
	else if(RTC_TimeStruct.RTC_Hours == Alarm[alarm_one_id].hour)
	{
		if(RTC_TimeStruct.RTC_Minutes > Alarm[alarm_one_id].min)
			return 1;
		else if(RTC_TimeStruct.RTC_Minutes == Alarm[alarm_one_id].min)
			if(RTC_TimeStruct.RTC_Seconds >= 0)
				return 1;
	}
	//闹钟没有过期，返回 0
	return 0;
}
