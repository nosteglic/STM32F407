#include "display.h"
#include "calendar.h"
#include "setting.h"
#include "alarm.h"

extern RTC_TimeTypeDef RTC_TimeStruct;     //时间结构体：时、分、秒、ampm
extern RTC_DateTypeDef RTC_DateStruct;			//日期结构体：年、月、日、星期
extern _alarm_obj Alarm[RTC_Alarm_MAX_NUM];
extern u8 SYSTEM_Alarm_NUM;

u8 alarm_num_open=0X00;

u8*const calendar_week_table[GUI_LANGUAGE_NUM][7]=
{
{(u8 *)"星期天",(u8 *)"星期一",(u8 *)"星期二",(u8 *)"星期三",(u8 *)"星期四",(u8 *)"星期五",(u8 *)"星期六"},
};

u8*const alarm_tbl[GUI_LANGUAGE_NUM][RTC_Alarm_MAX_NUM]=
{
	{
		(u8 *)"闹钟 1",
		(u8 *)"闹钟 2",
		(u8 *)"闹钟 3",
		(u8 *)"闹钟 4",
		(u8 *)"闹钟 5",
		(u8 *)"闹钟 6",
		(u8 *)"闹钟 7",
		(u8 *)"闹钟 8"
	},
};


/**********************************************************************
 * 函数名称：setting_play
 * 输入：void
 * 输出：u8 rval
 * 功能：显示功能菜单栏，并引发功能操作
 **********************************************************************/
