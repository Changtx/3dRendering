/* 
 * File:   Transformations.cpp
 * Author: aqadeer
 * 
 * Created on February 22, 2015, 2:52 AM
 */

#include "Transformations.h"
//#define debug

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
    
    float cos_t = cosf(degree*M_PI/180.0f);
    float sin_t = sinf(degree*M_PI/180.0f);
    
    GzMatrix tmp_Rx = 
    { 
            1.0f   ,	0.0f  ,	0.0f      ,	0.0f   , 
            0.0f   ,	cos_t,	(-1.0f)*sin_t ,	0.0f   , 
            0.0f   ,	sin_t,	cos_t    ,	0.0f   , 
            0.0f   ,	0.0f  ,	0.0f      ,	1.0f 
    };
    
    memcpy(mat, tmp_Rx, sizeof(float)*16);

    return GZ_SUCCESS;
}





int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
    
    float cos_t = cosf(degree*M_PI/180.0f);
    float sin_t = sinf(degree*M_PI/180.0f);
    
    GzMatrix tmp_Ry = 
    { 
            cos_t         ,	0.0f  ,	sin_t  ,	0.0f   , 
            0.0f         ,	1.0f  ,	0.0f   ,	0.0f   , 
            (-1.0f)*sin_t ,	0.0f  ,	cos_t  ,	0.0f   , 
            0.0f          ,	0.0f  ,	0.0f   ,	1.0f 
    };
    
    memcpy(mat, tmp_Ry, sizeof(float)*16);

    return GZ_SUCCESS;
}





int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
    
    float cos_t = cosf(degree*M_PI/180.0f);
    float sin_t = sinf(degree*M_PI/180.0f);
    
    GzMatrix tmp_Rz = 
    { 
            cos_t    ,	(-1.0f)*sin_t  ,	0.0f   ,	0.0f   , 
            sin_t    ,	cos_t          ,	0.0f   ,	0.0f   , 
            0.0f     ,	0.0f           ,	1.0f   ,	0.0f   , 
            0.0f     ,	0.0f           ,	0.0f   ,	1.0f 
    };
    
    memcpy(mat, tmp_Rz, sizeof(float)*16);

    return GZ_SUCCESS;
}





int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
    
    GzMatrix tmp_trans = 
    { 
            1.0f    ,	0.0f  ,	0.0f   ,	translate[X]   , 
            0.0f    ,	1.0f  ,	0.0f   ,	translate[Y]   , 
            0.0f    ,	0.0f  ,	1.0f   ,	translate[Z]   , 
            0.0f    ,	0.0f  ,	0.0f   ,	1.0f 
    };
    
    memcpy(mat, tmp_trans, sizeof(float)*16);

    return GZ_SUCCESS;
}




int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
    
    GzMatrix tmp_scale = 
    { 
            scale[X] ,	0.0f       ,	0.0f      ,	0.0f   , 
            0.0f     ,	scale[Y]   ,	0.0f      ,	0.0f   , 
            0.0f     ,	0.0f       ,	scale[Z]  ,	0.0f   , 
            0.0f     ,	0.0f       ,	0.0f      ,	1.0f 
    };
    
    memcpy(mat, tmp_scale, sizeof(float)*16);

    return GZ_SUCCESS;
}



/*             Space Transformations                        */

//*****************************************************************************
int prepareXcw(GzCamera *camera)
{
    //Step 1: Find Z-axis of camera
    float *c = camera->position;
    float *i = camera->lookat;
    GzCoord vector_ci;
    makeVector(c, i, vector_ci);
    GzCoord z;    
    normalizeVector(vector_ci, z);
    
    
    //Step 2: Find Y-axis of camera    
    float *up = camera->worldup; // Changed from {0,1,0} due to question answer by Brandon
    float up_dot_z = vectorDotProduct(up, z);
    GzCoord scaled_z;
    scalarMultiply(z, up_dot_z, scaled_z);
    GzCoord vector_up_dash;
    subtractVector(up, scaled_z, vector_up_dash);
    GzCoord y;
    normalizeVector(vector_up_dash, y);
    
    
    //Step 3: Find X-axis of camera    
    // x = y cross z
    GzCoord x;
    vectorCorssProduct(y, z, x);
    

    //ToDo: I am confused should it be vectorDotProduct(x,c) or (x, vector_ci)?
   
    GzMatrix tmp_Xiw = 
    { 
            x[X],	x[Y],	x[Z],	(-1.0f)*vectorDotProduct(x, c), 
            y[X],	y[Y],	y[Z],	(-1.0f)*vectorDotProduct(y, c), 
            z[X],	z[Y],	z[Z],	(-1.0f)*vectorDotProduct(z, c), 
            0.0f ,	0.0f ,	0.0f ,	 1.0f 
    };
    
    memcpy(camera->Xiw, tmp_Xiw, sizeof(float)*16);
    
    return GZ_SUCCESS;
    
}

