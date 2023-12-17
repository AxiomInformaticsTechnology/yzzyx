//specialized for rgb 24

#include "yzzyx.h"

static void do_histo( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] , int histo , int shfta , int mord )
{
  //foreachscanline
  for (byte4 line = 0; line < bmp->height; line++)
  {
    //foreachpixelonline
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB3& map = bmp->bitmapcolors.r3[line * bmp->width + pixel];
      byte rd = map.red   >> shfta;
      byte gr = map.green >> shfta;
      byte bl = map.blue  >> shfta;
      byte in = 0;
      histoz[histo][in][rd][gr][bl]++;
    }
  }
  display_histo(histoz,histo,shfta,mord,0);
}

static void do_histos( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] )
{
  do_histo(bmp,histoz,0,7, 2);  //1  bit per gun
  do_histo(bmp,histoz,1,6, 4);  //
  do_histo(bmp,histoz,2,5, 8);  //
  do_histo(bmp,histoz,3,4,16);  //
  do_histo(bmp,histoz,4,3,32);  //
}

void x24_histo( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] )
{
  do_histos(bmp, histoz);
}

//shfta is the divisor
//newcolor = oldcolor - ( oldcolor % (1<<shfta) )
//attempt to remove textured backgrounds (not white)
RGB3 RECOLOR(RGB3 map, int shfta)
{
  RGB3 newmap   = map;

  byte red    = (newmap.red   - 31 ) >> shfta;
  byte green  = (newmap.green - 31 ) >> shfta;
  byte blue   = (newmap.blue  - 31 ) >> shfta;

//if(shfta==6)
  if (red == blue && red == green)  // && blue==green
  {
    //its grey
    red   = (red>=2) ? 3 : 0;
    blue  = red;
    green = blue;
  }
  else if (red == blue)
  {
    red = green;
    blue= green;
  }
  else if (red == green)
  {
    if((red>=2) && ((blue>0)&&(blue<2)))
    {
      red   = 0;//blue;
      green = 0;//blue;
      blue  = 0;
    }
    else
    {
      blue = 255;
      red   = blue;
      green = blue;
    }
  }
  else if (blue == green)
  {
    blue   = red;
    green  = red;
  }

  //7    8-colors
  //6   64-colors
  //5  512-colors
  //4  512 * 8
  //3
  //
  //unsigned int guns =  newmap.red << 4 |  newmap.green << 2 |  newmap.blue << 0;
  //switch(guns)
  //{
  //   case 0:
  //       break;
  //
  //   case 0x3f:
  //
  //       break;
  //}


  newmap.red    = (red)   ? (((red  +1) << shfta)-1) : 0;
  newmap.green  = (green) ? (((green+1) << shfta)-1) : 0;
  newmap.blue   = (blue)  ? (((blue +1) << shfta)-1) : 0;

  //IFNOTBLACKTHENWHITE
  if(red||green||blue)
  {
    newmap.red    = 255;
    newmap.green  = 255;
    newmap.blue   = 255;
  }
  return newmap;
}

RGB3 RECOLORA(RGB3 map, int shfta)
{
  RGB3 newmap   = map;

  byte red    = newmap.red   >> shfta;
  byte green  = newmap.green >> shfta;
  byte blue   = newmap.blue  >> shfta;


  if (red == blue && red == green)  // && blue==greeen
  {
    //its grey
    red   = (red>=2) ? 3 : 3;
    blue  = red;
    green = blue;
  }
  else
  {
    red = blue = green = 3;
    green = 0;
  }

  newmap.red    = red   << shfta;
  newmap.green  = green << shfta;
  newmap.blue   = blue  << shfta;
  return newmap;

}

void x24_recolorA( PLAINBITMAP * bmp, int shfta, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt, int xcellmin, int ycellmin )
{
  if(bmp->bitsperpixel!=24) return;
  byte *    bp      = (byte*)bmp;
            bp      = &bp[0x36];
  RGB3 *    rgb3    = (RGB3*)bp;
  size_t    xstride = (((bmp->width*3)+3)&(~3));

  size_t    xscanw  = bmp->width  - xedgecropr;
  size_t    yscanh  = bmp->height - yedgecropt;
  size_t    xscans  = 0           + xedgecropl;
  size_t    yscans  = 0           + yedgecropb;

  int mode_flag = bmp->_0600[0];
  for (byte4 line = yscans; line < yscanh; line++)
  {
    size_t rgbord  = line * xstride;
    for (byte4 pixel = xscans; pixel < xscanw; pixel++)
    {
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3&   rgbmap = xgb3[0];
              rgbmap = RECOLOR(rgbmap, shfta);
    }
  }
  if(0)
  for (byte4 line = yscans; line < yscanh; line++)
  {
    size_t rgbord  = line * xstride;
    for (byte4 pixel = xscans; pixel < xscanw; pixel++)
    {
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3&   rgbmap = xgb3[0];
              rgbmap = RECOLORA(rgbmap, shfta);
    }
  }
}