u8 setting_play(void){
	int i;
	u8 rval=HOLD_CLOCK_SETTING,res;
	u8 **items;
	/*****时间/日期暂存************/
	u16 temp1;
	u8 temp2,temp3;	
	u8 is_open;
	_window_obj *menu;			//功能菜单栏
	_btn_obj *menu_btn[3];		/********功能键：边角按钮0X02********
									menu_btn[0]：时钟设置
									menu_btn[1]：日期设置
									menu_btn[2]：闹钟设置
								**************************************/
	items=(u8**)alarm_tbl[gui_phy.language];
	while(1){
		//////////////////////////////////////////////////////////////////////////////////////////////////
		/***********创建功能菜单栏********/
		menu=window_creat(menu_left,menu_top,menu_width,menu_height,menu_id,1<<5,24);//菜单栏创建
		/***********创建功能键************/
		menu_btn[0]=btn_creat(3,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		menu_btn[1]=btn_creat(6+menu_button_width,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		menu_btn[2]=btn_creat(6+2*menu_button_width,menu_button_top,menu_button_width,menu_button_height,menu_button_id,0x02);
		/////////////////////////////////////////////////////////////////////////////////////////////////
		/*********************************************配置**********************************************/
		
		/*****************界面配置*******************/
		//菜单配置
		if(menu==NULL){
			rval=ERROR_GUI;
			myerror_notice(rval);
		}
		else{
			menu->caption="设置菜单";
			menu->captionheight=40;
			menu->captionbkcu=YELLOW;
			menu->captionbkcd=YELLOW;
			menu->captioncolor=BLACK;
			menu->windowbkc=WHITE;
		}
		//功能键配置
		for(i=0;i<3;i++){
			if(menu_btn[i]==NULL){
				rval=ERROR_GUI;
				myerror_notice(rval);
				break;
			}
			menu_btn[i]->font=24;
			menu_btn[i]->bcfucolor=WHITE;
			menu_btn[i]->bcfucolor=WHITE;
			menu_btn[i]->bkctbl[0]=WHITE;
			menu_btn[i]->bkctbl[1]=GRAY;
			menu_btn[i]->bkctbl[2]=BLACK;
			menu_btn[i]->bkctbl[3]=BLACK;
			if(i==0)menu_btn[i]->caption="时钟设置";
			if(i==1)menu_btn[i]->caption="日期设置";
			if(i==2)menu_btn[i]->caption="闹钟设置";
			
		}
		///////////////////////////////////////////
		if(rval!=ERROR_GUI){							//GUI创建成功的话
			/*画组件*/
			window_draw(menu);							//画功能菜单栏
			for(i=0;i<3;i++)btn_draw(menu_btn[i]);		//画功能键
			//////////
			rval=1;
			while(rval!=HOLD_CLOCK_SETTING&&rval!=SWITCH_CLOCK_SETTING){
				tp_dev.scan(0);							//判断触摸屏有无触摸
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值
				if(TPAD_Scan(0)){						//按了TPAD，返回时钟显示界面
					rval=SWITCH_CLOCK_SETTING;
					break;
				}
				for(i=0;i<3;i++){						//判断三个功能键
					res=btn_check(menu_btn[i],&in_obj);	//menu_btn[i]是否被按下检测
					if(res){							//如果有功能键被按下
						if((menu_btn[i]->sta&0X80)==0){	//有有效操作
							
							/*根据功能键进入相应功能使能*/
							switch(i){
								case 0:/*时钟设置*/
									temp1=RTC_TimeStruct.RTC_Hours;
									temp2=RTC_TimeStruct.RTC_Minutes;
									rval=sysset_time_set((lcddev.width-300)/2,(lcddev.height-400)/2,(u8*)&temp1,(u8*)&temp2,"时钟设置界面");
									if(rval==SWITCH_CLOCK_SETTING) {
										RTC_Set_Time(temp1,temp2,0,0);	//将设置的时间保存
									}
									break;
								case 1:/*日期设置*/
									temp1=RTC_DateStruct.RTC_Year+2000;//由于RTC_Set_Date的year只有后两个数字，所以要加上2000
									temp2=RTC_DateStruct.RTC_Month;
									temp3=RTC_DateStruct.RTC_Date;
									rval=sysset_date_set((lcddev.width-380)/2,(lcddev.height-400)/2,&temp1,(u8*)&temp2,(u8*)&temp3,"日期设置界面");
									if(rval==SWITCH_CLOCK_SETTING){
										RTC_Set_Date(temp1-2000,temp2,temp3,RTC_Get_Week(temp1,temp2,temp3));//设置日期
									}
									break;
								case 2:/*闹钟设置*/
									if(SYSTEM_Alarm_NUM==0){						//此时没有闹钟
										temp1=RTC_TimeStruct.RTC_Hours;
										temp2=RTC_TimeStruct.RTC_Minutes;
										temp3=0;
										is_open=Alarm[0].saveflag;					//判断是否是第一次添加该闹钟
										rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,0,"未添加闹钟!","添加");
										if(rval==SWITCH_CLOCK_SETTING){				//如果点击“添加”
											if(is_open==0) is_open=0X02;			//如果是第一次添加
											rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8 *)&temp1,(u8 *)&temp2,(u8 *)&temp3,0XB0,"闹钟1",(u8 *)&is_open);//添加闹钟界面
											if(rval==SWITCH_CLOCK_SETTING){			//如果开启
												if(alarm_equal(SYSTEM_Alarm_NUM,(u8)temp1,temp2)){//有相同设置的闹钟
													rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"相同闹钟已存在!","确认");
												}
												else{
													alarm_Set(SYSTEM_Alarm_NUM,(u8)temp1,temp2,is_open,temp3,0);//保存闹钟配置
												}
											}
										}
									}
									else rval=SWITCH_CLOCK_SETTING;					//否则，进入闹钟列表
									
									while(rval==SWITCH_CLOCK_SETTING){
										printf("闹钟菜单栏： %d\n",rval);
										rval=alarm_play(SYSTEM_Alarm_NUM);			//显示闹钟列表
										if(rval==SWITCH_CLOCK_SETTING){				//点了闹钟列表界面的添加
											if(SYSTEM_Alarm_NUM>=RTC_Alarm_MAX_NUM){//超过规定可以添加的闹钟数
												rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"闹钟已满!","确认");
												SYSTEM_Alarm_NUM=RTC_Alarm_MAX_NUM;
											}
											else{									//未超过闹钟数
												temp1=RTC_TimeStruct.RTC_Hours;
												temp2=RTC_TimeStruct.RTC_Minutes;
												temp3=0;
												is_open=Alarm[SYSTEM_Alarm_NUM].saveflag;
												if(is_open==0) is_open=0X02;		//第一次添加
												rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8 *)&temp1,(u8 *)&temp2,(u8 *)&temp3,0XB0,items[SYSTEM_Alarm_NUM],(u8 *)&is_open);
												if(rval==SWITCH_CLOCK_SETTING){		//点了“开启”
													if(alarm_equal(SYSTEM_Alarm_NUM,(u8)temp1,temp2)){//有相同设置的闹钟
														rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"相同闹钟已存在!","确认");
													}
													else{
														alarm_Set(SYSTEM_Alarm_NUM,(u8)temp1,temp2,is_open,temp3,0);//保存闹钟配置
													}
												}
											}
										}//if(rval==SWITCH_CLOCK_SETTING)
										printf("alarm_num_open:%d\n\n\r\n\n",SYSTEM_Alarm_NUM);
									}
									break;
							}//switch(i)：判断设置什么
						}//if((menu_btn[i]->sta&0X80)==0)：第i个菜单按钮有有效操作
					}//if(flag_ok)：有菜单按钮按下
				}//for(i=0;i<3;i++)
			
			}//rval!=SWITCH_CLOCK_SETTING||rval!=HOLD_CLOCK_SETTING
		}//if(!flag_break)：一开始有无出错
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		window_delete(menu);							//删除功能菜单栏
		for(i=0;i<3;i++)btn_delete(menu_btn[i]);		//删除功能键
		printf("setting_play:%d\n",rval);
		return rval;
	}
}