//*****************************************************************************
//I initially fixed my wrong pot orientation problem by changing Xpc[0][0] and
//Xpc[1][1] to -1 mainly due to green slides which use right hand coordinate
//system.  Later, on the mention of Brandon, I fixed the bug which was due to
// scaled_z - up when instead I should have done up - scaled_z in Xcw.
int prepareXpc(GzCamera *camera)
{
    
    float one_over_d = tanf((camera->FOV/2.0)*M_PI/180.0f);
    
    GzMatrix tmp_Xpc = 
    { 
            1.0,	0.0,	0.0        ,	0.0, 
            0.0,	1.0,	0.0        ,	0.0, 
            0.0,	0.0,	1.0        ,	0.0, 
            0.0 ,	0.0 ,	one_over_d ,	1.0 
    };
    
    memcpy(camera->Xpi, tmp_Xpc, sizeof(float)*16);   
    
#ifdef debug
    printf("In function %s\n", __FUNCTION__);    
    printf("one_over_d = %f\n", one_over_d); 
    printf("...........\n");    
#endif
    
    return GZ_SUCCESS;
    
}

//*****************************************************************************
int prepareXsp(GzMatrix result, unsigned int Xs, unsigned int Ys, float FOV)
{
    
    
    //Arguments to tan are in radians
    float one_over_d = tanf((FOV/2.0)*M_PI/180);


    GzMatrix tmp_Xsp = 
    { 
            Xs/2.0f,	0.0f      ,	0.0f             ,	Xs/2.0f, 
            0.0f   ,	Ys/(-2.0f),	0.0f             ,	Ys/2.0f, 
            0.0f   ,	0.0f      ,	Z_MAX*one_over_d ,	0.0f   , 
            0.0f   ,	0.0f      ,	0.0f             ,	1.0f 
    };
    
    memcpy(result, tmp_Xsp, sizeof(float)*16);
    

#ifdef debug    
    printf("In function %s\n", __FUNCTION__);    
    printf("Xs = %d, Ys = %d, FOV = %f, 1/d = %f and d = %f\n", Xs, Ys, FOV, one_over_d, (1.0/one_over_d));   
    printf("...........\n");    
#endif
    
    return GZ_SUCCESS;  
    
}
//*****************************************************************************
void printMatrix(GzMatrix mat, unsigned int rows, unsigned int cols)
{
#ifdef debug

    printf("\n");
    for(unsigned int i = 0; i < rows; i++)
    {
        printf("[");
        for(unsigned int j = 0; j < cols; j++)
        {
            printf("%f\t", mat[i][j]);
        }
        printf("]\n");
    }
    printf("\n");
    
#endif
}
//*****************************************************************************
void normalizeVector(GzCoord vector, GzCoord result)
{
    
    float modulus = sqrtf(powf(vector[X],2)+
                          powf(vector[Y],2)+
                          powf(vector[Z],2));
    modulus = (modulus == 0.0f)?1:modulus;
    
    result[X] = vector[X]/modulus;
    result[Y] = vector[Y]/modulus;
    result[Z] = vector[Z]/modulus;
    
}
//*****************************************************************************
float vectorDotProduct(GzCoord x, GzCoord y)
{    
    return (x[X]*y[X] + x[Y]*y[Y] + x[Z]*y[Z]);    
}
//*****************************************************************************
void scalarMultiply(GzCoord x, float scalar, GzCoord result)
{
    result[X] = x[X] * scalar;
    result[Y] = x[Y] * scalar;
    result[Z] = x[Z] * scalar;
}
//*****************************************************************************
void scalarMultiplyAndAdd(GzColor x, float scalar, GzColor result)
{
    result[RED]   = (x[RED]   * scalar) + result[RED];
    result[GREEN] = (x[GREEN] * scalar) + result[GREEN];
    result[BLUE]  = (x[BLUE]  * scalar) + result[BLUE];
}
//*****************************************************************************
void makeVector(GzCoord point1, GzCoord point2, GzCoord vector)
{
    vector[X] = point2[X] - point1[X];
    vector[Y] = point2[Y] - point1[Y];
    vector[Z] = point2[Z] - point1[Z];    
}
//*****************************************************************************
void vectorCorssProduct(GzCoord x, GzCoord y, GzCoord result)
{
        // x = y cross z
    result[X] = x[Y]*y[Z] - x[Z]*y[Y];
    result[Y] = x[Z]*y[X] - x[X]*y[Z];
    result[Z] = x[X]*y[Y] - x[Y]*y[X];    
}
//*****************************************************************************
void subtractVector(GzCoord from, GzCoord to, GzCoord result)
{
    result[X] = from[X] - to[X];
    result[Y] = from[Y] - to[Y];
    result[Z] = from[Z] - to[Z];
}
//*****************************************************************************
void addVectors(GzCoord x, GzCoord y, GzCoord result)
{
    result[X] = x[X] + y[X];
    result[Y] = x[Y] + y[Y];
    result[Z] = x[Z] + y[Z];
}
//*****************************************************************************
void vectorMulElementByElement(GzCoord x, GzCoord y, GzColor r)
{
    r[X] = x[X] * y[X];
    r[Y] = x[Y] * y[Y];
    r[Z] = x[Z] * y[Z];

    
    
}
//*****************************************************************************
void printVector(GzCoord x, char *name)
{
    printf("%s < %f >  < %f >  < %f >\n", name, x[X], x[Y], x[Z]);
}
//*****************************************************************************