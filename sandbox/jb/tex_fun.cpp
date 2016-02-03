/* Texture functions for cs580 GzLib	*/
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include	"gz.h"
#include "Transformations.h"
#include "vectormath.h"

#define RED_SCALE 6
#define GREEN_SCALE 6
#define BLUE_SCALE 6

GzColor	**image;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color) {

    
    unsigned char pixel[3];
    unsigned char dummy;
    char foo[8];
    FILE *fd;


    if (reset) /* open and load texture file */
    { 
        fd = fopen("texture", "rb");
        if (fd == NULL) {
            fprintf(stderr, "texture file not found\n");
            return GZ_FAILURE;
        }
        fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);

        image = new GzColor*[ys]();
        for(int i = 0; i < ys; i++)
            image[i] = new GzColor[xs]();

        if (image == NULL) {
            fprintf(stderr, "Memory allocation for texture image failed.\n");
            return GZ_FAILURE;
        }


        for (int j = 0; j < ys; j++)
            for (int i = 0; i < xs; i++) 
            {
                fread(pixel, sizeof (pixel), 1, fd);
                
                image[j][i][RED]   = (float) ((int) pixel[RED])   * (1.0 / 255.0);
                image[j][i][GREEN] = (float) ((int) pixel[GREEN]) * (1.0 / 255.0);
                image[j][i][BLUE]  = (float) ((int) pixel[BLUE])  * (1.0 / 255.0);
            }

        
        reset = 0; /* init is done */
        fclose(fd);
    }

    // If I don't bring u, v values at 0 or 1 in the following, black line
    // artifacts appear in the image.
    
    if (u < 0) u = 0;
    if (v < 0) v = 0;
    if (u > 1) u = 1;
    if (v > 1) v = 1;

    float u_ = (u * (xs-1));
    float v_ = (v * (ys-1));

    unsigned int left   = floorf(u_);
    unsigned int right  = ceilf(u_);
    unsigned int bottom = floorf(v_);
    unsigned int top    = ceilf(v_);

    float s = u_ - left;
    float t = top - v_;


    GzColor *A = (float (*)[3])(&(image[top][left]));
    GzColor *B = (float (*)[3])(&(image[top][right]));
    GzColor *C = (float (*)[3])(&(image[bottom][right]));
    GzColor *D = (float (*)[3])(&(image[bottom][left]));

    color[RED] = color[GREEN] = color[BLUE] = 0.0f;
    scalarMultiplyAndAdd(*C, s*t, color);
    scalarMultiplyAndAdd(*D, (1 - s) * t, color);
    scalarMultiplyAndAdd(*B, s * (1 - t), color);
    scalarMultiplyAndAdd(*A, (1 - s)*(1 - t), color);

    return GZ_SUCCESS;
}



/* Procedural texture function */
float r = (double)rand() / (double)RAND_MAX;
int change = 0;
int ptex_fun(float u, float v, GzColor color)
{
    ///////////////////////////////////////////////////////
    // JAMES

    color[0] = sin(u * RED_SCALE);
    color[1] = cos(v * GREEN_SCALE);
    color[2] = sin((u+v) * BLUE_SCALE)*cos((u-v) * BLUE_SCALE);
    vmultc(color, color, 0.5);
    vaddc(color, color, 0.5);

    return GZ_SUCCESS;

    ///////////////////////////////////////////////////////////
    // ABDUL

    // // My texture will turn tea-pot into a Mushroom :-)
    
    // color[RED]   = 0.01 + u*(r + sinf(u+v));
    // color[GREEN] = 0.03 + v*(r + sinf(u+v));
    // color[BLUE]  = 0.05 + (u*v)*(r + sinf(u+v));
    
    // change++;
    // if ( change == 5)
    // {
    //     change = 0;
    //     r = (double)rand() / (double)RAND_MAX;
                
    // }
    
    // return GZ_SUCCESS;
}


