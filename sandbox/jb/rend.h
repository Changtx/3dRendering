#pragma once

#include <limits.h>

#include "Stack.h"

class GzDisplay;

/* Camera defaults */
#define	DEFAULT_FOV	35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT  {0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE  {0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR {0.2, 0.3, 0.4}
#define	DEFAULT_SPEC     32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

#define GzLight		GzPointer
#define GzLightSize     (sizeof(float[2][3]))
#define GzTexture	GzPointer

#define Z_MAX   (INT_MAX)

class GzRender {                               /* define a renderer */
public:
  GzRenderClass         renderClass;
  GzDisplay		*display;
  short                 open;
  GzCamera		camera;
  short                 matlevel;	        /* top of stack - current xform */
  GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
  GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
  GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
  GzColor		flatcolor;              /* color state for flat shaded triangles */
  int			interp_mode;
  int			numlights;
  GzLight		lights[MAX_LIGHTS];
  GzLight		ambientlight;
  GzColor		Ka, Kd, Ks;
  float     km_ka, km_kd, km_ks;
  float                 spec;                   /* specular power */
  GzTexture		tex_fun;                /* tex_fun(float u, float v, GzColor color) */
  Stack                 xStack;                 /* My own stack.  Not using Ximage*/
  Stack                 nStack;                 /* My own norms stack*/
  
  float                 aa_delta_x;             /* Anti aliasing delta x and y*/
  float                 aa_delta_y;
 
public:
    GzRender()
    {
        this->numlights = 0;
        this->aa_delta_x = 0;
        this->aa_delta_y = 0;
    }
};

// Function declaration
// HW2
int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display);
int GzFreeRender(GzRender *render);
int GzBeginRender(GzRender	*render);
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList);
int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList);

// HW3
int GzPutCamera(GzRender *render, GzCamera *camera);
int GzPushMatrix(GzRender *render, GzMatrix matrix, bool bothStacks);
int GzPopMatrix(GzRender *render);
// Transformation functions moved to Transformation.h for clarity.

// HW4
void calc_ambient(GzRender *render, GzColor col, bool mulByK);
void calc_diffuse(GzRender *render, GzCoord N, GzColor col, bool mulByK);
void calc_specular(GzRender *render, GzCoord N, GzColor col, bool mulByK);
void calc_color(GzRender *render, GzCoord N_orig, GzColor col, bool mulByK);
void addThreeColors(GzColor a, GzColor b, GzColor c, GzColor r);
bool shouldCalcLight(GzCoord N, GzCoord L, GzCoord E, GzCoord Nnew);

// HW5
void calcTexture(GzRender *render, GzTextureIndex *triTextures, GzCoord *c, 
                 float newZ, float alpha, float beta, float gamma, GzColor color);
int tex_fun(float u, float v, GzColor color);


//Helper functions
float findMin(GzCoord *array, int dim, int size);
float findMax(GzCoord *array, int dim, int size);
void printArray(GzCoord *array, int dim, int size);
bool isBehindViewPlane(GzRender *render, GzCoord *triVertices);
bool isClipped(GzRender *render, GzCoord *triVertices);
short ctoi(float color);

