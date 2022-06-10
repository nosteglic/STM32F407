#include "mp3play.h"
#include "includes.h"
#include "malloc.h" 
#include "mp3dec.h"
#include "stdio.h"

/* 推荐使用以下格式mp3文件：
 * 采样率：44100Hz
 * 声  道：2
 * 比特率：320kbps
 */

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304		
#define INPUTBUF_SIZE   3000	

static HMP3Decoder		Mp3Decoder;			/* mp3解码器指针	*/
static MP3FrameInfo		Mp3FrameInfo;		/* mP3帧信息  */
static MP3_TYPE mp3player;         /* mp3播放设备 */
static uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */

__mp3ctrl * mp3ctrl;	//mp3控制结构体 
__audiodev audiodev;	//音乐播放控制器

uint8_t inputbuf[INPUTBUF_SIZE]={0};        /* 解码输入缓冲区，1940字节为最大MP3帧大小  */
static short outbuffer[2][MP3BUFFER_SIZE];  /* 解码输出缓冲区，也是I2S输入数据，实际占用字节数：RECBUFFER_SIZE*2 */

FIL file_1;											/* file objects */
FRESULT result; 
UINT bw1;            					/* File R/W count */

/* 仅允许本文件内调用的函数声明 */
void MP3Player_I2S_DMA_TX_Callback(void);



//解析ID3V1 
//buf:输入数据缓存区(大小固定是128字节)
//pctrl:MP3控制器
//返回值:0,获取正常
//    其他,获取失败
u8 mp3_id3v1_decode(u8* buf,__mp3ctrl *pctrl)
{
	ID3V1_Tag *id3v1tag;
	id3v1tag=(ID3V1_Tag*)buf;
	if (strncmp("TAG",(char*)id3v1tag->id,3)==0)//是MP3 ID3V1 TAG
	{
		if(id3v1tag->title[0])strncpy((char*)pctrl->title,(char*)id3v1tag->title,30);
		if(id3v1tag->artist[0])strncpy((char*)pctrl->artist,(char*)id3v1tag->artist,30); 
	}else return 1;
	return 0;
}
//解析ID3V2 
//buf:输入数据缓存区
//size:数据大小
//pctrl:MP3控制器
//返回值:0,获取正常
//    其他,获取失败
u8 mp3_id3v2_decode(u8* buf,u32 size,__mp3ctrl *pctrl)
{
	ID3V2_TagHead *taghead;
	ID3V23_FrameHead *framehead; 
	u32 t;
	u32 tagsize;	//tag大小
	u32 frame_size;	//帧大小 
	taghead=(ID3V2_TagHead*)buf; 
	if(strncmp("ID3",(const char*)taghead->id,3)==0)//存在ID3?
	{
		tagsize=((u32)taghead->size[0]<<21)|((u32)taghead->size[1]<<14)|((u16)taghead->size[2]<<7)|taghead->size[3];//得到tag 大小
		pctrl->datastart=tagsize;		//得到mp3数据开始的偏移量
		if(tagsize>size)tagsize=size;	//tagsize大于输入bufsize的时候,只处理输入size大小的数据
		if(taghead->mversion<3)
		{
			printf("not supported mversion!\r\n");
			return 1;
		}
		t=10;
		while(t<tagsize)
		{
			framehead=(ID3V23_FrameHead*)(buf+t);
			frame_size=((u32)framehead->size[0]<<24)|((u32)framehead->size[1]<<16)|((u32)framehead->size[2]<<8)|framehead->size[3];//得到帧大小
 			if (strncmp("TT2",(char*)framehead->id,3)==0||strncmp("TIT2",(char*)framehead->id,4)==0)//找到歌曲标题帧,不支持unicode格式!!
			{
				strncpy((char*)pctrl->title,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_TITSIZE_MAX-1));
			}
 			if (strncmp("TP1",(char*)framehead->id,3)==0||strncmp("TPE1",(char*)framehead->id,4)==0)//找到歌曲艺术家帧
			{
				strncpy((char*)pctrl->artist,(char*)(buf+t+sizeof(ID3V23_FrameHead)+1),AUDIO_MIN(frame_size-1,MP3_ARTSIZE_MAX-1));
			}
			t+=frame_size+sizeof(ID3V23_FrameHead);
		} 
	}else pctrl->datastart=0;//不存在ID3,mp3数据是从0开始
	return 0;
} 

