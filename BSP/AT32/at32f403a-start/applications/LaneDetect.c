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

u16 yuv_8_to_RGBGray(u8 yuv422)
{
	u16 Gray;
	Gray =(((yuv422>>3)<<11)|((yuv422>>2)<<5)|((yuv422>>3)<<0));
	return Gray;
}

u16 yuv422_y_to_bitmap(u8 threshold,u8 yuv422)
{
	u16 bitmap;
	u8 temp;

//	temp = (u8)(yuv422>>8);
	temp = yuv422;

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
unsigned char GetMedianNum(int * bArray, int iFilterLen)
{
	int i,j;// 循环变量
	unsigned char bTemp;

	// 用冒泡法对数组进行排序
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// 互换
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}

	// 计算中值
	if ((iFilterLen & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}

	return bTemp;
}

