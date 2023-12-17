//============================================================================
// Name        : yzzyx.cpp
// Author      : nsd
// Version     : 0.1.0
// Copyright   : work-in-progress
// Description : extract text out of a image rendering (perfect format)
//               which is a portion of a image file (subdivide and recurse)
//               that is laid out as determined by expertise
//============================================================================

#include "yzzyx.h"

void display_histo(int histoz[5][33][33][33][33],int histo,int shfta,int mord,int inb)
{
  for( int hw = 0;  hw < ((inb) ? mord : 1) ; hw++)//inb 0 == rgb   1 == rgbi
  {
    for (int hx = 0; hx < mord; hx++)
    {
      printf("%6u ", hx << shfta);
      for (int hz = 0; hz < mord; hz++)
        printf("%6u ", hz << shfta);
      printf("\n");
      for (int hy = 0; hy < mord; hy++)
      {
        printf("%6u ", hy << shfta);
        for (int hz = 0; hz < mord; hz++)
        {
          printf("%6u ", histoz[histo][hw][hx][hy][hz]);
        }
        printf("\n");
      }
      printf("\n");
    }
    printf("\n");
  }
  int color = 1;

  for (int hw = 0; hw < ((inb) ? mord : 1) ; hw++)
  {
    for (int hx = 0; hx < mord; hx++)
    {
      for (int hy = 0; hy < mord; hy++)
      {
        for (int hz = 0; hz < mord; hz++)
        {
          if(histoz[histo][hw][hx][hy][hz]) printf("%5d %6u %3u %3u %3u %3u\n", color++, histoz[histo][hw][hx][hy][hz],hw,hx,hy,hz);
        }
      }
    }
  }
}

char * tempfile(void)
{
  static char bfr[1024];
  sprintf(bfr, "/tmp/temp-file-pid-%u-%d",getpid(),rand());
  return bfr;
}

void error_exit(int retc)
{
  exit(-1);
}

int mime_type_is(char*filename, const char * mime_token)
{
  return 0;
}

//two clp
//file to be processed  scanned-image
//file to be created    text-rendering

int swc(int fd, PLAINBITMAP* mp, size_t len)
{
  size_t wr = lseek(fd, 0, SEEK_SET);  wr = write(fd, mp, len);  lseek(fd, 0, SEEK_SET);
  close (fd);
  return  (wr != len) ? 6 : 0;
}
//open  -  allocate  -  read
int oar(char *file_to_use,int *bmp_fd, PLAINBITMAP**bmp_mp, size_t *bmp_len)
{
  int           fd  = open(file_to_use, O_RDWR);
  if (fd < 0)      return(1);
  long int      len = lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);  if (len <= 0)    return(2);
  void *        rgb = malloc(len);                                if (rgb == 0)    { close(fd); return(3); }
  long int      br  = read(fd,rgb,len);lseek(fd,0,SEEK_SET);      if (br != len)   { close(fd); free(rgb); return(4); }
  *bmp_mp  = (PLAINBITMAP *)rgb;
  *bmp_fd  = fd;
  *bmp_len = len;
  return 0;
}

