/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/

#ifndef DISP_H
#define	DISP_H

#include <stdio.h>
#include "gz.h"
//#include "Helper.h"


/* define general display pixel-type */
#ifndef GZ_PIXEL
typedef	struct {
  GzIntensity    red;
  GzIntensity    green;
  GzIntensity    blue;
  GzIntensity    alpha;
  GzDepth	 z;
} GzPixel;
#define GZ_PIXEL
#endif


/* define a display type */
#ifndef GZ_DISPLAY
class  GzDisplay{
public:
  unsigned short	xres;
  unsigned short	yres;
  GzDisplayClass	dispClass;
  short			open;
  GzPixel		*fbuf;		/* frame buffer array */
};
#define GZ_DISPLAY
#endif

#define	MAXXRES	1024	/* put some bounds on size in case of error */
#define	MAXYRES	1024

#define	ARRAY(x,y)	(x+(y*display->xres))	/* simplify fbuf indexing */

#define MAX_CLR_VAL 4095 //Maximum value which can fit in 12 bits

// Function declaration
int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes);
int GzFreeDisplay(GzDisplay *display);
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass);
int GzInitDisplay(GzDisplay *display);
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z);
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display);
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay* display);
char CalculateFlushableRGB(unsigned short s);

#endif	/* DISP_H */