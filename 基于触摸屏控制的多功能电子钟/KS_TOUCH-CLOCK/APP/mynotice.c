#include "mynotice.h"
#include "setting.h"

/**********************************************************************
 * 函数名称：myerror_notice
 * 输入：	u8 rval出错码
 * 输出：u8 rval
 * 功能：出错提示
 *********************************************************************/
void myerror_notice(u8 rval){
	switch(rval){
		case 1:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"GUI添加失败!","确认");
			break;
		case 2:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"双击失败!","确认");
			break;
		case 3:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"条目创建失败!","确认");
			break;
		case 4:
			gui_notice((lcddev.width-300)/2,(lcddev.height-400)/2,1,"内存分配失败!","确认");
			break;
		default:
			break;
	}
}

/**********************************************************************
 * 函数名称：gui_notice
 * 输入：	x,y:窗口坐标(窗口尺寸已经固定了的)	 
			mode：	0：两个按钮，添加/取消
					1：一个按钮，确认
			caption:提示条
			bcaption：按钮名
 * 输出：u8 rval
 * 功能：提示框
 *********************************************************************/
u8 gui_notice(u16 x,u16 y,u8 mode,u8 *caption,u8 *bcaption){
	u8 rval=0,res;
	u8 flag=0;
	_window_obj *twin=0;
	_btn_obj *Abtn;
	_btn_obj *Cbtn;
	_btn_obj *Obtn;
	_btn_obj *label;
	
	/////////////////////////////////////////////////////////////////////////
	twin=window_creat(x,y,300,400,0,1|1<<5,24);											//创建窗口
 	
	label=btn_creat(x+25+30,y+84+82,SYSSET_EDIT_WIDTH*2,SYSSET_EDIT_HEIGHT,0,0x04);		//创建提示条
	
	/*窗体*/
	if(twin==NULL)rval=ERROR_GUI;
	else{
		twin->caption="提示";
		twin->captionheight=60;
		twin->captionbkcu=YELLOW;
		twin->captionbkcd=YELLOW;
		twin->captioncolor=BLACK;
		twin->windowbkc=WHITE;
	}
	
	if(mode==0){//两个按钮
		Abtn=btn_creat(x+25,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
		Cbtn=btn_creat(x+25+125,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);	//创建按钮
		
		if(Abtn==NULL||Cbtn==NULL)rval=ERROR_GUI;
		
		Abtn->font=24;
		Abtn->bcfucolor=BLACK;		//松开时为黑色
		Abtn->bcfdcolor=BLACK;		//按下时为黑色
		Abtn->bkctbl[0]=WHITE;		//边框颜色
		Abtn->bkctbl[1]=GRAY;		//第一行的颜色				
		Abtn->bkctbl[2]=GREEN;		//上半部分颜色
		Abtn->bkctbl[3]=GREEN;		//下半部分颜色
		Abtn->caption=bcaption;
		
		Cbtn->font=24;
		Cbtn->bcfucolor=WHITE;		//松开时为白色
		Cbtn->bcfdcolor=WHITE;		//按下时为白色
		Cbtn->bkctbl[0]=WHITE;		//边框颜色
		Cbtn->bkctbl[1]=GRAY;		//第一行的颜色				
		Cbtn->bkctbl[2]=RED;		//上半部分颜色
		Cbtn->bkctbl[3]=RED;		//下半部分颜色
		Cbtn->caption="取消";
	}
	else{//一个按钮
		Obtn=btn_creat(x+25+SYSSET_BTN2_WIDTH/2+1,y+84+234,SYSSET_BTN2_WIDTH,SYSSET_BTN2_HEIGHT,0,0x02);		//创建按钮
		
		if(Obtn==NULL)rval=ERROR_GUI;
		
		Obtn->font=24;
		Obtn->bcfucolor=WHITE;		//松开时为白色
		Obtn->bcfdcolor=WHITE;		//按下时为白色
		Obtn->bkctbl[0]=WHITE;		//边框颜色
		Obtn->bkctbl[1]=GRAY;		//第一行的颜色				
		Obtn->bkctbl[2]=RED;		//上半部分颜色
		Obtn->bkctbl[3]=RED;		//下半部分颜色
		Obtn->caption=bcaption;
	}
	
	/*显示提示信息*/
	label->caption=caption;
	label->font=24;
	
	if(rval!=ERROR_GUI){
		window_draw(twin);					//画窗体
		
		/*画按钮*/
		if(mode==0){
			btn_draw(Abtn);					
			btn_draw(Cbtn);
		}
		else{
			btn_draw(Obtn);
		}
		
		btn_draw(label);					//画标签
		
		while(flag==0){
			tp_dev.scan(0);    						//触摸屏
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值
			
			if(mode==0){
				/*取消按钮检测*/
				res=btn_check(Cbtn,&in_obj);
				if(res){
					if(((Cbtn->sta&0X80)==0)){//有有效操作
						rval=HOLD_CLOCK_SETTING;
						flag=1;
						break;	//退出
					}
				}
				/*bcaption按钮检测*/
				res=btn_check(Abtn,&in_obj);
				if(res){
					if((Abtn->sta&0X80)==0){//有有效操作
						rval=SWITCH_CLOCK_SETTING;
						flag=1;
						break;
					}
				}
			}//if(mode==0)
			else{
				/*确认按钮检测*/
				res=btn_check(Obtn,&in_obj);
				if(res){
					if(((Obtn->sta&0X80)==0)){
						rval=SWITCH_CLOCK_SETTING;
						flag=1;
						break;
					}
				}
			}
			
		}//while(rval==0)
	}//if(rval==0)
	///////////////////////////////////////////////////////////////
	window_delete(twin);				//删除窗口
	
	/*删除按钮*/
	if(mode==0){
		btn_delete(Abtn);
		btn_delete(Cbtn);
	}
	btn_delete(label);					//删除提示条
	printf("gui_notice:%d\n",rval);
	return rval;
}
