#include "sys.h"

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f


u16 Gray_to_RGB565Gray(u16 Gray);
u16 RGB565_to_Gray(u16 RGB);
u16 yuv422_to_Gray(u16 yuv422);
u16 yuv422_y_to_bitmap(u8 threshold,u16 yuv422);

