#ifndef __MP3PLAY_H
#define __MP3PLAY_H
#include "inttypes.h"
#include "sys.h"
#include "ff.h"

#define MP3_TITSIZE_MAX		40		//歌曲名字最大长度
#define MP3_ARTSIZE_MAX		40		//歌曲名字最大长度
//取2个值里面的较小值.
#ifndef AUDIO_MIN			
#define AUDIO_MIN(x,y)	((x)<(y)? (x):(y))
#endif

/* 状态 */
enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_ERR,			/*  error  */
};

typedef struct
{
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucStatus;			/* 状态，0表示待机，1表示播放中，2 出错 */	
	uint32_t ucFreq;			/* 采样频率 */
}MP3_TYPE;	

//ID3V1 标签 
typedef __packed struct 
{
    u8 id[3];		   	//ID,TAG三个字母
    u8 title[30];		//歌曲名字
    u8 artist[30];		//艺术家名字
	u8 year[4];			//年代
	u8 comment[30];		//备注
	u8 genre;			//流派 
}ID3V1_Tag;

//ID3V2 标签头 
typedef __packed struct 
{
    u8 id[3];		   	//ID
    u8 mversion;		//主版本号
    u8 sversion;		//子版本号
    u8 flags;			//标签头标志
    u8 size[4];			//标签信息大小(不包含标签头10字节).所以,标签大小=size+10.
}ID3V2_TagHead;

//ID3V2.3 版本帧头
typedef __packed struct 
{
    u8 id[4];		   	//帧ID
    u8 size[4];			//帧大小
    u16 flags;			//帧标志
}ID3V23_FrameHead;

//MP3 Xing帧信息(没有全部列出来,仅列出有用的部分)
typedef __packed struct 
{
    u8 id[4];		   	//帧ID,为Xing/Info
    u8 flags[4];		//存放标志
    u8 frames[4];		//总帧数
	u8 fsize[4];		//文件总大小(不包含ID3)
}MP3_FrameXing;
 
//MP3 VBRI帧信息(没有全部列出来,仅列出有用的部分)
typedef __packed struct 
{
    u8 id[4];		   	//帧ID,为Xing/Info
	u8 version[2];		//版本号
	u8 delay[2];		//延迟
	u8 quality[2];		//音频质量,0~100,越大质量越好
	u8 fsize[4];		//文件总大小
	u8 frames[4];		//文件总帧数 
}MP3_FrameVBRI;


//MP3控制结构体
typedef __packed struct 
{
    u8 title[MP3_TITSIZE_MAX];		//歌曲名字
    u8 artist[MP3_ARTSIZE_MAX];		//艺术家名字
    u32 totsec ;				//整首歌时长,单位:秒
    u32 cursec ;				//当前播放时长
	
    u32 bitrate;	   			//比特率
	u32 samplerate;				//采样率
	u16 outsamples;				//PCM输出数据量大小(以16位为单位),单声道MP3,则等于实际输出*2(方便DAC输出)
	
	u32 datastart;				//数据帧开始的位置(在文件里面的偏移)
}__mp3ctrl;

//音乐播放控制器
typedef __packed struct
{  
	//2个I2S解码的BUF
	u8 *i2sbuf1;
	u8 *i2sbuf2; 
	u8 *tbuf;				//零时数组,仅在24bit解码的时候需要用到
	FIL *file;				//音频文件指针
	
	u8 status;				//bit0:0,暂停播放;1,继续播放
							//bit1:0,结束播放;1,开启播放 
}__audiodev; 
extern __audiodev audiodev;	//音乐播放控制器

extern __mp3ctrl * mp3ctrl;
//const char *
uint8_t mp3PlayerDemo(u8*const mp3file);


#endif  /* __MP3PLAYER_H__   */

