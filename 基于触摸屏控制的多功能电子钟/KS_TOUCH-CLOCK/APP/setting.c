#include "setting.h"
#include "spb.h"

/*****************************打勾图标*******************************/
//“1:”表示flash
//“0：”表示sd卡
//“2：”表示啥忘记了
u8*const APP_OK_PIC="1:/SYSTEM/APP/COMMON/ok.bmp";				//确认图标
u8*const APP_CANCEL_PIC="1:/SYSTEM/APP/COMMON/cancel.bmp";		//取消图标
u8*const APP_UNSELECT_PIC="1:/SYSTEM/APP/COMMON/unselect.bmp";	//未选中图标
u8*const APP_SELECT_PIC="1:/SYSTEM/APP/COMMON/select.bmp";		//选中图标
///////////////////////////////////////////////////////////////////////

/**********************************************************************
 * 函数名称：app_show_nummid
 * 输入：	x,y,width,height:区域	 
			num:要显示的数字
			len:位数
			size:字体尺寸
			ptcolor,bkcolor:画笔颜色以及背景色
 * 输出：void
 * 功能：在一个区域中间显示数字
 *********************************************************************/
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor){
	u16 numlen;
	u8 xoff,yoff;
	numlen=(size/2)*len;							//数字长度
	if(numlen>width||size>height)return;
	xoff=(width-numlen)/2;
	yoff=(height-size)/2;
	POINT_COLOR=ptcolor;
	BACK_COLOR=bkcolor;
	LCD_ShowxNum(x+xoff,y+yoff,num,len,size,0X80);	//显示这个数字
}

/**********************************************************************
 * 函数名称：sysset_time_set
 * 输入：	x,y:窗口坐标(窗口尺寸已经固定了的)	 
			hour,min:时分
			caption:窗口名字
 * 输出：u8 rval
 * 功能：时间/闹钟设置
 *********************************************************************/