/***********************************************************************
 * 函数名称：alarm_play
 * 输入：count：当前闹钟数
 * 输出：u8 rval
 * 功能：显示闹钟列表界面
 **********************************************************************/
u8 alarm_play(u8 count){	//count:已设置闹钟数
	////////////////////////////////////////
	u8 temp1,temp2,temp3,is_open;
	u8 rval=0,res;
	u8 rval_temp;
	u8 flag=0;
	u8 flag_temp=0;
	//[7]:0,按下的是返回按键,或者发生了错误;1,按下了确认按键,或者双击了选中的条目.
	//[6:0]:0,表示无错误;非零,错误代码.
	u8 selitem=0;			//selitem:当前选中的条目
	u8 topitem=0;			//topitem:当前最顶部的条目
	u8 **items;				//items[]:条目名字集
	/////////////////////////////////////////////////////////////
	items=(u8**)alarm_tbl[gui_phy.language];//获得闹钟名
	
	while(flag==0){
		res=app_listbox_select(&selitem,&topitem,"闹钟设置",items,count);
		if(res&SWITCH_CLOCK_SETTING){		//假如点了”添加“
			rval=SWITCH_CLOCK_SETTING;
			flag=1;
		}
		else if(res&HOLD_CLOCK_SETTING){	//”假如点了“取消”
			rval=HOLD_CLOCK_SETTING;
			flag=1;
		}
		else if(res&0X80){					//双击第selitem个闹钟
			flag_temp=0;
			temp1=Alarm[selitem].hour;
			temp2=Alarm[selitem].min;
			temp3=Alarm[selitem].weekmask;
			is_open=Alarm[selitem].state;	//判断闹钟开不开
			if(is_open==0X50)temp3=0;		//当是单次闹钟时，掩码显示为空
			while(flag_temp==0){
				rval=app_items_sel(0,0,lcddev.width,lcddev.height,(u8**)calendar_week_table[gui_phy.language],7,(u8*)&temp1,(u8*)&temp2,(u8*)&temp3,0XB0,items[selitem],(u8 *)&is_open);//多选
				if(rval==SWITCH_CLOCK_SETTING){	//假如点了“开启/修改/删除”
					if(is_open==0X01){	//删除
						rval_temp=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,0,"确认删除该闹钟?","确认");
						if(rval_temp==SWITCH_CLOCK_SETTING) {//确定删除
							alarm_delete(selitem);		//删除当前的闹钟
							selitem=0;
							count=SYSTEM_Alarm_NUM;
							//rval=SWITCH_CLOCK_SETTING;		//回到闹钟栏界面
							flag_temp=1;
						}
						else{is_open==0X00;}		//关闭闹钟
					}
					else{				//开启/修改
						if(alarm_equal(selitem,temp1,temp2)){//有相同设置的闹钟
							rval=gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"相同闹钟已存在!","确认");
						}
						else{
							alarm_Set(selitem,temp1,temp2,is_open,temp3,0);//保存闹钟配置
							flag_temp=1;
						}
					}
				}//if(rval==SWITCH_CLOCK_SETTING)
				else if(rval==HOLD_CLOCK_SETTING){
					flag_temp=1;
				}
			}//while(flag_temp==0)
		}//else if(res&0X80)
		else flag=1;						//出错			
	}//while
	printf("alarm_play:%d\n",rval);
	return rval;
}