int main(int argc, char**args)
{

  if(argc<3) error_exit(0);
  int rectangles_extracted = 0;
  int xcellmin    = 1;
  int ycellmin    = 1;
  int xedgecropl  = (argc>3) ? atoi(args[3]) : 1;
  int yedgecropb  = (argc>4) ? atoi(args[4]) : xedgecropl;
  int xedgecropr  = (argc>5) ? atoi(args[5]) : xedgecropl;
  int yedgecropt  = (argc>6) ? atoi(args[6]) : yedgecropb;

  //the main process produces a stream of bitmaps
  //that have the 'textual' content of the scanned
  //image  this stream is processed forward in the
	//pipeline

  //configure run-time parameters

  //open and read image
  //determine if file needs to be preprocessed
  //decompression / flattening
  //if so replace args[1] with the fullname
  char * file_to_use  = args[1];
  char * file_used    = args[1];

  if ( mime_type_is(file_to_use, "tiff") )
  {
    char clb[1024];
    char * file_to_mime = file_to_use;
    file_to_use = strdup(tempfile());
    sprintf(clb,"tiff-convertor --in=%s --out=%s", file_to_mime, file_to_use);
    system(clb);
  }//if else ....


  char curpath[] = "/home/mike/Desktop/";
  char bfr[1024];
  sprintf(bfr, "%swork-%s",curpath,file_used);
  file_to_use =  strdup(bfr);
  sprintf(bfr,"cd %s ; cp %s %s", curpath, file_used, file_to_use);
  system(bfr);
  // file_to_use = dtc(file_used);

  //single plane without palette
  //bmps are understood
  //top of screen -- toward end of file
  //open allocate and read plain bmp
  int           bmp_fd  = 0;
  size_t        bmp_len = 0;
  PLAINBITMAP * bmp_mp  = 0;
  int           bmp_rc  = oar(file_to_use, &bmp_fd, &bmp_mp, &bmp_len); if(bmp_rc) error_exit(2);


                //assume image has RGB 3 or 4 bytes ? just 4   8 bits per color gun
                //[0] is 2*2      2 values per color gun total 8   colors
                //[1] is 4*4      4            color           64
                //[2] is 8*8      8
                //[3] is 16*16    16
                //[4] is 32*32    32
                //color histo
  int           intensity = (bmp_mp->bitsperpixel==24) ? 1 : 33;
  int           histoz[5][33][33][33][33];
                RGB_HISTO(bmp_mp, histoz);

                //if no seperation reject bmp  `twin peaks'
                //   *
                //  * *             *
                //  *  * * * * * * * *
                // background     text
                // match sets of large but different range numer   393939322 background    2424424 text   3325 scrunge    1 anomaly
                // match sets of large but different range numer   333567344 background    2859844 text   1255 scrunge
                // match sets of large but different range numer   396363663 background
                // can the number of different colors in raw image be represented in word size of color ordinals in unified image
                //on an x64 an int is as big as or bigger than 32 bit rgbi bmp
                //over do it pad the size up machine word
                //unified image is an height * width array of machine words (typically unsigned int but surely (size of void*))
                //and some 1024 extra
                //speed over memory optimization

  ssize_t       area  = 1024 + bmp_mp->height*bmp_mp->width*sizeof(void*);
  void *        mem   = malloc(area);                                 if (mem == 0)    error_exit(5);

  //translate image to memory 1 x W x H  (white text on a black background)
  //determine black/white

  //unify_image(mem,rgb);
  //create acceptable pallete
  //goto

  //memcpy(mem,rgb,len);

    //rows in an rgb bmp are dword aligned 1 2 3 pixel mod 4
    //stride = (((bmp->width*3)+3)&(~3));
    //             extra 4 color entries gun mapping?                                             i  r  g  b
    //                                   1 which position in the 4 bytes is the red       gun    00 ff 00 00
    //                                   2 which position in the 4 bytes is the green     gun    00 00 ff 00
    //                                   3 which position in the 4 bytes is the blue      gun    00 00 00 ff
    //                                   4 which position in the 4 bytes is the intensity gun    00 00 00 00 no intensity???


  int expected_count_of_extracted_rectangles = 1;

  int done = 0;
  int cont = 1;

  //set special flag
  bmp_mp->_0600[0] = 0;
  //CLR_IMAGE
  // does recoloring passes on bitmap making two-color image
  // attempts to detect text lines
  // grids texts
  // output words and chars as "01" rectangles when found
  do
  {

    //xxx_color(bmp, xedgecropl, xedgecropr, yedgecropb, yedgecropt, xcellmin, ycellmin, &rectangles_extracted)
    //if CLR_IMAGE presented with perfect 2 color horizontal text
    //then CLR_IMAGE will output a RECTANGLE for each of the characters

    //RECTANGLE(000179) L 1537 P  321 CU 1561 CO  336 HD  339 WD  879 HR   24 WR   15
    //000000000000000
    //000000111110000
    //000000111111000
    //000000111111000
    //000000000111000
    //000011110111000
    //011111111111000
    //011111111111000
    //111111111111000
    //111100000111000
    //111000000111000
    //111000000111000
    //111000000111000
    //111000000111000
    //111100001111000
    //111111111111111
    //011111111111111
    //011111111111111
    //000011100001000
    //000000000000000
    //000000000000000
    //000000000000000
    //000000000000000
    //000000000000000

    CLR_IMAGE(bmp_mp);

    done = ( rectangles_extracted >= expected_count_of_extracted_rectangles ) ? 1 : 0;

    cont = 0;
    //cont = ( !done ) ? XFRM_IMAGE(bmp_mp,step,value) : cont;
    //
    //
    //if done move on

  } while(!done && cont);


  if(!done)
  {
    //update strategy
  }

  //dump_text_bitmaps();
  bmp_rc = swc(bmp_fd,bmp_mp,bmp_len);

  //adjust configuration

  //report

	return 0;
}