//获取MP3基本信息
//pname:MP3文件路径
//pctrl:MP3控制信息结构体 
//返回值:0,成功
//    其他,失败
u8 mp3_get_info(u8 *pname,__mp3ctrl* pctrl)
{
    HMP3Decoder decoder;
    MP3FrameInfo frame_info;
	MP3_FrameXing* fxing;
	MP3_FrameVBRI* fvbri;
	FIL*fmp3;
	u8 *buf;
	u32 br;
	u8 res;
	int offset=0;
	u32 p;
	short samples_per_frame;	//一帧的采样个数
	u32 totframes;				//总帧数
	
	fmp3=mymalloc(SRAMIN,sizeof(FIL)); 
	buf=mymalloc(SRAMIN,5*1024);		//申请5K内存 
	if(fmp3&&buf)//内存申请成功
	{ 		
		f_open(fmp3,(const TCHAR*)pname,FA_READ);//打开文件
		res=f_read(fmp3,(char*)buf,5*1024,&br);
		if(res==0)//读取文件成功,开始解析ID3V2/ID3V1以及获取MP3信息
		{  
			mp3_id3v2_decode(buf,br,pctrl);	//解析ID3V2数据
			f_lseek(fmp3,fmp3->fsize-128);	//偏移到倒数128的位置
			f_read(fmp3,(char*)buf,128,&br);//读取128字节
			mp3_id3v1_decode(buf,pctrl);	//解析ID3V1数据  
			decoder=MP3InitDecoder(); 		//MP3解码申请内存
			f_lseek(fmp3,pctrl->datastart);	//偏移到数据开始的地方
			f_read(fmp3,(char*)buf,5*1024,&br);	//读取5K字节mp3数据
 			offset=MP3FindSyncWord(buf,br);	//查找帧同步信息
			if(offset>=0&&MP3GetNextFrameInfo(decoder,&frame_info,&buf[offset])==0)//找到帧同步信息了,且下一阵信息获取正常	
			{ 
				p=offset+4+32;
				fvbri=(MP3_FrameVBRI*)(buf+p);
				if(strncmp("VBRI",(char*)fvbri->id,4)==0)//存在VBRI帧(VBR格式)
				{
					if (frame_info.version==MPEG1)samples_per_frame=1152;//MPEG1,layer3每帧采样数等于1152
					else samples_per_frame=576;//MPEG2/MPEG2.5,layer3每帧采样数等于576 
 					totframes=((u32)fvbri->frames[0]<<24)|((u32)fvbri->frames[1]<<16)|((u16)fvbri->frames[2]<<8)|fvbri->frames[3];//得到总帧数
					pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//得到文件总长度
				}else	//不是VBRI帧,尝试是不是Xing帧(VBR格式)
				{  
					if (frame_info.version==MPEG1)	//MPEG1 
					{
						p=frame_info.nChans==2?32:17;
						samples_per_frame = 1152;	//MPEG1,layer3每帧采样数等于1152
					}else
					{
						p=frame_info.nChans==2?17:9;
						samples_per_frame=576;		//MPEG2/MPEG2.5,layer3每帧采样数等于576
					}
					p+=offset+4;
					fxing=(MP3_FrameXing*)(buf+p);
					if(strncmp("Xing",(char*)fxing->id,4)==0||strncmp("Info",(char*)fxing->id,4)==0)//是Xng帧
					{
						if(fxing->flags[3]&0X01)//存在总frame字段
						{
							totframes=((u32)fxing->frames[0]<<24)|((u32)fxing->frames[1]<<16)|((u16)fxing->frames[2]<<8)|fxing->frames[3];//得到总帧数
							pctrl->totsec=totframes*samples_per_frame/frame_info.samprate;//得到文件总长度,计算出总时间，给mp3_get_curtime用
						}else	//不存在总frames字段
						{
							pctrl->totsec=fmp3->fsize/(frame_info.bitrate/8);
						} 
					}else 		//CBR格式,直接计算总播放时间
					{
						pctrl->totsec=fmp3->fsize/(frame_info.bitrate/8);	//计算出总时间，给mp3_get_curtime用
					}
				} 
				pctrl->bitrate=frame_info.bitrate;			//得到当前帧的码率
				mp3ctrl->samplerate=frame_info.samprate; 	//得到采样率. 
				if(frame_info.nChans==2)mp3ctrl->outsamples=frame_info.outputSamps; //输出PCM数据量大小 
				else mp3ctrl->outsamples=frame_info.outputSamps*2; //输出PCM数据量大小,对于单声道MP3,直接*2,补齐为双声道输出
			}else res=0XFE;//未找到同步帧	
			MP3FreeDecoder(decoder);//释放内存		
		} 
		f_close(fmp3);
	}else res=0XFF;
	myfree(SRAMIN,fmp3);
	myfree(SRAMIN,buf);	
	return res;	
}  
//得到当前播放时间
//fx:文件指针
//mp3x:mp3播放控制器
void mp3_get_curtime(FIL*fx,__mp3ctrl *mp3x)	//mp3x->totsec和mp3x->datastart在p3_get_info函数中已经获取
{
	u32 fpos=0;  	 
	if(fx->fptr>mp3x->datastart)fpos=fx->fptr-mp3x->datastart;	//得到当前文件播放到的地方 
	mp3x->cursec=fpos*mp3x->totsec/(fx->fsize-mp3x->datastart);	//当前播放到第多少秒了?	
}
//mp3文件快进快退函数
//pos:需要定位到的文件位置
//返回值:当前文件位置(即定位后的结果)
u32 mp3_file_seek(u32 pos)
{
	if(pos>audiodev.file->fsize)
	{
		pos=audiodev.file->fsize;
	}
	f_lseek(audiodev.file,pos);
	return audiodev.file->fptr;
}


