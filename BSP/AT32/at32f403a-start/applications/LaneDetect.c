#include "LaneDetect.h"

//RGB565_framebuf=RGB565_to_Gray(RGB565_framebuf);
//RGB565_framebuf=Gray_to_RGB565Gray(RGB565_framebuf);

//把灰度值转换为RGB565格式
u16 Gray_to_RGB565Gray(u16 Gray)
{
  u16 RGB565Gray;
  RGB565Gray=((0x001F&Gray)<<11)|((0x003F&(Gray<<1))<<5)|(0x001F&Gray);
	return RGB565Gray;
}
u16 RGB565_to_Gray(u16 RGB)
{
  u16 Gray;
  u16 R  = (RGB & RGB565_RED)    >> 3+8;
  u16 G  = (RGB & RGB565_GREEN)  >> 5;
  u16 B  = (RGB & RGB565_BLUE)   ;
  Gray=(u16)(( R*77 +  G * 150 +  B * 29 +128 )/256);
  return Gray;
}
u16 yuv422_to_Gray(u16 yuv422)
{
	u16 Gray;
	Gray =(((yuv422>>(8+3))<<11)|((yuv422>>(8+2))<<5)|((yuv422>>(8+3))<<0));
	return Gray;
}

u16 yuv422_y_to_bitmap(u8 threshold,u16 yuv422)
{
	u16 bitmap;
	u8 temp;

	temp = (u8)(yuv422>>8);

	if(temp >= threshold)
	{
		bitmap =0xffff;
	}
	else
	{
		bitmap =0x0000;
	}

	return bitmap;
}