void x24_color( PLAINBITMAP * bmp, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt, int xcellmin, int ycellmin, int * rectangles_extracted )
{
  if(bmp->bitsperpixel!=24) return;

//  x24_recolorA(bmp, 6 );


  byte *    bp      = (byte*)bmp;
            bp      = &bp[0x36];
  RGB3 *    rgb3    = (RGB3*)bp;
  size_t    xstride = (((bmp->width*3)+3)&(~3));

  size_t    xscanw  = bmp->width  - xedgecropr;
  size_t    yscanh  = bmp->height - yedgecropt;
  size_t    xscans  = 0           + xedgecropl;
  size_t    yscans  = 0           + yedgecropb;

  int mode_flag = bmp->_0600[0];

  //looking for dark text on a light background
  //scene should be mostly white 
  //turns black(000) green
  //no white scan shows up white and/or green
  //all black turns all green
  //all white turns all black

  for (byte4 line = yscans; line < yscanh; line++)
  {
    size_t rgbord  = line * xstride;
    for (byte4 pixel = xscans; pixel < xscanw; pixel++)
    {
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3&   rgbmap = xgb3[0];
      //blacken white background
      if( ((rgbmap.red==255) && (rgbmap.green==255) && (rgbmap.blue==255))||
          ((rgbmap.red >200) && (rgbmap.green >200) && (rgbmap.blue >200)))
      {
        rgbmap.red   = 255;
        rgbmap.green = 255;
        rgbmap.blue  = 255;
      }
      //green black text
      else if(( (rgbmap.red==0) && (rgbmap.green==0) && (rgbmap.blue==0))||
              ( (rgbmap.red<60) && (rgbmap.green<60) && (rgbmap.blue<60)))
      {
        rgbmap.red   = 0;
        rgbmap.green = 255;
        rgbmap.blue  = 0;
      }
      //whiten everything else  -- the edges around black text in super hires
      else
      { //||        ((rgbmap.red >120) && (rgbmap.green >120) && (rgbmap.blue < 96))
        if (mode_flag==1)
        {
          ;
        }
        else
        {
          rgbmap.red   = 0;
          rgbmap.green = 0;
          rgbmap.blue  = 0;
        }
      }
    }
  }
  if (mode_flag==1) return;
  return;

  //check for green if not all white
  //use greens for smearing and then turn white
  for (byte4 line = yscans; line < yscanh; line++)
  {
    size_t rgbord  = line * xstride;
    for (byte4 pixel = xscans; pixel < xscanw; pixel++)
    {
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3&   rgbmap = xgb3[0];

      if(( (rgbmap.red==0) && (rgbmap.green==255) && (rgbmap.blue==0))  ) //||
 //        ( (rgbmap.red==0) && (rgbmap.green==  0) && (rgbmap.blue==0)))
      {
        rgbmap.red   = 255;
        rgbmap.green = 255;
        rgbmap.blue  = 255;
      }
      else
      {
        rgbmap.red   = 0;
        rgbmap.green = 0;
        rgbmap.blue  = 0;
      }
    }
  }
  if (mode_flag==2) return;


  //-------------  .....
  //-------------  .....
  //-------------  .....
  //-------------  line 1
  //
  // lines --rows  --- xth
  // strips--cols  --- yth
  //
  // scans bottom-to-top / left-to-right 
  //
  // ||||||||||
  // ||||||||||
  // ||||||||||
  // ||||||||||
  // ||||||||||
  // ||||||||||
  // ||||||||||
  // A
  // strip 1
  //after the above the bitmap should have two color values  text(white) or not text(black)
  //its harder to get a column out of a array in row major order
  //subtle distinction to be made -- row-major column-major
  //row-major row I is unique many J columns
  //col-major col J is unique many I rows
  //line  is     unique  line 1 specifies only one line
  //pixel is not unique  each line has a pixel 1
  //strip is     unique  
  //pixel is never unique
  

  //disect memory image
  //attempt to draw horizontal red lines
  //attempt to draw vertical blue line (blue under red)
  //attempt to draw vertical green on lines where space is say 4 pixels or greater
  //attempt to draw vertical yellow on lines where space is less then the say 4 
  //say 4 inter-word-pixels
  //      maximum-inter-char-pixels
  //after completion the image text is in fields surrounded by green/yellow/blue/red/edge
  //pixel stack items point to start of text columns
  //line stack items point to row of text 

  //to keep track of kerning boxes(where the text is)
  int line_stack[640];   //eg  5 1 3 6 8 12  5 scans  made it across field uninterruptted at lines  1,3,6,8,12
  int pixel_stack[480];  //eq  2 31 104      2 strips made it across field uninterruptted at pixel positions 31 and 104 
  line_stack[0]   = 0;	 //position ord 0 is current stack count
  pixel_stack[0]  = 0;	 //number of line or pixel positions stacked

  line_stack[++line_stack[0]] = yscans;
  pixel_stack[++pixel_stack[0]] = xscans;


  //put red horizontal line if page wide
  for (byte4 line = yscans; line < yscanh; line++)
  {
    int abandon = -1;
    size_t rgbord  = line * xstride;
    for (byte4 pixel = xscans; pixel < xscanw; pixel++)
    {
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3 &  rgbmap = xgb3[0];
      if(rgbmap.red||rgbmap.green||rgbmap.blue)
      {
        abandon=pixel;
        break;
      }
      rgbmap.red = 255;
      xgb3[0] = rgbmap;
    }
    static int inout = 0;
    if(abandon==-1)
    {
      inout = 1;
    }
    else
    {
      if(inout==1)
        line_stack[++line_stack[0]] = line;

      inout = 0;
      //undraw
      for (byte4 pixel = xscans; pixel < (byte4) abandon; pixel++)
      {
        RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
        RGB3   &rgbmap = xgb3[0];
        rgbmap.red = 0;
      }
    }
  }
  line_stack[++line_stack[0]] = yscanh;
  if (mode_flag==3) return;



  //put blue vertical if page wide   red is unaffected
  for (byte4 pixel = xscans; pixel < xscanw; pixel++)
  {
    int abandon = -1;
    for (byte4 line = yscans; line < yscanh; line++)
    {
      size_t rgbord  = line * xstride;
      RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
      RGB3&   rgbmap = xgb3[0];

      if((rgbmap.red||rgbmap.green||rgbmap.blue)&&(!(rgbmap.red==255&&rgbmap.green==0&&rgbmap.blue==0)))
      {
        abandon = line;
        break;
      }
      if (rgbmap.red==0&&rgbmap.green==0&&rgbmap.blue==0)
      {
        rgbmap.blue = 255;
      }
    }
    static int inout = 0;
    if(abandon==-1)
    {
      inout = 1;
    }
    else
    {
      if(inout==1)
        pixel_stack[++pixel_stack[0]] = pixel;
      inout=0;
      //undraw
      for (byte4 line = yscans; line < (byte4)abandon; line++)
      {
        size_t rgbord  = line * xstride;
        RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*pixel)));
        RGB3 &  rgbmap = xgb3[0];
        if (rgbmap.red==0&&rgbmap.green==0&&rgbmap.blue==255)
        {
          rgbmap.blue = 0;
        }
      }
    }
  }
  pixel_stack[ ++pixel_stack[0] ] = xscanw;
  if (mode_flag==4) return;

  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  {
    for(int line =1; line<=line_stack[0];line++)
    {
      printf("%d %d %d %d\n",pixel,line,pixel_stack[pixel],line_stack[line]);
    }
  }

  //disect sentences  put vertical green in space between words or  if at least xcellmin pixels wide
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line =1; line<=line_stack[0];line++)
  {
    if (mode_flag==500) if ( ( (line_stack[line+1]-1) - line_stack[line]) > 30 ) continue;
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    int fillall = 0;
    for(int xth = col; xth < pixel_stack[pixel+1]; xth++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord  = y * xstride;
        RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*xth)));
        RGB3& rgbmapA = xgb3[0];
        RGB3& rgbmapB = xgb3[1];
        RGB3& rgbmapC = xgb3[2];
        RGB3& rgbmapD = xgb3[3];
        if(               ((rgbmapA.red||rgbmapA.green||rgbmapA.blue)&&(!(rgbmapA.red==255&&rgbmapA.green==0&&rgbmapA.blue==0)))  ||
           ((fillall==0)&&((rgbmapB.red||rgbmapB.green||rgbmapB.blue)&&(!(rgbmapB.red==255&&rgbmapB.green==0&&rgbmapB.blue==0)))) ||
           ((fillall==0)&&((rgbmapC.red||rgbmapC.green||rgbmapC.blue)&&(!(rgbmapC.red==255&&rgbmapC.green==0&&rgbmapC.blue==0)))) ||
           ((fillall==0)&&((rgbmapD.red||rgbmapD.green||rgbmapD.blue)&&(!(rgbmapD.red==255&&rgbmapD.green==0&&rgbmapD.blue==0)))))
        {
          abandon = y;
          break;
        }
        if(rgbmapA.red==255&&rgbmapA.green==0&&rgbmapA.blue==0)
        {
          break;
        }
        if (rgbmapA.red==0&&rgbmapA.green==0&&rgbmapA.blue==0)
        {
          rgbmapA.green = 255;
        }
        if (fillall==0)
        {
          rgbmapB.green = 255;
          rgbmapC.green = 255;
          rgbmapD.green = 255;
        }
      }
      if(abandon==-1)
      {
        if(fillall==0) { xth++;xth++;xth++;}
        fillall = 1;
      }
      else
      {
        if(fillall==1)  fillall = 0;
        //undraw
        for (byte4 y = row; y < (byte4)abandon; y++)
        {
          size_t rgbord  = y * xstride;
          RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*xth)));
          RGB3& rgbmapA = xgb3[0];
          RGB3& rgbmapB = xgb3[1];
          RGB3& rgbmapC = xgb3[2];
          RGB3& rgbmapD = xgb3[3];
          if (rgbmapA.red==0&&rgbmapA.green==255&&rgbmapA.blue==0) rgbmapA.green = 0;
          if (fillall==0)
          {
            if (rgbmapB.red==0&&rgbmapB.green==255&&rgbmapB.blue==0) rgbmapB.green = 0;
            if (rgbmapC.red==0&&rgbmapC.green==255&&rgbmapC.blue==0) rgbmapC.green = 0;
            if (rgbmapD.red==0&&rgbmapD.green==255&&rgbmapD.blue==0) rgbmapD.green = 0;
          }
       }
      }
    }
  }
  if (mode_flag==5) return;


  //disect words   put yellow vertical between letters if room
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line =1; line<= line_stack[0]; line++)
  {
    //if multiple lines not seperated by red dont vertical seperate
    if (mode_flag==500) if ( ( (line_stack[line+1]-1) - line_stack[line]) > 30 ) continue;

    int row = line_stack[line];
    int col = pixel_stack[pixel];
    for(int xth = col; xth < pixel_stack[pixel+1]; xth++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord  = y * xstride;
        RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*xth)));
        RGB3 & rgbmapA = xgb3[0];
        if( ((rgbmapA.red||rgbmapA.green||rgbmapA.blue)&&(!(rgbmapA.red==255&&rgbmapA.green==0&&rgbmapA.blue==0))))
        {
          abandon = y;
          break;
        }
        if(rgbmapA.red==255&&rgbmapA.green==0&&rgbmapA.blue==0)
        {
          break;
        }
        if (rgbmapA.red==0&&rgbmapA.green==0&&rgbmapA.blue==0)
        {
          rgbmapA.green = 255;rgbmapA.red = 255;
        }
      }
      if(abandon!=-1)
      {
        //undraw
        for (byte4 y = row; y < (byte4)abandon; y++)
        {
          size_t rgbord  = y * xstride;
          RGB3 *  xgb3   = (RGB3 *)((byte*)rgb3+(rgbord+(3*xth)));
          RGB3 & rgbmapA = xgb3[0];
          if (rgbmapA.red==255&&rgbmapA.green==255&&rgbmapA.blue==0) { rgbmapA.green = 0; rgbmapA.red = 0; }
       }
      }
    }
  }
  if (mode_flag==6) return;

  //extract text
  //go up in red hit a blue turn right
  {
    int   not_done = 1;
    byte4 line     = yscans;
    byte4 pixel    = xscans;
    while (not_done)  //
    {
      size_t rgbord = line * xstride;
      RGB3 * xgb3   = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * pixel)));
      RGB3 & rgbmap = xgb3[0];
      RGB3   PIXEL  = rgbmap;
      if (PIXEL_IS_RED)
      {
        //go up
        line++;
      }
      else if (PIXEL_IS_BLUE||PIXEL_IS_YELLOW||PIXEL_IS_GREEN)
      {
        int countup = line;
        do
        {
          countup++;
          if ( (unsigned)countup < yscanh)
          {
            size_t rgbord = countup * xstride;
            RGB3 * xgb3 = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * pixel)));
            RGB3 & rgbmap = xgb3[0];
            PIXEL = rgbmap;
          }
        } while (((unsigned)countup < yscanh) && (PIXEL_IS_BLUE||PIXEL_IS_YELLOW||PIXEL_IS_GREEN));

        do
        {
          //move over to text cell
          do
          {
            pixel++;
            if(pixel < xscanw)
            {
               size_t rgbord = line * xstride;
               RGB3 * xgb3   = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * pixel)));
               RGB3 & rgbmap = xgb3[0];
               PIXEL  = rgbmap;
            }
          } while((pixel < xscanw) && !(PIXEL_IS_WHITE)&&!(PIXEL_IS_BLACK) && !(PIXEL_IS_LIGHT)&&!(PIXEL_IS_DARK));

          if(pixel < xscanw)
          {
            // count the width of cell
            size_t countover = pixel;
            do
            {
              countover++;
              if (countover < xscanw)
              {
                size_t rgbord = line * xstride;
                RGB3 * xgb3 = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * countover)));
                RGB3 & rgbmap = xgb3[0];
                PIXEL = rgbmap;
              }
            } while ((countover < xscanw) && ((PIXEL_IS_WHITE) || (PIXEL_IS_BLACK) || (PIXEL_IS_LIGHT) || (PIXEL_IS_DARK)  ));
            if(countover < xscanw)
            {
              //extract rectangular area from  line,pixel to uptored,whileblackorwhite

              int pixels_hit     = 0;

              if ((countup-line >= ycellmin )&&(countover-pixel >= xcellmin))
              for(int ith = countup-1;ith >= (signed)line;ith--)
              {
                size_t rgbord = ith * xstride;
                for(size_t jth = pixel;jth<countover;jth++)
                {
                  RGB3 * xgb3   = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * jth)));
                  RGB3 & rgbmap = xgb3[0];
                  PIXEL = rgbmap;
                  int bitpresent = (PIXEL_IS_BLACK) ?  0 :
                                   (PIXEL_IS_WHITE) ?  1 :
                                   (PIXEL_IS_LIGHT) ?  2 :
                                   (PIXEL_IS_DARK ) ?  3 : 4;
                  if (bitpresent)
                  {
                    pixels_hit++;
                  }
                  else
                  {
                  }
                }
              }
              printf("RECTANGLE(%06d) L%5d P%5d CU%5d CO%5ld HD%5d WD%5d HR%5d WR%5ld\n",
                  *rectangles_extracted,
                  line, pixel,
                  countup, countover,
                  bmp->height-line,
                  bmp->width-pixel,
                  countup-line,
                  countover-pixel);
              if(pixels_hit)
              {
                //output "01" image
                if ((countup - line >= ycellmin) && (countover - pixel >= xcellmin))
                for (int ith = countup - 1; ith >= (signed) line; ith--)
                {
                  size_t rgbord = ith * xstride;
                  for (size_t jth = pixel; jth < countover; jth++)
                  {
                    RGB3 * xgb3 = (RGB3 *) ((byte*) rgb3 + (rgbord + (3 * jth)));
                    RGB3 & rgbmap = xgb3[0];
                    PIXEL = rgbmap;
                    int bitpresent = (PIXEL_IS_BLACK) ?  0 :
                                     (PIXEL_IS_WHITE) ?  1 :
                                     (PIXEL_IS_LIGHT) ?  2 :
                                     (PIXEL_IS_DARK ) ?  3 : 4;
                    printf("%1d", bitpresent);
                  }
                  printf("\n");
                }

                if ((countup - line >= ycellmin) && (countover - pixel >= xcellmin))
                *rectangles_extracted = *rectangles_extracted + 1;
              }
            }
            pixel = countover;
          }
        } while(pixel < xscanw);
        line  = countup;
        pixel = xscans;
      }
      else if (PIXEL_IS_WHITE)
      {
        line++;
      }
      else if (PIXEL_IS_BLACK)
      {
        line++;
      }
      else
      {
        printf("KABOOM : Expertise(\"Unexpected color in map\") %d %d  FF:%d 00:%d B%d YW%d G%d R%d D%d L%d\n",line,pixel,PIXEL_IS_WHITE,PIXEL_IS_BLACK,PIXEL_IS_BLUE,PIXEL_IS_YELLOW,PIXEL_IS_GREEN,PIXEL_IS_RED,PIXEL_IS_DARK,PIXEL_IS_LIGHT);
        break;
      }
      not_done = ( (line < yscanh) && (pixel < xscanw) ) ? 1 : 0;
      if(!not_done)
      {
        if (line < yscanh-1)
        {
          pixel = xscans;
          line++;
          not_done = 1;
        }
      }
    }
  }
  return;
}

