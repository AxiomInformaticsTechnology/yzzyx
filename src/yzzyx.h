/*
 * yzzyx.h
 *
 *  Created on: Jan 23, 2015
 *      Author: mike
 */

#ifndef YZZYX_H_
#define YZZYX_H_


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char       byte  ;
typedef unsigned short      byte2 ;
typedef unsigned int        byte4 ;
typedef unsigned long int   byte8 ;

//dont pad structure elements
#pragma pack(1)

typedef struct rgb4
{
  byte intensity;
  byte blue;
  byte green;
  byte red;
} RGB4;
typedef struct rgbi
{
  byte blue;
  byte green;
  byte red;
  byte intensity;
} RGBI;

typedef struct rgb3
{
  byte blue;
  byte green;
  byte red;
} RGB3;

typedef union rgb34
{
  RGB3 r3[1];
  RGB4 r4[1];
  RGBI i4[1];
} RGB34;

typedef struct PLAINBITMAP
{
  byte2 bitmark;
  byte4 filesize;
  byte _0600[4];
  byte _0A00[4];
  byte _0E00[4];
  byte4 width;
  byte4 height;
  byte2 depth;
  byte2 bitsperpixel;
  byte4 _1E00;
  byte4 datasize;
  byte4 _2600;
  byte4 _2A00;
  byte4 _2E00;
  byte4 _3200;

  RGB34 bitmapcolors;

}  PLAINBITMAP;

void x24_color(PLAINBITMAP* bmp, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt, int xcellmin, int ycellmin, int *rectangles_extracted);
void x32_color(PLAINBITMAP* bmp, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt, int xcellmin, int ycellmin, int *rectangles_extracted);
void x32i_color(PLAINBITMAP* bmp, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt, int xcellmin, int ycellmin, int *rectangles_extracted);
void x24_histo(PLAINBITMAP *bmp, int histoz[5][33][33][33][33]);
void x32_histo(PLAINBITMAP *bmp, int histoz[5][33][33][33][33]);
void x32i_histo(PLAINBITMAP *bmp, int histoz[5][33][33][33][33]);
void display_histo(int histoz[5][33][33][33][33],int histo,int shfta,int mord,int inb);

#define RGB_HISTO(bmp,hst) (bmp->bitsperpixel==24) ? ( x24_histo(bmp,hst) ) : ((bmp->filesize - bmp->datasize)==70) ? ( x32_histo(bmp,hst) ) : (x32i_histo(bmp,hst) )
#define CLR_IMAGE(bmp)     (bmp->bitsperpixel==24) ? ( x24_color(bmp, xedgecropl, xedgecropr, yedgecropb, yedgecropt, xcellmin, ycellmin, &rectangles_extracted ) ) : ((bmp->filesize - bmp->datasize)==70) ? ( x32_color(bmp, xedgecropl, xedgecropr, yedgecropb, yedgecropt, xcellmin, ycellmin, &rectangles_extracted ) ) : (x32i_color(bmp, xedgecropl, xedgecropr, yedgecropb, yedgecropt, xcellmin, ycellmin, &rectangles_extracted )  )


#define PIXEL_IS_BLACK  (PIXEL.red==0   &&PIXEL.green==0    &&PIXEL.blue==0  )
#define PIXEL_IS_DARK   (PIXEL.red< 128 &&PIXEL.green< 128  &&PIXEL.blue< 128)
#define PIXEL_IS_RED    (PIXEL.red==255 &&PIXEL.green==0    &&PIXEL.blue==0  )
#define PIXEL_IS_BLUE   (PIXEL.red==0   &&PIXEL.green==0    &&PIXEL.blue==255)
#define PIXEL_IS_GREEN  (PIXEL.red==0   &&PIXEL.green==255  &&PIXEL.blue==0  )
#define PIXEL_IS_YELLOW (PIXEL.red==255 &&PIXEL.green==255  &&PIXEL.blue==0  )
#define PIXEL_IS_WHITE  (PIXEL.red==255 &&PIXEL.green==255  &&PIXEL.blue==255)
#define PIXEL_IS_LIGHT  (PIXEL.red>=128 &&PIXEL.green>=128  &&PIXEL.blue>=128)

#endif /* YZZYX_H_ */