/**
  * @brief   MP3格式音频播放主程序
  * @param  无
  * @retval 无
  */
uint8_t mp3PlayerDemo(u8*const mp3file)
{
	uint8_t res,key,t;
	uint8_t *read_ptr=inputbuf;
	uint32_t frames=0;
	int err=0, i=0, outputSamps=0;	
	int	read_offset = 0;				/* 读偏移指针 */
	int	bytes_left = 0;					/* 剩余字节数 */	
	mp3player.ucFreq=I2S_AudioFreq_Default;		//设置采样频率
	mp3player.ucStatus=STA_IDLE;							//状态
	mp3player.ucVolume=40;										//音量
	mp3ctrl=mymalloc(SRAMIN,sizeof(__mp3ctrl)); //为MP3控制结构体分配空间
	result=f_open(&file_1,(const TCHAR*)mp3file,FA_READ);		//读取MP3文件
	//当读取文件失败时，向串口输出错误信息
	if(result!=FR_OK)
	{
		printf("Open mp3file :%s fail!!!->%d\r\n",mp3file,result);
		result = f_close (&file_1);
		return 0xf0;	/* 停止播放 */
	}
	printf("当前播放文件 -> %s\n",mp3file);
	res=mp3_get_info((u8 *)mp3file,mp3ctrl); //获取播放的mp3文件的信息 
	if(res!=0)return 0xf4;
	//初始化MP3解码器
	Mp3Decoder = MP3InitDecoder();	
	if(Mp3Decoder==0)
	{
		printf("初始化helix解码库设备\n");
		return 0xf1;	/* 停止播放 */
	}
	printf("初始化中...\n");
	delay_ms(10);	/* 延迟一段时间，等待I2S中断结束 */
	//不开启是不会响的
	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出 	
	WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
	/*  初始化并配置I2S  */
	I2S_Play_Stop();
	//	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);	
	i2s_tx_callback=MP3Player_I2S_DMA_TX_Callback;					//回调函数
	I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],MP3BUFFER_SIZE);	//I2S,DMA初始化
	bufflag=0;
	Isread=0;
	mp3player.ucStatus = STA_PLAYING;		/* 放音状态 */
  result=f_read(&file_1,inputbuf,INPUTBUF_SIZE,&bw1);
	//读取失败
	if(result!=FR_OK)
	{
		printf("读取%s失败 -> %d\r\n",mp3file,result);
		MP3FreeDecoder(Mp3Decoder);//释放内存		
		return 0xf2;
	}
	read_ptr=inputbuf;
	bytes_left=bw1;
	audiodev.status=3;//开始播放+非暂停
	/* 进入主程序循环体 */
	while(mp3player.ucStatus == STA_PLAYING)
	{
		//寻找帧同步，返回第一个同步字的位置
		read_offset = MP3FindSyncWord(read_ptr, bytes_left);			
		//没有找到同步字
		if(read_offset < 0)																		
		{
			result=f_read(&file_1,inputbuf,INPUTBUF_SIZE,&bw1);//读取
			if(result!=FR_OK)
			{
				printf("读取%s失败 -> %d\r\n",mp3file,result);
				break;
			}
			read_ptr=inputbuf;
			bytes_left=bw1;//剩余未读字节数
			continue;
		}
		
		read_ptr += read_offset;					//偏移至同步字的位置
		bytes_left -= read_offset;				//同步字之后的数据大小	
		if(bytes_left < 1024)							//补充数据
		{
			/* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
			i=(uint32_t)(bytes_left)&3;									//判断多余的字节
			if(i) i=4-i;														//需要补充的字节
			memcpy(inputbuf+i, read_ptr, bytes_left);	//从对齐位置开始复制
			read_ptr = inputbuf+i;										//指向数据对齐位置
			//补充数据
			result = f_read(&file_1, inputbuf+bytes_left+i, INPUTBUF_SIZE-bytes_left-i, &bw1);
			bytes_left += bw1;										//有效数据流大小
		}
		//开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
		err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, outbuffer[bufflag], 0);					
		frames++;	
		//错误处理
		if (err != ERR_MP3_NONE)									
		{
			switch (err)
			{
				case ERR_MP3_INDATA_UNDERFLOW:
					printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
					result = f_read(&file_1, inputbuf, INPUTBUF_SIZE, &bw1);
					read_ptr = inputbuf;
					bytes_left = bw1;
					break;		
				case ERR_MP3_MAINDATA_UNDERFLOW:
					/* do nothing - next call to decode will provide more mainData */
					printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
					break;		
				default:
					printf("UNKNOWN ERROR:%d\r\n", err);		
					// 跳过此帧
					if (bytes_left > 0)
					{
						bytes_left --;
						read_ptr ++;
					}	
					break;
			}
			Isread=1;
		}
		else		//解码无错误，准备把数据输出到PCM
		{
			MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//获取解码信息				
			/* 输出到DAC */
			outputSamps = Mp3FrameInfo.outputSamps;							//PCM数据个数
			if (outputSamps > 0)
			{
				if (Mp3FrameInfo.nChans == 1)	//单声道
				{
					//单声道数据需要复制一份到另一个声道
					for (i = outputSamps - 1; i >= 0; i--)
					{
						outbuffer[bufflag][i * 2] = outbuffer[bufflag][i];
						outbuffer[bufflag][i * 2 + 1] = outbuffer[bufflag][i];
					}
					outputSamps *= 2;
				}//if (Mp3FrameInfo.nChans == 1)	//单声道
			}//if (outputSamps > 0)
			
			/* 根据解码信息设置采样率 */
			if (Mp3FrameInfo.samprate != mp3player.ucFreq)	//采样率 
			{
				mp3player.ucFreq = Mp3FrameInfo.samprate;
				
				printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				printf(" \r\n Samprate      %dHz", mp3player.ucFreq);
				printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
				printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
				printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
				printf(" \r\n Version       %d", Mp3FrameInfo.version);
				printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
				printf("\r\n");
				//I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
				if(mp3player.ucFreq >= I2S_AudioFreq_Default)	
				{
					//根据采样率修改I2S速率
					I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);
					I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],outputSamps);
				}
		//		audio_start();	//包含了				
				I2S_Play_Start();

			}
		}//else 解码正常
		
		if(file_1.fptr==file_1.fsize) 		//mp3文件读取完成，退出
		{
			printf("END\r\n");
			res=KEY0_PRES;
			break;
		}	
		
		while(Isread==0)	//等待填充完毕
		{
		}
		Isread=0;		
		while(1)	//按键检测
		{
			key=KEY_Scan(0);
			if(key==WKUP_PRES)//暂停
			{
				if(audiodev.status&0X01)audiodev.status&=~(1<<0);
				else audiodev.status|=0X01;  
			}	
			if(key==KEY2_PRES||key==KEY0_PRES)//关闭
			{
				res=key;
				mp3player.ucStatus = STA_IDLE;
				break; 
			}
			mp3_get_curtime(&file_1,mp3ctrl); //获取解码信息	
			t++;
			if(t==20)
			{
				t=0;
				LED0=!LED0;   //LED0闪烁
			}
			if((audiodev.status&0X01)==0)delay_ms(10);
			else break;
		}
	}
	I2S_Play_Stop();	//播放停止
	MP3FreeDecoder(Mp3Decoder);//释放内存		
	f_close(&file_1);	//关闭文件
	return res;
}

/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MP3Player_I2S_DMA_TX_Callback(void)
{
	u16 i;
	if(DMA1_Stream4->CR&(1<<19)) //当前使用Memory1数据
	{
		bufflag=0;                       //可以将数据读取到缓冲区0
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<MP3BUFFER_SIZE;i++)//暂停
			{
				outbuffer[0][i]=0;//填充0
			}
		}
	}
	else                               //当前使用Memory0数据
	{
		bufflag=1;                       //可以将数据读取到缓冲区1
		if((audiodev.status&0X01)==0)
		{
			for(i=0;i<MP3BUFFER_SIZE;i++)//暂停
			{
				outbuffer[1][i]=0;//填充0
			}
		}
	}
	Isread=1;                          // DMA传输完成标志
}







