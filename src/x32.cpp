
#include "yzzyx.h"

static void do_histo( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] , int histo , int shfta , int mord )
{
//foreachscanline
  for (byte4 line = 0; line < bmp->height; line++)
  {
    //foreachpixelonline
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB4& map = bmp->bitmapcolors.r4[line * bmp->width + pixel];
      byte rd = map.red       >> shfta;
      byte gr = map.green     >> shfta;
      byte bl = map.blue      >> shfta;
      byte in = map.intensity >> shfta;
      histoz[histo][in][rd][gr][bl]++;
    }
  }
  display_histo(histoz,histo,shfta,mord,1);
}

static void do_histos( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] )
{
  do_histo(bmp,histoz,0,7, 2);
  do_histo(bmp,histoz,1,6, 4);
  do_histo(bmp,histoz,2,5, 8);
  do_histo(bmp,histoz,3,4,16);
  do_histo(bmp,histoz,4,3,32);
}

void x32_histo( PLAINBITMAP * bmp , int histoz[5][33][33][33][33] )
{
  do_histos(bmp, histoz);
}

void x32_color( PLAINBITMAP * bmp, int xedgecropl, int xedgecropr, int yedgecropb, int yedgecropt , int xcellmin, int ycellmin, int * rectangles_extracted  )
{
  int           hdrsize       = bmp->filesize - bmp->datasize;
  int           hdrkick       = (hdrsize==70) ? 5 : 0;
  RGB34       * bitmapcolors  = (RGB34*)&bmp->bitmapcolors.r4[0];

//
  if (bmp->bitsperpixel!=32) return;
  if (((bmp->bitsperpixel==32) && (hdrkick==5)))
  {
      bitmapcolors->r4[0].intensity = 0;
      bitmapcolors->r4[1].intensity = 0;
      bitmapcolors->r4[2].intensity = 0;
      bitmapcolors->r4[3].intensity = 0;
      bitmapcolors->r4[0].red       = 0;
      bitmapcolors->r4[1].red       = 0;
      bitmapcolors->r4[2].red       = 255;
      bitmapcolors->r4[3].red       = 0;
      bitmapcolors->r4[0].green     = 0;
      bitmapcolors->r4[1].green     = 255;
      bitmapcolors->r4[2].green     = 0;
      bitmapcolors->r4[3].green     = 0;
      bitmapcolors->r4[0].blue      = 255;
      bitmapcolors->r4[1].blue      = 0;
      bitmapcolors->r4[2].blue      = 0;
      bitmapcolors->r4[3].blue      = 0;
      // byte * cp = (byte*)bitmapcolors;
         // cp = &cp[16];
         // bitmapcolors = (RGB34 *)cp;
      bitmapcolors  = (RGB34*)&bmp->bitmapcolors.r4[4];
  }



  #define rgbmap(map,r,g,b,R,G,B) if(rd==r&&gr==g&&bl==b)  { map.red=R; map.green = G; map.blue  = B; }

  RGB3 * rgb3 = (RGB3 *)&bmp->bitmapcolors;//(RGB3*)&bitmapcolors->r3[0];
  //looking for dark text on a light background
  if(bmp->bitsperpixel==24)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB3 map = rgb3[rgbord + pixel];
      byte rd = map.red   >> 4;
      byte gr = map.green >> 4;
      byte bl = map.blue  >> 4;

      //map none text to intensity
      if(rd>12&&gr>12&&bl>12)
      {
        rgb3[rgbord + pixel].red   = 0;
        rgb3[rgbord + pixel].green = 255;
        rgb3[rgbord + pixel].blue  = 0;
      }

      if(map.red==0&&map.green==255&&map.blue==0)
      {
      //  map.red   = 0;
      //  map.green = 0;
      //  map.blue  = 0;
      }
      else
      {
       // map.red   = 255;
       // map.green = 255;
       // map.blue  = 255;
      }
    }
  }

  if(bmp->bitsperpixel==32&&hdrkick==0)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGBI& map = bitmapcolors->i4[rgbord + pixel];

      //create remapped color 0-15  256>>4
      byte it = map.intensity >> 4 ;
      byte rd = map.red       >> 4 ;
      byte gr = map.green     >> 4 ;
      byte bl = map.blue      >> 4 ;

      //map none text to intensity
      if(rd>12&&gr>12&&bl>12)
      {
        map.intensity = 255;
        map.red       = 0;
        map.green     = 0;
        map.blue      = 0;
      }
      //
      //other
      //
      if(map.intensity==255&&map.red==0&&map.green==0&&map.blue==0)
      {
        map.intensity = 0;
        map.red       = 0;
        map.green     = 0;
        map.blue      = 0;
      }
      else
      {
        map.intensity = 0;
        map.red       = 255;
        map.green     = 255;
        map.blue      = 255;
      }
    }
  }

  byte * cp = (byte*)&bmp->bitmapcolors;
  cp = &cp[16];
  RGB4 * rgb4 = (RGB4 *)cp;

  if(bmp->bitsperpixel==32&&hdrkick!=0)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB4& map = rgb4[rgbord + pixel];

      //create remapped color 0-15  256>>4
      byte rd = map.red       >> 4 ;
      byte gr = map.green     >> 4 ;
      byte bl = map.blue      >> 4 ;

      //map none text to intensity
      if(rd>12&&gr>12&&bl>12)
      {
        map.red       = 0;
        map.green     = 255;
        map.blue      = 0;
      }
      //
      //other
      //
      if(map.red==0&&map.green==255&&map.blue==0)
      {
        map.red       = 0;
        map.green     = 0;
        map.blue      = 0;
      }
      else
      {
        map.intensity = 0;
        map.red       = 255;
        map.green     = 255;
        map.blue      = 255;
      }
    }
  }

  //after the above the bitmap should have two color values  text(white) or not text(black)


  int line_stack[640];
  int pixel_stack[480];
  line_stack[0] = 0;
  pixel_stack[0] = 0;
  //after bitification there is room for 3 (24b)  or 4 (32b) copies  1 x W x H


  //disect memory image
  if(bmp->bitsperpixel==24)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    int abandon = -1;
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB3& map = bitmapcolors->r3[rgbord + pixel];
      if(map.red||map.green||map.blue)
      {
        abandon=pixel;
        break;
      }
      map.red = 255;
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
      size_t rgbord = line * bmp->width;
      for (byte4 pixel = 0; pixel < (byte4) abandon; pixel++)
      {
        RGB3& map = bitmapcolors->r3[rgbord  + pixel];
        map.red = 0;
      }
    }
  }
  if(bmp->bitsperpixel==32&&hdrkick==0)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    int abandon = -1;
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGBI& map = bitmapcolors->i4[rgbord++];
      if(map.red||map.green||map.blue)
      {
        abandon=pixel;
        break;
      }
      map.red = 255;
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
      size_t rgbord = line * bmp->width;
      for (byte4 pixel = 0; pixel <  (byte4) abandon; pixel++)
      {
        RGBI& map = bitmapcolors->i4[rgbord++];
        map.red = 0;
      }
    }
  }

  if(bmp->bitsperpixel==32&&hdrkick!=0)
  for (byte4 line = 0; line < bmp->height; line++)
  {
    int abandon = -1;
    size_t rgbord = line * bmp->width;
    for (byte4 pixel = 0; pixel < bmp->width; pixel++)
    {
      RGB4& map = rgb4[rgbord+pixel];
      if(map.red||map.green||map.blue)
      {
        abandon=pixel;
        break;
      }
      map.red = 255;
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
      size_t rgbord = line * bmp->width;
      for (byte4 pixel = 0; pixel <  (byte4)abandon; pixel++)
      {
        RGB4& map = bitmapcolors->r4[rgbord+pixel];
        map.red = 0;
      }
    }
  }

  line_stack[++line_stack[0]] = bmp->height;
  printf("LINE  %u ", line_stack[0]);for(int ord=1;ord<=line_stack[0];ord++) printf(" %u ",line_stack[ord]);  printf("\n");

  if(bmp->bitsperpixel==24)
  for (byte4 pixel = 0; pixel < bmp->width; pixel++)
  {
    int abandon = -1;
    for (byte4 line = 0; line < bmp->height; line++)
    {
      size_t rgbord = line * bmp->width + pixel;
      RGB3& map = bitmapcolors->r3[rgbord];
      if((map.red||map.green||map.blue)&&(!(map.red==255&&map.green==0&&map.blue==0)))
      {
        abandon = line;
        break;
      }
      if (map.red==0&&map.green==0&&map.blue==0)
      {
        map.blue = 255;
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
      for (byte4 line = 0; line <  (byte4)abandon; line++)
      {
        size_t rgbord = line * bmp->width + pixel;
        RGB3& map = bitmapcolors->r3[rgbord];
        if (map.red==0&&map.green==0&&map.blue==255) map.blue = 0;
      }
    }
  }
  if(bmp->bitsperpixel==32&&hdrkick==0)
  for (byte4 pixel = 0; pixel < bmp->width; pixel++)
  {
    int abandon = -1;
    for (byte4 line = 0; line < bmp->height; line++)
    {
      size_t rgbord = line * bmp->width + pixel;
      RGBI& map = bitmapcolors->i4[rgbord];
      if((map.red||map.green||map.blue)&&(!(map.red==255&&map.green==0&&map.blue==0)))
      {
        abandon = line;
        break;
      }
      if (map.red==0&&map.green==0&&map.blue==0)
      {
        map.blue = 255;
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
      for (byte4 line = 0; line <  (byte4)abandon; line++)
      {
        size_t rgbord = line * bmp->width + pixel;
        RGBI& map = bitmapcolors->i4[rgbord];
        if (map.red==0&&map.green==0&&map.blue==255) map.blue = 0;
      }
    }
  }
  if(bmp->bitsperpixel==32&&hdrkick!=0)
  for (byte4 pixel = 0; pixel < bmp->width; pixel++)
  {
    int abandon = -1;
    for (byte4 line = 0; line < bmp->height; line++)
    {
      size_t rgbord = line * bmp->width + pixel;
      RGB4& map = bitmapcolors->r4[rgbord];
      if((map.red||map.green||map.blue)&&(!(map.red==255&&map.green==0&&map.blue==0)))
      {
        abandon = line;
        break;
      }
      if (map.red==0&&map.green==0&&map.blue==0)
      {
        map.blue = 255;
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
      for (byte4 line = 0; line <  (byte4)abandon; line++)
      {
        size_t rgbord = line * bmp->width + pixel;
        RGB4& map = bitmapcolors->r4[rgbord];
        if (map.red==0&&map.green==0&&map.blue==255) map.blue = 0;
      }
    }
  }


  printf("PIXEL %u ", pixel_stack[0]);for(int ord=1;ord<=pixel_stack[0];ord++) printf(" %u ",pixel_stack[ord]);  printf("\n");

  if(bmp->bitsperpixel==24)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    printf("LINEPIXEL  %7u %7u  ",line,pixel);
    printf("ROWCOLUMN  %7u, %7u  %7u, %7u\n",row,col,line_stack[line+1]-1,pixel_stack[pixel+1]);
    int fillall = 0;
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGB3& mapA = bitmapcolors->r3[rgbord];
        RGB3& mapB = bitmapcolors->r3[rgbord+1];
        RGB3& mapC = bitmapcolors->r3[rgbord+2];
        RGB3& mapD = bitmapcolors->r3[rgbord+3];
        if(               ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0)))  ||
           ((fillall==0)&&((mapB.red||mapB.green||mapB.blue)&&(!(mapB.red==255&&mapB.green==0&&mapB.blue==0)))) ||
           ((fillall==0)&&((mapC.red||mapC.green||mapC.blue)&&(!(mapC.red==255&&mapC.green==0&&mapC.blue==0)))) ||
           ((fillall==0)&&((mapD.red||mapD.green||mapD.blue)&&(!(mapD.red==255&&mapD.green==0&&mapD.blue==0)))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;
        }
        if (fillall==0)
        {
          mapB.green = 255;
          mapC.green = 255;
          mapD.green = 255;
        }
      }
      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
        if(fillall==0) { x++;x++;x++;}
        fillall = 1;
      }
      else
      {
        if(fillall==1)
          fillall = 0;
        inout = 0;
        //undraw
        for (byte4 y = row; y <  (byte4) abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGB3& mapA = bitmapcolors->r3[rgbord];
          RGB3& mapB = bitmapcolors->r3[rgbord+1];
          RGB3& mapC = bitmapcolors->r3[rgbord+2];
          RGB3& mapD = bitmapcolors->r3[rgbord+3];
                         if (mapA.red==0&&mapA.green==255&&mapA.blue==0) mapA.green = 0;
          if (fillall==0)
          {
            if (mapB.red==0&&mapB.green==255&&mapB.blue==0) mapB.green = 0;
            if (mapC.red==0&&mapC.green==255&&mapC.blue==0) mapC.green = 0;
            if (mapD.red==0&&mapD.green==255&&mapD.blue==0) mapD.green = 0;
          }
       }
      }
    }
  }


  if(bmp->bitsperpixel==32&&hdrkick==0)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    printf("LINEPIXEL  %7u %7u  ",line,pixel);
    printf("ROWCOLUMN  %7u, %7u  %7u, %7u\n",row,col,line_stack[line+1]-1,pixel_stack[pixel+1]);
    int fillall = 0;
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGBI& mapA = bitmapcolors->i4[rgbord];
        RGBI& mapB = bitmapcolors->i4[rgbord+1];
        RGBI& mapC = bitmapcolors->i4[rgbord+2];
        RGBI& mapD = bitmapcolors->i4[rgbord+3];
        if(               ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0)))  ||
           ((fillall==0)&&((mapB.red||mapB.green||mapB.blue)&&(!(mapB.red==255&&mapB.green==0&&mapB.blue==0)))) ||
           ((fillall==0)&&((mapC.red||mapC.green||mapC.blue)&&(!(mapC.red==255&&mapC.green==0&&mapC.blue==0)))) ||
           ((fillall==0)&&((mapD.red||mapD.green||mapD.blue)&&(!(mapD.red==255&&mapD.green==0&&mapD.blue==0)))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;
        }
        if (fillall==0)
        {
          mapB.green = 255;
          mapC.green = 255;
          mapD.green = 255;
        }
      }

      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
        if(fillall==0) { x++;x++;x++;}
        fillall = 1;
      }
      else
      {
        if(fillall==1)
          fillall = 0;
        inout = 0;
        //undraw
        for (byte4 y = row; y <  (byte4)abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGBI& mapA = bitmapcolors->i4[rgbord];
          RGBI& mapB = bitmapcolors->i4[rgbord+1];
          RGBI& mapC = bitmapcolors->i4[rgbord+2];
          RGBI& mapD = bitmapcolors->i4[rgbord+3];
                         if (mapA.red==0&&mapA.green==255&&mapA.blue==0) mapA.green = 0;
          if (fillall==0)
          {
            if (mapB.red==0&&mapB.green==255&&mapB.blue==0) mapB.green = 0;
            if (mapC.red==0&&mapC.green==255&&mapC.blue==0) mapC.green = 0;
            if (mapD.red==0&&mapD.green==255&&mapD.blue==0) mapD.green = 0;
          }
       }
      }
    }
  }
  if(bmp->bitsperpixel==32&&hdrkick!=0)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    printf("LINEPIXEL  %7u %7u  ",line,pixel);
    printf("ROWCOLUMN  %7u, %7u  %7u, %7u\n",row,col,line_stack[line+1]-1,pixel_stack[pixel+1]);
    int fillall = 0;
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGB4& mapA = rgb4[rgbord];
        RGB4& mapB = rgb4[rgbord+1];
        RGB4& mapC = rgb4[rgbord+2];
        RGB4& mapD = rgb4[rgbord+3];
        if(               ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0)))  ||
           ((fillall==0)&&((mapB.red||mapB.green||mapB.blue)&&(!(mapB.red==255&&mapB.green==0&&mapB.blue==0)))) ||
           ((fillall==0)&&((mapC.red||mapC.green||mapC.blue)&&(!(mapC.red==255&&mapC.green==0&&mapC.blue==0)))) ||
           ((fillall==0)&&((mapD.red||mapD.green||mapD.blue)&&(!(mapD.red==255&&mapD.green==0&&mapD.blue==0)))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;
        }
        if (fillall==0)
        {
          mapB.green = 255;
          mapC.green = 255;
          mapD.green = 255;
        }
      }
      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
        if(fillall==0) { x++;x++;x++;}
        fillall = 1;
      }
      else
      {
        if(fillall==1)
          fillall = 0;
        inout = 0;
        //undraw
        for (byte4 y = row; y < (byte4)abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGB4& mapA = rgb4[rgbord];
          RGB4& mapB = rgb4[rgbord+1];
          RGB4& mapC = rgb4[rgbord+2];
          RGB4& mapD = rgb4[rgbord+3];
                         if (mapA.red==0&&mapA.green==255&&mapA.blue==0) mapA.green = 0;
          if (fillall==0)
          {
            if (mapB.red==0&&mapB.green==255&&mapB.blue==0) mapB.green = 0;
            if (mapC.red==0&&mapC.green==255&&mapC.blue==0) mapC.green = 0;
            if (mapD.red==0&&mapD.green==255&&mapD.blue==0) mapD.green = 0;
          }
       }
      }
    }
  }

  if(bmp->bitsperpixel==24)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGB3& mapA = bitmapcolors->r3[rgbord];
        if( ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;mapA.red = 255;
        }
      }
      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
      }
      else
      {
        inout = 0;
        //undraw
        for (byte4 y = row; y < (byte4)abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGB3& mapA = bitmapcolors->r3[rgbord];
          if (mapA.red==255&&mapA.green==255&&mapA.blue==0) { mapA.green = 0; mapA.red = 0; }
       }
      }
    }
  }


  if(bmp->bitsperpixel==32&&hdrkick==0)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGBI& mapA = bitmapcolors->i4[rgbord];
        if( ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;mapA.red = 255;
        }
      }
      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
      }
      else
      {
        inout = 0;
        //undraw
        for (byte4 y = row; y <  (byte4)abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGBI& mapA = bitmapcolors->i4[rgbord];
          if (mapA.red==255&&mapA.green==255&&mapA.blue==0) { mapA.green = 0; mapA.red = 0; }
       }
      }
    }
  }


  if(bmp->bitsperpixel==32&&hdrkick!=0)
  for(int pixel=1;pixel<=pixel_stack[0];pixel++)
  for(int line=1;line<=line_stack[0];line++)
  {
    int row = line_stack[line];
    int col = pixel_stack[pixel];
    for(int x = col; x < pixel_stack[pixel+1]; x++)
    {
      int abandon = -1;
      for(int y = row; y < line_stack[line+1]-1; y++)
      {
        size_t rgbord = y * bmp->width + x;
        RGB4& mapA = rgb4[rgbord];
        if( ((mapA.red||mapA.green||mapA.blue)&&(!(mapA.red==255&&mapA.green==0&&mapA.blue==0))))
        {
          abandon = y;
          break;
        }
        if(mapA.red==255&&mapA.green==0&&mapA.blue==0)
        {
          break;
        }
        if (mapA.red==0&&mapA.green==0&&mapA.blue==0)
        {
          mapA.green = 255;mapA.red = 255;
        }
      }
      static int inout = 0;
      if(abandon==-1)
      {
        inout = 1;
      }
      else
      {
        inout = 0;
        //undraw
        for (byte4 y = row; y < (byte4)abandon; y++)
        {
          size_t rgbord = y * bmp->width + x;
          RGB4& mapA = rgb4[rgbord];
          if (mapA.red==255&&mapA.green==255&&mapA.blue==0) { mapA.green = 0; mapA.red = 0; }
       }
      }
    }
  }
}

