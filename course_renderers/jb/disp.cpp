/* 
*  disp.cpp -- definition file for Display
*  USC csci 580 
*/

#include "gz.h"
#include "disp.h"

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
  /* create a framebuffer:
   -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
   -- pass back pointer
  */

  GzPixel* fbuf = new GzPixel[width*height];
  *framebuffer = (char*) fbuf;

  return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes)
{
  /* create a display:
   -- allocate memory for indicated class and resolution
   -- pass back pointer to GzDisplay object in display
   */
  
  GzDisplay* disp = new GzDisplay();

  disp->dispClass = dispClass;
  disp->xres = (unsigned short) xRes;
  disp->yres = (unsigned short) yRes;
  disp->open = false;
  GzNewFrameBuffer((char **) &(disp->fbuf), xRes, yRes);

  *display = disp;

  return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay *display)
{
  /* clean up, free memory */

  delete[] display->fbuf;
  delete display;

  return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass *dispClass)
{
  /* pass back values for an open display */

  *xRes = display->xres;
  *yRes = display->yres;
  dispClass = &display->dispClass;

  return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay *display)
{
  /* set everything to some default values - start a new frame */

  GzPixel* fbuf = display->fbuf;
  for (int i = 0; i < display->xres; i++)
  {
    for (int j = 0; j < display->yres; j++)
    {
      fbuf[ARRAY(i, j)].red = 2048;
      fbuf[ARRAY(i, j)].green = 1792;
      fbuf[ARRAY(i, j)].blue = 1536;
      // fbuf[ARRAY(i, j)].red = 4000;
      // fbuf[ARRAY(i, j)].green = 4000;
      // fbuf[ARRAY(i, j)].blue = 4000;
      fbuf[ARRAY(i, j)].alpha = 1;
      fbuf[ARRAY(i, j)].z = MAXIMUM_Z;
    }
  }

  display->open = true;

  return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
  /* write pixel values into the display */

  if (!display->open) {
    return GZ_FAILURE;
  }

  if (i < 0 || i >= display->xres) {
    return GZ_FAILURE;
  }

  if (j < 0 || j >= display->yres) {
    return GZ_FAILURE;
  }

  display->fbuf[ARRAY(i,j)].red = r;  
  display->fbuf[ARRAY(i,j)].green = g;  
  display->fbuf[ARRAY(i,j)].blue = b;
  display->fbuf[ARRAY(i,j)].alpha = a;
  display->fbuf[ARRAY(i,j)].z = z;

  return GZ_SUCCESS;
}

int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
  /* pass back pixel value in the display */
  /* check display class to see what vars are valid */
  
  if (!display->open) {
    return GZ_FAILURE;
  }

  if (i < 0 || i >= display->xres) {
    return GZ_FAILURE;
  }

  if (j < 0 || j >= display->yres) {
    return GZ_FAILURE;
  }

  *r = display->fbuf[ARRAY(i,j)].red;
  *g = display->fbuf[ARRAY(i,j)].green;
  *b = display->fbuf[ARRAY(i,j)].blue;
  *a = display->fbuf[ARRAY(i,j)].alpha;
  *z = display->fbuf[ARRAY(i,j)].z;

  return GZ_SUCCESS;
}

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
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
  /* write pixels to framebuffer:
   - Put the pixels into the frame buffer
   - Caution: store the pixel to the frame buffer as the order of blue, green, and red
   - Not red, green, and blue !!!
  */
  
  return GZ_FAILURE;
}
