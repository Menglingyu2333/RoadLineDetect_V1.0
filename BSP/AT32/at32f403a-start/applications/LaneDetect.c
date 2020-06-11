#include "LaneDetect.h"

//RGB565_framebuf=RGB565_to_Gray(RGB565_framebuf);
//RGB565_framebuf=Gray_to_RGB565Gray(RGB565_framebuf);

//�ѻҶ�ֵת��ΪRGB565��ʽ
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
void Image_YUV2Bitmap(u8*InputImageBuf,u8* OutputImageBuf,int width,int height,u8 threshold)
{
  int i;
  for(i=0;i<width*height;++i)
  {
    if(*(InputImageBuf+i) >= threshold)
      *(OutputImageBuf+i) = 0xFF;
    else
      *(OutputImageBuf+i) = 0;
  }
}
unsigned char GetMedianNum(u8 * bArray, int iFilterLen)
{
	int i,j;// ѭ������
	unsigned char bTemp;

	// ��ð�ݷ��������������
	for (i = 0; i < iFilterLen - 1; i ++)
	{
    for(j = 0;j < iFilterLen - i - 1; j ++)
			if (bArray[j] > bArray[j + 1])
			{
				// ����
				bTemp = bArray[j];
				bArray[j] = bArray[j + 1];
				bArray[j + 1] = bTemp;
			}
	}

	// ������ֵ
	if ((iFilterLen & 1) > 0)
	{
		// ������������Ԫ�أ������м�һ��Ԫ��
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}

	return bTemp;
}

unsigned char GetAverageNum(u8 * bArray, int iFilterLen)
{
	u32 sum;
  int i;
	for (i = 0; i < iFilterLen; i ++)
	{
    sum += bArray[i];
	}
  return sum/iFilterLen;
}

void Filter(u8 * pic,u16 W,u16 H, u16 FilterLength)
{
	u8 i,j;// ѭ������
	u8 m,n;
	u8* bTemp;//�˲����ڴ�
  bTemp = rt_malloc(FilterLength * FilterLength);
  if(!bTemp)
  {
    rt_kprintf("Get filter memory FAILED!\n");
    return;
  }
  for(i=0;i<H-FilterLength;++i)
  {
    for(j=0;j<W-FilterLength;++j)
    {
      for(m=0;m<FilterLength;++m)
      {
        for(n=0;n<FilterLength;++n)
        {
          *(bTemp+m*FilterLength+n) = *(pic+(m+i)*W+n+j);
        }
      }
//      *(pic+i*W+j) = GetMedianNum(bTemp, FilterLength*FilterLength);//��ֵ�˲�
      *(pic+i*W+j) = GetAverageNum(bTemp, FilterLength*FilterLength);//��ֵ�˲�
    }
  }
  rt_free(bTemp);
  bTemp = NULL;
}
////Sobel���ӱ�Ե���
void EdgeDetect(unsigned char* pBmpBuf,unsigned char* pBmpBuf2,int lineByte,int width,int height)
{
  int ul, uc, ur, dl, dc, dr;
	int lu, lc, ld, ru, rc, rd;
	double hir, vec;
	for (int i = 1; i < height - 1; ++i){
		for (int j = 1; j < width - 1; ++j){
			// ��ֱ�ݶ����ӣ����ˮƽ��Ե
			vec = 0;
			ul = *(pBmpBuf + (i + 1)*lineByte + (j - 1))*(-1);
			uc = *(pBmpBuf + (i + 1)*lineByte + j)*(-2);
			ur = *(pBmpBuf + (i + 1)*lineByte + j)*(-1);
			dl = *(pBmpBuf + (i - 1)*lineByte + (j - 1)) * 1;
			dc = *(pBmpBuf + (i - 1)*lineByte + j) * 2;
			dr = *(pBmpBuf + (i - 1)*lineByte + j) * 1;
			vec = ul + uc + ur + dl + dc + dr;
			// ˮƽ�ݶ����ӣ���ⴹֱ��Ե
			hir = 0;
			lu = *(pBmpBuf + (i + 1)*lineByte + (j - 1))*(-1);
			lc = *(pBmpBuf + (i - 0)*lineByte + (j - 1))*(-2);
			ld = *(pBmpBuf + (i - 1)*lineByte + (j - 1))*(-1);
			ru = *(pBmpBuf + (i + 1)*lineByte + (j + 1)) * 1;
			rc = *(pBmpBuf + (i - 0)*lineByte + (j + 1)) * 2;
			rd = *(pBmpBuf + (i - 1)*lineByte + (j + 1)) * 1;
			hir = lu + lc + ld + ru + rc + rd;
			*(pBmpBuf2+i*lineByte+j) = round(sqrt(hir*hir + vec*vec));
		}
	}
}

////ֱ�߼��

#define pi 3.14159265
#define width   240                 //ͼ����
#define height  160                 //ͼ��߶�

#define ImgWBgn 0                   //��������ͼ������ʼ
#define ImgWEd  width               //��������ͼ���Ƚ���
#define ImgHBgn 0                   //��������ͼ��߶���ʼ��ԭͼ���40%����ʼ����
#define ImgHEd  (height/10*6)       //��������ͼ��߶Ƚ���

#define k_begin_1 20                //�ں�����֪ʶ�������������������ڵĽǶȷ�Χ
#define k_end_1   80
#define k_begin_2 100
#define k_end_2   160
#define k_resolution 5              //�������¼���߶εĽǶȷֱ���

#define mk_1  (k_end_1-k_begin_1)     //�������½Ƕ����ֵ
#define mk_2  (k_end_2-k_begin_2)     //�������½Ƕ����ֵ
#define mp    288                   //�������³������ֵ������������ͼ��Խ��߳��ȣ�
                                    //=sqrt((ImgHEd-ImgHBgn)*(ImgHEd-ImgHBgn)+(ImgWEd-ImgWBgn)*(ImgWEd-ImgWBgn))
#define TwoDArrayGet(Array,x,y,xLen)	*(Array+x*xLen+y)
int kmax_1,pmax_1;
int kmax_2,pmax_2;
int HoughLineDetect(u8* Imagebuf)
{
  int i,j;
  int p,k;
	int threshold;
  u8* PointBuf1;
  u8* PointBuf2;
#define PointBuf1_get(k,p)  (*(PointBuf1+k*mk_1+p))

  PointBuf1=(u8*)rt_malloc(mk_1 * mp);
  if(!PointBuf1)
    return 1;
  PointBuf2=(u8*)rt_malloc(mk_2 * mp);
  if(!PointBuf2)
  {
    rt_free(PointBuf1);
    return 2;
  }
  rt_memset(PointBuf1,(u8)0,mk_1*mp);
  rt_memset(PointBuf2,(u8)0,mk_2*mp);

  for(i=ImgHBgn;i<ImgHEd;++i)
  {
    for(j=ImgWBgn;j<ImgWEd;++j)
    {
      if(Imagebuf[i*width+j]==0xFF)
      {
        for(k=k_begin_1;k<k_end_1;k+=k_resolution)
        {
          p=(int)(i*cos(pi*k/180)+j*sin(pi*k/180));//p hough�任�о������
          p=(int)(p/2+mp/2); //��pֵ�Ż���ֹΪ��
          TwoDArrayGet(PointBuf1,k,p,mk_1)+=1; //�Ա任���ж�Ӧ�ظ����ֵĵ��ۼ�
        }
        for(k=k_begin_2;k<k_end_2;k+=k_resolution)
        {
          p=(int)(i*cos(pi*k/180)+j*sin(pi*k/180));//p hough�任�о������
          p=(int)(p/2+mp/2); //��pֵ�Ż���ֹΪ��
          TwoDArrayGet(PointBuf2,k,p,mk_2)+=1; //�Ա任���ж�Ӧ�ظ����ֵĵ��ۼ�
        }
      }
    }
  }

  //�ڷ�Χ1��Ѱ���ֱ��
  kmax_1=0; //�ֱ�ߵĽǶ�
  pmax_1=0; //�ֱ�ߵľ���
  for(i=k_begin_1;i<k_end_1;i+=k_resolution)
  {
    for(j=1;j<mp;j++) //mpΪԭͼ�Խ��߾���
    {
      if(TwoDArrayGet(PointBuf1,i,j,mk_1)>threshold) //�ҳ��ֱ�� thresholdΪ�м�������ڱȽ�
      {
        threshold=TwoDArrayGet(PointBuf1,i,j,mk_1);
        kmax_1=i; //��¼�ֱ�ߵĽǶ�
        pmax_1=j; //��¼�ֱ�ߵľ���
      }
    }
  }
  //�ڷ�Χ2��Ѱ���ֱ��
  kmax_2=0; //�ֱ�ߵĽǶ�
  pmax_2=0; //�ֱ�ߵľ���
  for(i=k_begin_2;i<k_end_2;i+=k_resolution)
  {
    for(j=1;j<mp;j++) //mpΪԭͼ�Խ��߾���
    {
      if(TwoDArrayGet(PointBuf2,i,j,mk_2)>threshold) //�ҳ��ֱ�� thresholdΪ�м�������ڱȽ�
      {
        threshold=TwoDArrayGet(PointBuf2,i,j,mk_2);
        kmax_2=i; //��¼�ֱ�ߵĽǶ�
        pmax_2=j; //��¼�ֱ�ߵľ���
      }

    }
  }

  for(i=ImgHBgn;i<ImgHEd;++i)
  {
    for(j=ImgWBgn;j<ImgWEd;++j)
    {
      if(Imagebuf[i*width+j]==0xFF)
      {
        p=(int)(i*cos(pi*kmax_1/180)+j*sin(pi*kmax_1/180));//pi=3.1415926
        p=(int)(p/2+mp/2); //mpΪԭͼ�Խ��߾���
        if(p==pmax_1)
          TwoDArrayGet(Imagebuf,i,j,width)=0x01; //��ԭͼ�е�ֱ�����ص������ɫֵ
        p=(int)(i*cos(pi*kmax_2/180)+j*sin(pi*kmax_2/180));//pi=3.1415926
        p=(int)(p/2+mp/2); //mpΪԭͼ�Խ��߾���
        if(p==pmax_2)
          TwoDArrayGet(Imagebuf,i,j,width)=0x01; //��ԭͼ�е�ֱ�����ص������ɫֵ
      }
    }
  }

  rt_free(PointBuf1);
  rt_free(PointBuf2);
  return 0;
}



