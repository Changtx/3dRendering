#pragma once

class GzDisplay;

/* Camera defaults */
#define DEFAULT_FOV   35.0
#define DEFAULT_IM_Z  (-10.0)  /* world coords for image plane origin */
#define DEFAULT_IM_Y  (5.0)    /* default look-at point = 0,0,0 */
#define DEFAULT_IM_X  (-10.0)

#define DEFAULT_AMBIENT  {0.1, 0.1, 0.1}
#define DEFAULT_DIFFUSE  {0.7, 0.6, 0.5}
#define DEFAULT_SPECULAR {0.2, 0.3, 0.4}
#define DEFAULT_SPEC     32

#define MATLEVELS 100   /* how many matrix pushes allowed */
#define MAX_LIGHTS  10    /* how many lights allowed */

struct GzRender {     /* define a renderer */
  GzRenderClass renderClass;
  GzDisplay   *display;
  short       open;
  GzCamera    camera;
  short       matlevel;         /* top of stack - current xform */
  GzMatrix    Ximage[MATLEVELS];  /* stack of xforms (Xsm) */
  GzMatrix    Xnorm[MATLEVELS]; /* xforms for norms (Xim) */
  GzMatrix    Xsp;            /* NDC to screen (pers-to-screen) */
  GzColor   flatcolor;          /* color state for flat shaded triangles */
  int     interp_mode;
  int     numlights;
  GzLight   lights[MAX_LIGHTS];
  GzLight   ambientlight;
  GzColor   Ka, Kd, Ks;
  float       spec;   /* specular power */
  GzTexture   tex_fun;    /* tex_fun(float u, float v, GzColor color) */
  float   aashiftx, aashifty;
};

// Function declaration
// HW2
int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display);
int GzFreeRender(GzRender *render);
int GzBeginRender(GzRender  *render);
int GzPutAttribute(GzRender *render, int numAttributes, GzToken *nameList, 
  GzPointer *valueList);
int GzPutTriangle(GzRender *render, int numParts, GzToken *nameList,
  GzPointer *valueList);

// HW3
int GzPutCamera(GzRender *render, GzCamera *camera);
int GzPushMatrix(GzRender *render, GzMatrix matrix);
int GzPopMatrix(GzRender *render);

// Object Translation
int GzRotXMat(float degree, GzMatrix mat);
int GzRotYMat(float degree, GzMatrix mat);
int GzRotZMat(float degree, GzMatrix mat);
int GzTrxMat(GzCoord translate, GzMatrix mat);
int GzScaleMat(GzCoord scale, GzMatrix mat);


// My own helper functions
#define NORMAL_TRIANGLE 0
#define HORIZONTAL_TOP_EDGE 1
#define HORIZONTAL_BOTTOM_EDGE 2
#define DEGENERATE_TRIANGLE 3

short ctoi(float color);
int sortVertices(GzCoord* vertices, GzCoord* normals, GzTextureIndex* texture);
void swap(float** arr, int i, int j, int n);
int putNormalTriangle(GzRender* render, GzCoord* vertices, GzCoord* normals, GzTextureIndex* texture);
void getBoundingBox(GzCoord* vertices, float* minx, float* maxx, float* miny, float* maxy);
float min(float a, float b);
float max(float a, float b);
int round(float a);
int matmultiply(GzMatrix dest, GzMatrix a, GzMatrix b);
int transformPoints(GzCoord* vertex, GzMatrix transform);
int getColor(GzColor dest, GzRender* render, GzCoord vertex, GzCoord normal);
float dot(GzCoord a, GzCoord b);

void vmult(GzCoord dest, GzCoord a, GzCoord b);
void vdiv(GzCoord dest, GzCoord a, GzCoord b);
void vadd(GzCoord dest, GzCoord a, GzCoord b);
void vsub(GzCoord dest, GzCoord a, GzCoord b);
void vmultc(GzCoord dest, GzCoord a, float b);
void vdivc(GzCoord dest, GzCoord a, float b);
void vaddc(GzCoord dest, GzCoord a, float b);
void vsubc(GzCoord dest, GzCoord a, float b);
void vnorm(GzCoord a);
void vcopy(GzCoord dest, GzCoord a);
float vmag(GzCoord a);
void vcross(GzCoord dest, GzCoord a, GzCoord b);
void verr(GzCoord a);
int checkColor(float* color);
void barycentric(float* a, float* b, float*c, float x, float y, float z, GzCoord v_a, GzCoord v_b, GzCoord v_c);
int sign(float x);

int interpolateRGB(GzIntensity* r, GzIntensity* g, GzIntensity* b, float x, float y, float z, 
                GzCoord v_a, GzCoord v_b, GzCoord v_c, GzColor c_a, GzColor c_b, GzColor c_c);
int interpolateNormal(GzCoord normal, float x, float y, float z, 
                GzCoord v_a, GzCoord v_b, GzCoord v_c, GzCoord n_a, GzCoord n_b, GzCoord n_c);
int addAA(GzRender* render, GzCoord* vertices);