u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption){
	u8 rval=HOLD_CLOCK_SETTING;
	u8 res;
	u8 i;
	u8 flag=0;
	///////////////////////////////////////////////////////////////////
	_window_obj* twin=0;	//窗体
 	_btn_obj * tbtn[6];		//总共六个按钮:0,时钟加按钮;1,时钟减按钮;2,分钟加按钮;3,分钟减按钮;4,确认按钮;5,取消按钮.		  
 	twin=window_creat(x,y,300,400,0,1|1<<5,24);											//创建窗口
 	tbtn[0]=btn_creat(x+35,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//创建按钮
	tbtn[1]=btn_creat(x+35,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[2]=btn_creat(x+35+130,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[3]=btn_creat(x+35+130,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//创建按钮
	tbtn[4]=btn_creat(x+25,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
	tbtn[5]=btn_creat(x+25+125,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建按钮
	///////////////////////////////////////////////////////////////////
	/*窗口设置*/
	if(twin==NULL)rval=ERROR_GUI;		//出错
	else{
		twin->caption=caption;			//窗体标题
		twin->captionheight=60;			//标题高度
		twin->captionbkcu=YELLOW;		//标题上界颜色
		twin->captionbkcd=YELLOW;		//标题下界颜色
		twin->captioncolor=BLACK;		//标题颜色
		twin->windowbkc=WHITE;			//窗体背景颜色
	}
	
	/*按钮设置*/
	for(i=0;i<6;i++){					
		if(tbtn[i]==NULL){				//出错
			rval=ERROR_GUI;
			break;
		}
		tbtn[i]->font=24;				//按钮字体大小
		if(i<4){						//加减按键
			tbtn[i]->bcfucolor=WHITE;	//松开时为白色
			tbtn[i]->bcfdcolor=WHITE;	//按下时为白色			
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色			
			tbtn[i]->bkctbl[2]=GRAY;	//上半部分颜色
			tbtn[i]->bkctbl[3]=GRAY;	//下半部分颜色
		}else if(i==4){					//确认按键
			tbtn[i]->bcfucolor=BLACK;	//松开时为黑色
			tbtn[i]->bcfdcolor=BLACK;	//按下时为黑色
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色				
			tbtn[i]->bkctbl[2]=GREEN;	//上半部分颜色
			tbtn[i]->bkctbl[3]=GREEN;	//下半部分颜色
		}
		else{							//取消按键
			tbtn[i]->bcfucolor=WHITE;	//松开时为白色
			tbtn[i]->bcfdcolor=WHITE;	//按下时为白色
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色				
			tbtn[i]->bkctbl[2]=RED;		//上半部分颜色
			tbtn[i]->bkctbl[3]=RED;		//下半部分颜色
		}
		/*按钮名称*/
		if(i==0||i==2)tbtn[i]->caption="+";
		if(i==1||i==3)tbtn[i]->caption="-";
		if(i==4)tbtn[i]->caption="确定";
		if(i==5)tbtn[i]->caption="取消";				
	}
	//////////////////////////////////////////////////////////////////
	if(rval!=ERROR_GUI){//无错误
		window_draw(twin);					//画出窗体
		for(i=0;i<6;i++)btn_draw(tbtn[i]);	//画按钮
		
		/*画设置的时/分数字*/
		gui_fill_rectangle(x+35+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//填充时钟背景
		gui_fill_rectangle(x+35+130+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充分钟背景
 		app_show_nummid(x+35,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+35+130,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		
		while(flag==0){
			tp_dev.scan(0);							//触摸屏
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			
			/*按钮扫描*/
			for(i=0;i<6;i++){
				res=btn_check(tbtn[i],&in_obj);		//确认按钮检测
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//有有效操作
						switch(i){
							case 0:					//时钟增加按钮按下了
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1:					//时钟减少按钮按下了	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2:					//分钟增加按钮按下了
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3:					//分钟减少按钮按下了	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4:					//确认按钮按下
								rval=SWITCH_CLOCK_SETTING;
								flag=1;
								break;	  
							case 5:					//取消按钮按下	  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
 						}//switch(i)
					}//if((tbtn[i]->sta&0X80)==0)
					
					/*显示调整的时/分*/
					app_show_nummid(x+35,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+35+130,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<6;i++)
		}//while(flag==0)
 	}//if(rval!=ERROR_GUI)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	window_delete(twin);					//删除窗口
	for(i=0;i<6;i++)btn_delete(tbtn[i]);	//删除按钮
	printf("sysset_time_set:%d",rval);
	return rval;
}


/**********************************************************************
 * 函数名称：sysset_is_leap_year
 * 输入：	year
 * 输出：	该年份是不是闰年.1,是.0,不是
 * 功能：判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
 *********************************************************************/
u8 sysset_is_leap_year(u16 year){			  
	if(year%4==0){ 							//必须能被4整除
		if(year%100==0){ 
			if(year%400==0)return 1;		//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	

/***********************************************************************
 * 函数名称：sysset_date_set
 * 输入：	x,y:窗口坐标(窗口尺寸已经固定了的)	 
			year,month,date:年月日
			caption:窗口名字
 * 输出：	u8 rval
 * 功能：日期设置
 ***********************************************************************/
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption) 
{
	u8 rval=0,res;
	u8 i;
	u8 maxdate=31;			//2月份最大的天数
	u8 flag=0;
	//////////////////////////////////////////////////////////////////////////
	_window_obj* twin=0;	//窗体
 	_btn_obj * tbtn[8];		//总共八个按钮:0,年份加按钮;1,年份减按钮;2,月份加按钮;3月份减按钮;4,日期加按钮;5,日期减按钮;6,确认按钮;7,取消按钮		  
 	
	twin=window_creat(x,y,380,400,0,1|1<<5,24);											//创建窗口
 	tbtn[0]=btn_creat(x+20,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//创建按钮
	tbtn[1]=btn_creat(x+20,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[2]=btn_creat(x+20+120,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[3]=btn_creat(x+20+120,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//创建按钮
	tbtn[4]=btn_creat(x+20+240,y+84,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[5]=btn_creat(x+20+240,y+84+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//创建按钮

	tbtn[6]=btn_creat(x+40,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
	tbtn[7]=btn_creat(x+40+180,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建按钮
	
	////////////////////////////////////////////////////////////////////////////////
	/*窗体设置*/
 	if(twin==NULL)rval=ERROR_GUI;
	else{
		twin->caption=caption;
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
	}
	/*按钮设置*/
	for(i=0;i<8;i++)
	{
		tbtn[i]->font=24;
		if(tbtn[i]==NULL)
		{
			rval=ERROR_GUI;
			break;
		}
		if(i<6)			//加减按键
		{
			tbtn[i]->bcfucolor=WHITE;	//松开时为白色
			tbtn[i]->bcfdcolor=WHITE;	//按下时为白色			
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色			
			tbtn[i]->bkctbl[2]=GRAY;	//上半部分颜色
			tbtn[i]->bkctbl[3]=GRAY;	//下半部分颜色
		}else if(i==6)	//确认按键
		{
			tbtn[i]->bcfucolor=BLACK;	//松开时为黑色
			tbtn[i]->bcfdcolor=BLACK;	//按下时为黑
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色				
			tbtn[i]->bkctbl[2]=GREEN;	//上半部分颜色
			tbtn[i]->bkctbl[3]=GREEN;	//下半部分颜色
		}
		else{			//取消按键
			tbtn[i]->bcfucolor=WHITE;	//松开时为白
			tbtn[i]->bcfdcolor=WHITE;	//按下时为白色
			tbtn[i]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[i]->bkctbl[1]=GRAY;	//第一行的颜色				
			tbtn[i]->bkctbl[2]=RED;		//上半部分颜色
			tbtn[i]->bkctbl[3]=RED;		//下半部分颜色
		}
		if(i==0||i==2||i==4)tbtn[i]->caption="+";
		if(i==1||i==3||i==5)tbtn[i]->caption="-";
		if(i==6)tbtn[i]->caption="确定";
		if(i==7)tbtn[i]->caption="取消";				
	}
	
	if(rval!=ERROR_GUI)//无错误
	{
		window_draw(twin);					//画出窗体
		for(i=0;i<8;i++)btn_draw(tbtn[i]);	//画按钮
		
		/*日期数字*/
		gui_fill_rectangle(x+20+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//填充年份背景
		gui_fill_rectangle(x+20+120+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充月份背景
 		gui_fill_rectangle(x+20+240+1,y+84+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);	//填充日期背景

 		app_show_nummid(x+20,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+20+120,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		app_show_nummid(x+20+240,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
		
		while(flag==0){
			tp_dev.scan(0);    						//触摸屏检测
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值
			
			/*按键*/
			for(i=0;i<8;i++){
				res=btn_check(tbtn[i],&in_obj);		//确认按钮检测
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//有有效操作
						if(*month==2){
							if(sysset_is_leap_year(*year))maxdate=29;//是闰年的2月份
	 						else maxdate=28;	  
						}
						else if((*month==1)||(*month==3)||(*month==5)||(*month==7)||(*month==8)||(*month==10)||(*month==12))maxdate=31;
						else maxdate=30; 
						switch(i){
							case 0://年份增加按钮按下了
								(*year)++;
								if(*year>2100)*year=2000;
								break;
							case 1://年份减少按钮按下了	  
								if(*year>2000)(*year)--;
								else *year=2100;
								break;
							case 2://月份增加按钮按下了
								(*month)++;
								if(*month>12)(*month)=1;
								break;
							case 3://月份减少按钮按下了	  
								if(*month>1)(*month)--;
								else *month=12;
								break;
							case 4://日期增加按钮按下了
								(*date)++;
								if(*date>maxdate)(*date)=1;
								break;
							case 5://日期减少按钮按下了	  
								if(*date>1)(*date)--;
								else *date=maxdate;
								break;
							case 6://确认按钮按下  
								rval=SWITCH_CLOCK_SETTING;
								flag=1;
								break;	  
							case 7://取消按钮按下  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
 						}
					}   
			 		app_show_nummid(x+20,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*year,4,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+20+120,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*month,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
					app_show_nummid(x+20+240,y+84+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*date,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<8;i++)
		}//while(rval==0)
 	}
	///////////////////////////////////////////////////////
	window_delete(twin);				//删除窗口
	for(i=0;i<8;i++)btn_delete(tbtn[i]);//删除按钮
	return rval;
}

/***********************************************************************
 * 函数名称：app_listbox_select
 * 输入：	//sel:当前选中的条目
			//top:当前最顶部的条目
			//caption:选择框名字
			//items[]:条目名字集
			//itemsize:总条目数
 * 输出：	[7]:0,按下的是返回按键,或者发生了错误;1,按下了确认按键,或者双击了选中的条目.
     		[6:0]:0,表示无错误;非零,错误代码.
 * 功能：新建选择框
 ***********************************************************************/
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize){
	u8  res;
	u8 rval=0;					//返回值	  
  	u16 i;	    						   
	u8 flag=0;
 	_btn_obj* rbtn=0;			//返回按钮控件
 	_btn_obj* okbtn=0;			//确认按钮控件
	_listbox_obj * tlistbox;	//listbox 
	
	if(*sel>=itemsize||*top>=itemsize)return ERROR_ITEM_SIZE;	//参数错误/参数非法
 	app_filebrower(caption,0X07);								//显示标题
   	tlistbox=listbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.tbfsize);//创建一个filelistbox
	if(tlistbox==NULL)rval=ERROR_MALLOC;						//申请内存失败.
	else{														//添加条目
		for(i=0;i<itemsize;i++){
			res=listbox_addlist(tlistbox,items[i]);
			if(res){
				rval=ERROR_MALLOC;
				break;
			}
		}
	}
	if(rval==0){//成功添加了条目
		tlistbox->scbv->topitem=*top;
		tlistbox->selindex=*sel;
		listbox_draw_listbox(tlistbox);
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
 		okbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建确认文字按钮
		if(rbtn==NULL||okbtn==NULL)rval=ERROR_GUI;		//没有足够内存够分配
		else{
		 	rbtn->caption="取消";			//名字
			rbtn->font=gui_phy.tbfsize;		//设置新的字体大小	 	 
			rbtn->bcfdcolor=BLACK;			//按下时的颜色
			rbtn->bcfucolor=BLACK;			//松开时的颜色
			btn_draw(rbtn);					//画按钮
			
		 	okbtn->caption="添加";			//名字
		 	okbtn->font=gui_phy.tbfsize;	//设置新的字体大小	 
			okbtn->bcfdcolor=BLACK;			//按下时的颜色
			okbtn->bcfucolor=BLACK;			//松开时的颜色
			btn_draw(okbtn);				//画按钮
		}	   
 	}  
   	while(flag==0){
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值 
		/*取消按钮检测*/
		res=btn_check(rbtn,&in_obj);		    
		if(res)if(((rbtn->sta&0X80)==0)){		//按钮状态改变了
			rval=HOLD_CLOCK_SETTING;
			flag=1;
		}
		/*添加按钮检测*/
		res=btn_check(okbtn,&in_obj);
		if(res){
			if(((okbtn->sta&0X80)==0)){ 
				rval=SWITCH_CLOCK_SETTING;
				flag=1;				 				   			   
  			}	 
		}  
		
		listbox_check(tlistbox,&in_obj);		//扫描 
		if(tlistbox->dbclick==0X80){			//双击了
			*top=tlistbox->scbv->topitem;		//记录退出时选择的条目
			*sel=tlistbox->selindex;
			rval|=1<<7;							//标记双击按下了
			flag=1;			
		}
		
	}
	////////////////////////////////////////////////////////////////////////
	listbox_delete(tlistbox);					//删除listbox
	btn_delete(okbtn);							//删除按钮	  	 
	btn_delete(rbtn);							//删除按钮 
	printf("app_listbox_select:%d\n\n",rval);
	return rval; 
}

/***********************************************************************
 * 函数名称：app_filebrower
 * 输入：	//topname:浏览的时候要显示的名字	 
			//mode:
				//[0]:0,不显示上方色条;1,显示上方色条
				//[1]:0,不显示下方色条;1,显示下方色条
				//[2]:0,不显示名字;1,显示名字
				//[3~7]:保留
 * 输出：	void
 * 功能：文件浏览横条显示
 ***********************************************************************/
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);								//下分界线
	if(mode&0X02)app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);//上分界线
	if(mode&0X04)gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,BLACK,gui_phy.tbfsize,topname);	  
} 

/***********************************************************************
 * 函数名称：app_gui_tcbar
 * 输入：	//x,y,width,height:坐标及尺寸.
			//mode:	设置分界线
				//	    [3]:右边分界线
				//		[2]:左边分界线
				//		[1]:下边分界线
				//		[0]:上边分界线
 * 输出：	void
 * 功能：2色条
 ***********************************************************************/
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,YELLOW);  			//填充底部颜色	
 	gui_fill_rectangle(x,y+halfheight,width,halfheight,YELLOW); //填充底部颜色
	if(mode&0x01)gui_draw_hline(x,y,width,GRAY);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,GRAY);
	if(mode&0x04)gui_draw_vline(x,y,height,GRAY);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,GRAY);
} 

/***********************************************************************
 * 函数名称：set_search_caption
 * 输入：	mcaption:主目录下的目录名(一定要有'.'字符在里面)
 * 输出：	(u8*)(++mcaption)
 * 功能：	查找条目名字
 ***********************************************************************/
u8 * set_search_caption(const u8 *mcaption)
{
	while(*mcaption!='.')mcaption++;
	return (u8*)(++mcaption);
}

/**************************************
 * 函数名称：
 * 输入：	//x,y,width,height:坐标尺寸(width最小为150,height最小为72)
			//items[]:条目名字集
			//itemsize:总条目数(最大不超过8个)
			//selx:结果.多选模式时,对应各项的选择情况.单选模式时,对应选择的条目.
			//mode:
				//[7]:0,无OK按钮;1,有OK按钮
				//[6]:0,不读取背景色;1,读取背景色
				//[5]:0,单选模式;1,多选模式
				//[4]:0,不加载图表;1,加载图标
				//[3:0]:保留
				//caption:窗口名字	  
 * 输出：u8 rval
 * 功能：闹钟界面
 **************************************/
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *hour,u8 *min,u8 *selx,u8 mode,u8*caption,u8 *is_open80) 
{
	u8 rval=0,res;
	u8 selsta=0;	//选中状态为0,
					//[7]:标记是否已经记录第一次按下的条目;
					//[6:4]:保留
	                //[3:0]:第一次按下的条目
	u16 i;

	u8 temp;
	u16 itemheight=0;		//每个条目的高度
	u16 itemwidth=0;		//每个条目的宽度
	u8* unselpath=0;		//未选中的图标的路径
	u8* selpath=0;			//选中图标的路径
	u8* icopath=0;
	u8 flag=0;
	
///////////////////////////////////////////////////////////////////////////////////////////
 	_window_obj* twin=0;	//窗体
	_btn_obj * tbtn[7];		//时间设置的6个按钮
							//tbtn[0-3]：加减
							//tbtn[4]:开启/修改
							//tbtn[5]:取消
							//tbtn[6]:关闭
					
							
	if(itemsize>8||itemsize<1)return ERROR_ITEM_SIZE;	//条目数错误
	if(width<150||height<72)return ERROR_ITEM_SIZE; 	//尺寸错误
	
	itemheight=(height-450)/itemsize-1;					//得到每个条目的高度
	itemwidth=width;									//每个条目的宽度
	
 	twin=window_creat(0,0,lcddev.width,lcddev.height,0,1|(1<<5)|((1<<6)&mode),24);		//创建窗口
	
	tbtn[0]=btn_creat(x+120,y+114,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);			//创建按钮
	tbtn[1]=btn_creat(x+120,y+114+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[2]=btn_creat(x+120+130,y+114,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);		//创建按钮
	tbtn[3]=btn_creat(x+120+130,y+114+134,SYSSET_BTN1_WIDTH,SYSSET_BTN1_HEIGHT,0,0x02);	//创建按钮
	
	/*窗体*/
	if(twin==NULL){
		spb_delete();						//释放SPB占用的内存
		twin=window_creat(0,0,lcddev.width,lcddev.height,0,1|(1<<5)|((1<<6)&mode),24);//重新创建窗口
		rval=ERROR_GUI;
 	}
	/*按钮*/
	for(i=0;i<4;i++){
		if(tbtn[i]==NULL){
			rval=ERROR_GUI;
			break;
		}
		tbtn[i]->font=24;
		tbtn[i]->bcfucolor=WHITE;			//松开时为白色
		tbtn[i]->bcfdcolor=WHITE;			//按下时为白色			
		tbtn[i]->bkctbl[0]=WHITE;			//边框颜色
		tbtn[i]->bkctbl[1]=GRAY;			//第一行的颜色			
		tbtn[i]->bkctbl[2]=GRAY;			//上半部分颜色
		tbtn[i]->bkctbl[3]=GRAY;			//下半部分颜色
		
		if(i==0||i==2)tbtn[i]->caption="+";
		if(i==1||i==3)tbtn[i]->caption="-";				
	}
  	if(mode&(1<<7)){						//有OK按钮
		tbtn[4]=btn_creat(0,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);										//创建开启/修改按钮
		tbtn[5]=btn_creat(lcddev.width-SYSSET_BTN2_WIDTH,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);			//创建取消按钮
		tbtn[6]=btn_creat((lcddev.width-SYSSET_BTN2_WIDTH)/2,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建关闭按钮
		if(tbtn[6]==NULL)rval=ERROR_GUI;
		else{
			if(((*is_open80)==0X80)|((*is_open80)==0X50))tbtn[6]->caption="关闭";
			else tbtn[6]->caption="删除";
			tbtn[6]->font=24;
			tbtn[6]->bcfucolor=WHITE;	//松开时为白色
			tbtn[6]->bcfdcolor=WHITE;	//按下时为白色
			tbtn[6]->bkctbl[0]=WHITE;	//边框颜色
			tbtn[6]->bkctbl[1]=GRAY;	//第一行的颜色				
			tbtn[6]->bkctbl[2]=BLACK;	//上半部分颜色
			tbtn[6]->bkctbl[3]=BLACK;	//下半部分颜色
		}
		if(twin==NULL||tbtn[4]==NULL)rval=ERROR_GUI;
		else{
			tbtn[4]->font=24;
			tbtn[4]->bcfucolor=BLACK;		//松开时为黑色
			tbtn[4]->bcfdcolor=BLACK;		//按下时为黑色
			tbtn[4]->bkctbl[0]=WHITE;		//边框颜色
			tbtn[4]->bkctbl[1]=GRAY;		//第一行的颜色				
			tbtn[4]->bkctbl[2]=GREEN;		//上半部分颜色
			tbtn[4]->bkctbl[3]=GREEN;		//下半部分颜色
			if(((*is_open80)==0X80)|((*is_open80)==0X50)) tbtn[4]->caption="修改";
			else tbtn[4]->caption="开启";
			
		}
		
	}else{ 										//没有ok按钮
		tbtn[5]=btn_creat(lcddev.width-SYSSET_BTN2_WIDTH,lcddev.height-y-SYSSET_BTN2_HEIGHT,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建取消按钮
		if(twin==NULL||tbtn[5]==NULL)rval=ERROR_GUI;
	}
 	if(rval!=ERROR_GUI||rval!=ERROR_ITEM_SIZE){//之前的操作正常
		/*窗体*/
		twin->font=24;
 		twin->caption=caption;
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
		
		/*按钮*/
		tbtn[5]->font=24;
 		tbtn[5]->caption="取消";		//取消
		tbtn[5]->bcfucolor=WHITE;		//松开时为白色
		tbtn[5]->bcfdcolor=WHITE;		//按下时为白色
 		tbtn[5]->bkctbl[0]=WHITE;		//边框颜色
		tbtn[5]->bkctbl[1]=GRAY;		//第一行的颜色				
		tbtn[5]->bkctbl[2]=RED;			//上半部分颜色
		tbtn[5]->bkctbl[3]=RED;			//下半部分颜色

		if(mode&(1<<4))					//需要加载图标
		{
  			if(mode&(1<<5))				//多选模式
			{
				unselpath=(u8*)APP_CANCEL_PIC;		//未选中的图标的路径
				selpath=(u8*)APP_OK_PIC;			//选中图标的路径
			}else		   				//单选模式
			{
				unselpath=(u8*)APP_UNSELECT_PIC;	//未选中的图标的路径
				selpath=(u8*)APP_SELECT_PIC;		//选中图标的路径
			}
		}
		
		window_draw(twin);					//画出窗体
		btn_draw(tbtn[5]);					//画按钮
	    if(mode&(1<<7)){
			if((*is_open80)!=0X02)
				btn_draw(tbtn[6]);
			btn_draw(tbtn[4]);
		}
		for(i=0;i<4;i++)btn_draw(tbtn[i]);	//画按钮
		
		gui_fill_rectangle(x+120+1,y+114+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);  		//填充时钟背景
		gui_fill_rectangle(x+120+130+1,y+114+82,SYSSET_EDIT_WIDTH-2,SYSSET_EDIT_HEIGHT,SYSSET_EDIT_BACK_COLOR);		//填充分钟背景
 		app_show_nummid(x+120,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		app_show_nummid(x+120+130,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
		
		/*星期*/
		for(i=0;i<itemsize;i++){
			icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); 										//得到图标路径
			app_show_items(x+5,y+375+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,twin->windowbkc);	//显示所有的条目
			if((i+1)!=itemsize)app_draw_smooth_line(x+5,y+375+(i+1)*(itemheight+1)-1,itemwidth,1,0Xb1ffc4,0X1600b1);//画彩线
 		}
		while(flag==0){
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			
			for(i=0;i<7;i++){
				res=btn_check(tbtn[i],&in_obj);		//确认按钮检测
				if(res){
					if((tbtn[i]->sta&0X80)==0){		//有有效操作
						switch(i){
							case 0://时钟增加按钮按下了
								(*hour)++;
								if(*hour>23)*hour=0;
								break;
							case 1://时钟减少按钮按下了	  
								if(*hour)(*hour)--;
								else *hour=23;
								break;
							case 2://分钟增加按钮按下了
								(*min)++;
								if(*min>59)(*min)=0;
								break;
							case 3://分钟减少按钮按下了	  
								if(*min)(*min)--;
								else *min=59;
								break;
							case 4://确认按钮按下
								if(mode&(1<<7)){
									(*is_open80)=0X80;
									rval=SWITCH_CLOCK_SETTING;
									flag=1;
								}
								break;	  
							case 5://取消按钮按下	  
								rval=HOLD_CLOCK_SETTING;
								flag=1;
								break;
							case 6:
								if(mode&(1<<7)){
									if(((*is_open80)==0X80)|((*is_open80)==0X50)){
										(*is_open80)=0X00;//关闭
										rval=CLOSE_CLOCK;
									}
									else{
										(*is_open80)=0X01;//删除
										rval=SWITCH_CLOCK_SETTING;
									}
									flag=1;
								}
								break;
 						}
					}
					app_show_nummid(x+120,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*hour,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);
					app_show_nummid(x+120+130,y+114+82,SYSSET_EDIT_WIDTH,SYSSET_EDIT_HEIGHT,*min,2,24,BLACK,SYSSET_EDIT_BACK_COLOR);  
				}//if(res)
			}//for(i=0;i<6;i++)
			
			temp=0XFF;//标记量,如果为0XFF,在松开的时候,说明是不在有效区域内的.如果非0XFF,则表示TP松开的时候,是在有效区域内.
			for(i=0;i<itemsize;i++){
				if(tp_dev.sta&TP_PRES_DOWN){//触摸屏被按下
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+375+i*(itemheight+1),itemwidth,itemheight)){//判断某个时刻,触摸屏的值是不是在某个区域内
						if((selsta&0X80)==0){//还没有按下过
							icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); //得到图标路径
							app_show_items(x+5,y+375+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,APP_ITEM_SEL_BKCOLOR);//反选条目
							selsta=i;		//记录第一次按下的条目
							selsta|=0X80;	//标记已经按下过了
						}
						break;		
					}
				}else{ //触摸屏被松开了
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+375+i*(itemheight+1),itemwidth,itemheight)){//判断某个时刻,触摸屏的值是不是在某个区域内
						temp=i;	   
						break;
					}
				}
			}
			if((selsta&0X80)&&(tp_dev.sta&TP_PRES_DOWN)==0){//有按下过,且按键松开了
				if((selsta&0X0F)==temp){//松开之前的坐标也是在按下时的区域内.
					if(mode&(1<<5)){//多选模式,执行取反操作
						if((*selx)&(1<<temp))*selx&=~(1<<temp);
						else *selx|=1<<temp;
					}else{//单选模式																				  
						app_show_items(x+5,y+375+(*selx)*(itemheight+1),itemwidth,itemheight,items[*selx],unselpath,BLACK,twin->windowbkc);//取消之前选择的条目
						*selx=temp;
					}
				}else temp=selsta&0X0F;//得到当时按下的条目号
 				icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,temp); //得到图标路径
				app_show_items(x+5,y+375+temp*(itemheight+1),itemwidth,itemheight,items[temp],icopath,BLACK,twin->windowbkc);//反选条目
				selsta=0;//取消
			}
 		}
 	}
	/////////////////////////////////////////////////////////////////
	window_delete(twin);				//删除窗体
	for(i=0;i<6;i++)btn_delete(tbtn[i]);//删除按钮
	printf("app_items_sel:%d\n\n",rval);
	return rval;
} 

