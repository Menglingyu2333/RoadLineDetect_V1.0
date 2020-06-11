#include "sys.h"

#include <rtthread.h>
#include <math.h>

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

u16 Gray_to_RGB565Gray(u16 Gray);
u16 RGB565_to_Gray(u16 RGB);
u16 yuv422_to_Gray(u16 yuv422);
u16 yuv_8_to_RGBGray(u8 yuv422);
u16 yuv422_y_to_bitmap(u8 threshold,u8 yuv422);
void Filter(u8 * pic,u16 W,u16 H, u16 FilterLength);
void EdgeDetect(unsigned char* pBmpBuf,unsigned char* pBmpBuf2,int lineByte,int width,int height);
void Image_YUV2Bitmap(u8*InputImageBuf,u8* OutputImageBuf,int width,int height,u8 threshold);
int HoughLineDetect(u8* Imagebuf);

