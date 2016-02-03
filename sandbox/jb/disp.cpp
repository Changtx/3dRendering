/* 
*  disp.cpp -- definition file for Display
*  USC csci 580 
 * Student Name: Abdul Qadeer
*  email: aqadeer@usc.edu
*/

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "gz.h"
#include "disp.h"

/******************************************************************************/

int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes)
{
    //Do little sanity checking
    if (xRes <= 0 || yRes <= 0 || display == NULL)
    {
        printf("Bad input to %s.\n",__FUNCTION__);
        return GZ_FAILURE;
    }
    
    *display = new GzDisplay();
    if ( NULL == *display)
    {
        return GZ_FAILURE;
    }
    (*display)->dispClass = dispClass;
    (*display)->xres = xRes;
    (*display)->yres = yRes;
    //ToDo: Currently I am not sure what open means?
    (*display)->open = 0;
    
    
    (*display)->fbuf = new GzPixel[xRes * yRes];
    if ( NULL == (*display)->fbuf)
    {
        return GZ_FAILURE;
    }
    
    
    return GZ_SUCCESS;
}

/******************************************************************************/

int GzFreeDisplay(GzDisplay *display)
{
  /* clean up, free memory */
    if(NULL == display)
    {
        return GZ_FAILURE;
    }
    
    if (display->fbuf != NULL)
    {
        delete [] display->fbuf;
        display->fbuf = NULL;
    }
    delete display;
    display = NULL;
    return GZ_SUCCESS;
}

/******************************************************************************/

int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
  /* pass back values for an open display */
    
    if(NULL == display)
    {
        return GZ_FAILURE;
    }
    
    *xRes = display->xres;
    *yRes = display->yres;
    *dispClass = display->dispClass;

    return GZ_SUCCESS;
}

/******************************************************************************/

int GzInitDisplay(GzDisplay *display)
{
  /* set everything to some default values - start a new frame */
    if(NULL == display || NULL == display->fbuf || display->xres <= 0 || display->yres <= 0)
    {
        return GZ_FAILURE;
    }
    
    for(int y = 0; y < display->yres; y++)
    {
        for(int x = 0; x < display->xres; x++)
        {
            display->fbuf[ARRAY(x,y)].alpha = 1;
            display->fbuf[ARRAY(x,y)].blue = 1536;  
            display->fbuf[ARRAY(x,y)].green = 1792; 
            display->fbuf[ARRAY(x,y)].red = 2048;   
            display->fbuf[ARRAY(x,y)].z = INT_MAX;
        }
    }
    
    return GZ_SUCCESS;
}

/******************************************************************************/


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
  
    if(NULL == display || NULL == display->fbuf || i < 0 || i >= display->xres || j < 0 || j >= display->yres)
    {
        return GZ_FAILURE;
    }   
    
    
    display->fbuf[ARRAY(i,j)].red = r;
    display->fbuf[ARRAY(i,j)].green = g;
    display->fbuf[ARRAY(i,j)].blue = b;
    display->fbuf[ARRAY(i,j)].alpha = a;
    display->fbuf[ARRAY(i,j)].z = z;    

   return GZ_SUCCESS;
}

/******************************************************************************/


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{

    if(NULL == display || NULL == display->fbuf || i < 0 || i >= display->xres || j < 0 || j >= display->yres ||
       display->dispClass != GZ_RGBAZ_DISPLAY || NULL == r || NULL == g || NULL == b ||
       NULL == a || NULL == z)
    {
        return GZ_FAILURE;
    }
    
    *r = display->fbuf[ARRAY(i,j)].red;
    *g = display->fbuf[ARRAY(i,j)].green;
    *b = display->fbuf[ARRAY(i,j)].blue;
    
    *a = display->fbuf[ARRAY(i,j)].alpha;
    *z = display->fbuf[ARRAY(i,j)].z;   
    
    
    return GZ_SUCCESS;
}

/******************************************************************************/

char CalculateFlushableRGB(unsigned short s)
{
    unsigned short val = (s > MAX_CLR_VAL)?(MAX_CLR_VAL):(s); 
    return val >> 4;
}

/******************************************************************************/

/* References:
 * 
 (1) Info about pbm format read from: http://netpbm.sourceforge.net/doc/ppm.html
 (2) P3 means ASCII Text based PPM.  Needs spaces between r,g and b values.
 (3) P6 for raw (i.e binary) PPM.  Each r,g and b should be 8 bits here.
 (4) Info about PPM also from http://rosettacode.org/wiki/Bitmap/Write_a_PPM_file
 
 */

int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
    /* write pixels to ppm file based on display class -- "P3 %d %d 4096\r" */

    fprintf(outfile, "P3\n");
    fprintf(outfile, "%u %u\n", display->xres, display->yres);
    // int maxval = findmaxval(display->fbuf, display->xres*display->yres);
    int maxval = 4096;

    fprintf(outfile, "%u\n", maxval);
    for (int i = 0; i < display->yres; i++) {
    for (int j = 0; j < display->xres; j++) {
      fprintf(outfile, "%u ", display->fbuf[ARRAY(j, i)].red);
      fprintf(outfile, "%u ", display->fbuf[ARRAY(j, i)].green);
      fprintf(outfile, "%u ", display->fbuf[ARRAY(j, i)].blue);
    }
    fprintf(outfile, "\n");
    }

    return GZ_SUCCESS;

    ///////////////////////////////////////
    // This is Abdul's code which for some reason is in binary format (?!?!?)
    //    which is kind of hard to debug

    
  // /* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
  //   if(NULL == outfile || NULL == display || NULL == display->fbuf)
  //   {
  //       return GZ_FAILURE;
  //   }
    
  //   fprintf(outfile, "%s\n%d %d\n%d\n", "P6", display->xres, display->yres, 255);
  //   static unsigned char color[3];
    

  //   for (int y = 0; y < display->yres; y++)
  //   { 
  //       for (int x = 0; x < display->xres; x++) {           
            
            
            
  //           color[0] = CalculateFlushableRGB(display->fbuf[ARRAY(x,y)].red);
  //           color[1] = CalculateFlushableRGB(display->fbuf[ARRAY(x,y)].green);
  //           color[2] = CalculateFlushableRGB(display->fbuf[ARRAY(x,y)].blue);
  //           //printf("**** RGB %d %d %d     ****", color[0], color[1], color[2]);    
  //           fwrite(color, 1, 3, outfile);
            
  //       }
  //   }


  //   return GZ_SUCCESS;
}
/******************************************************************************/