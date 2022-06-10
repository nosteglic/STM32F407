#ifndef __ALARM_H
#define __ALARM_H
#include "sys.h"
#include "rtc.h"
#include "calendar.h"
//----------------------------------------------------------------
//                 alarm：闹钟
//----------------------------------------------------------------

//******宏定义*******
#define RTC_Alarm_MAX_NUM  				8				//闹钟最大个数
#define SYSTEM_PARA_SAVE_BASE 		99			//系统存储起始地址

//*****系统地址变量*****
extern u8 SYSTEM_Alarm_SAVE_BASE;								//闹钟存储起始地址
extern u8 SYSTEM_Alarm_SAVE_END;									//闹钟存储结束地址

//*****alarm相关变量*****
extern u8 SYSTEM_Alarm_NUM;											//系统中的闹钟个数
extern u8 wait_alarm[RTC_Alarm_MAX_NUM];					//等待闹钟的顺序（以在数组中的序号为索引）
extern u8 ring_id;																//当前设置到Alarm_A的闹钟号
extern u8 Alarm_FLAG;														//闹钟响铃的标志
extern u8 is_first_alarm;												//是否是第一次设置闹钟
extern u8 alarm_one_id;													//上一次设置的单次闹钟的id(=0XFF:表示它已经响过了)

//******闹钟结构体******					   
__packed typedef struct  
{			
	u8  hour;	   			//闹铃小时
	u8  min;				//闹铃分钟		 
	u8  state;				//闹铃状态,0X80，开状态;其他，为关闭状态
 	u8  weekmask;			//闹钟响铃掩码 bit1~bit7,代表周一~周日.  
	u8  saveflag;			//保存标志,0X0A,保存过了;其他,还从未保存	   
}_alarm_obj;


extern _alarm_obj Alarm[RTC_Alarm_MAX_NUM];
//******函数声明******
void alarm_init(void);					//初始化闹钟基本信息
void alarm_schedule(void);				//闹钟调度
void alarm_sort(void);					//闹钟排序
u8   alarm_compare(void);				//闹钟比较
void alarm_Set(u8 id,u8 hour,u8 min,u8 state,u8 weekmask,u8 saveflag);			//设置闹钟信息(增/改)
_alarm_obj alarm_Get(u8 id);			//获取闹钟信息
void alarm_delete(u8 id);				//闹钟删除
void alarm_read(u16 addr,u8 id);		//读取闹钟从AT24CXX中
void alarm_save(u16 addr,u8 id);		//保存闹钟到AT24CXX中
void alarm_ring(void);					//闹钟响闹铃
void alarm_system(void);				//初始化系统中的闹钟信息
u8   alarm_equal(u8 id,u8 hour,u8 min);	//闹钟比较得到已存在的闹钟中是否已有相同的闹钟存在
u8   alarm_one_delay(void);				//如果单次闹钟在设置界面有效，则推迟该单次闹钟到下一天

#endif
