/* CS580 Homework 3 */

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "Transformations.h"
#include "Matrix.h"
#include "Stack.h"
#include "rend.h"
#include "disp.h"
#include "gz.h"
#include "vectormath.h"
#include "kubelkamunkparameters.h"
#include "km.h"


//#define debug


// I moved all transformations into Transformation.h and Transformation.cpp to 
// reduce clutter here in rend.cpp
//----------------------------------------------------------
// Begin main functions
int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display) { 
    if(render == NULL || renderClass != GZ_Z_BUFFER_RENDER || display == NULL)
    {
        return GZ_FAILURE;
    }
    
    *render = new GzRender();
    
    if(NULL == *render)
    {
        return GZ_FAILURE;
    }
    
    (*render)->open = 0;//i.e. render is closed.
    (*render)->renderClass = renderClass;
    (*render)->display = display;
    
    
    //Prepare Xsp using default camera parameters       
    prepareXsp((*render)->Xsp,(*render)->display->xres, (*render)->display->xres, (*render)->camera.FOV);
    GzPushMatrix(*render, (*render)->Xsp, false);    
    
    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
int GzFreeRender(GzRender *render) {

    if(NULL == render)
    {
        return GZ_FAILURE;
    }
    
    delete render;
    render = NULL;
    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
int GzBeginRender(GzRender *render) {
     
    if(NULL == render)
    {
        return GZ_FAILURE;
    }
    
    render->display->open = 1;
    
    //GzInitDisplay verifies that display is not NULL before using it.    
    GzInitDisplay(render->display);
    
    //Prepare Xpc    
    prepareXpc(&(render->camera));
    GzPushMatrix(render, render->camera.Xpi, false);
    
    //Prepare Xcw    
    prepareXcw(&(render->camera));    
    GzPushMatrix(render, render->camera.Xiw, true);    

    
    return GZ_SUCCESS;	
}

//------------------------------------------------------------------------------
int GzPutCamera(GzRender *render, GzCamera *camera) {
   
    //Do sanity checking
    if(render == NULL || camera == NULL)
    {
        return GZ_FAILURE;        
    }
    
    memcpy(&(render->camera), camera, sizeof(GzCamera));
    
    //I must empty the stack if someone has called this method.
    //Xsp need to be at the bottom of the stack
    render->xStack.emptyStack();
   
    //Prepare new Xsp using current user provided camera parameters    
    prepareXsp(render->Xsp, render->display->xres, render->display->xres, render->camera.FOV);
    
    GzPushMatrix(render,render->Xsp, false);

    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
int GzPushMatrix(GzRender *render, GzMatrix matrix, bool bothStacks) {
    Matrix *Xcw_stack = new Matrix(matrix);
    render->xStack.push(Xcw_stack);
    
    if(bothStacks)
    {
        Matrix *Xcw_norm_stack = new Matrix(matrix);
        Xcw_norm_stack->makeShadingCaclFriendly();
        render->nStack.push(Xcw_norm_stack);
    }
    
    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
int GzPopMatrix(GzRender *render) {
    if(render->xStack.isEmpty())
        return GZ_FAILURE;
    
    
    Matrix *throwAway;
    render->xStack.pop(&throwAway);
    delete throwAway;
                
                
    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
int GzPutAttribute(GzRender *render, int numAttributes, GzToken *nameList, GzPointer *valueList) {
    // A little sanity checking won"t hurt.    
    if(NULL == render || NULL == nameList || NULL == valueList || numAttributes <= 0)
    {
        return GZ_FAILURE;
    }
    
    int errCount = 0;
    
    for(int i = 0; i < numAttributes; i++)
    {
        void *v = valueList[i];
        switch(nameList[i])
        {
            case GZ_RGB_COLOR:
            {                
                GzColor *c = static_cast<GzColor*> (v);
                int size = sizeof (GzColor) / sizeof ((*c)[0]);
                memcpy(render->flatcolor, *c, size);                
                break;
            }
                
            case GZ_DIRECTIONAL_LIGHT:
            {
                render->lights[render->numlights] = v;//shallow copy
                (render->numlights)++;
                break;
            }
                
            case GZ_AMBIENT_LIGHT:
            {

                render->ambientlight = v; //shallow copy
                break;
            }
                
            case GZ_DIFFUSE_COEFFICIENT:
            {
                
                float *c = static_cast<float *> (v);
                int size = sizeof (GzColor);
                memcpy(render->Kd, c, size);
                break;
            }
                
            case GZ_INTERPOLATE:
            {
                int *i = static_cast<int*> (v);
                render->interp_mode = *i; 
                break;
            }
                
            case GZ_AMBIENT_COEFFICIENT:
            {
                float *c = static_cast<float *>(v);
                int size = sizeof (GzColor);
                memcpy(render->Ka, c, size);
                break;
            }               
                
            case GZ_SPECULAR_COEFFICIENT:
            {
                float *c = static_cast<float *> (v);
                int size = sizeof (GzColor) ;
                memcpy(render->Ks, c, size);
                break;
            }
                               
            case GZ_DISTRIBUTION_COEFFICIENT:
            {
                float *p = static_cast<float*> (v);
                render->spec = *p;
                break;
            }
            
            case GZ_TEXTURE_MAP:
            {
                // v is pointer to function (int (*)(float, float, GzColor))
                render->tex_fun = v;
                break;
            }
            
            case GZ_AASHIFTX:
            {               
                render->aa_delta_x = *((float*)v);
                break;
            }
            
            case GZ_AASHIFTY:
            {
                render->aa_delta_y = *((float*)v);  
                break;
            }
                
            default:
                errCount++;
                break;
        }
    }
    
    
    return (errCount == 0)?GZ_SUCCESS:GZ_FAILURE;		
}

//------------------------------------------------------------------------------
void f_GZ_RGB_COLOR(GzColor c, GzRender *render) {
    int counter = sizeof (GzColor) / sizeof (c[0]);
    for (int i = 0; i < counter; i++) 
    {
        render->flatcolor[i] = c[i];
    }    
}

//------------------------------------------------------------------------------
float f(GzCoord a, GzCoord b, GzCoord c) {
    float x = ((a[1] - b[1])*c[0]) + ((b[0] - a[0])*c[1]) + a[0]*b[1] - b[0]*a[1];
    return x;
}

//------------------------------------------------------------------------------
bool shouldCalcLight(GzCoord N, GzCoord L, GzCoord E, GzCoord Nnew) {
    float NdotL = vectorDotProduct(N, L);
    float NdotE = vectorDotProduct(N, E);
    bool shouldCalc = (NdotL * NdotE) > 0? true: false;
    
    if(NdotL < 0 && NdotE < 0)
    {
        scalarMultiply(N, -1.0f, Nnew);
    }
    
    return shouldCalc;
}

//------------------------------------------------------------------------------
void calc_ambient(GzRender *render, GzColor col, bool mulByK) {
    float(*la)[3] = (float(*)[3])(render->ambientlight);
    GzCoord la_intensity = {la[1][0], la[1][1], la[1][2]};
    
    if(mulByK)
        vectorMulElementByElement(render->Ka, la_intensity, col);
    else
    {
        col[RED]   = la_intensity[RED];
        col[GREEN] = la_intensity[GREEN];
        col[BLUE]  = la_intensity[BLUE];
        
    }
}

//------------------------------------------------------------------------------
void calc_diffuse(GzRender *render, GzCoord N_orig, GzColor col, bool mulByK) {
    // N is already Ncm transformed.    
    GzCoord N = {0.0f,0.0f,0.0f};
    normalizeVector(N_orig, N);
    
    GzCoord AccumulatedDiffuseResult = {0.0f, 0.0f, 0.0f};
    
    for(int i = 0; i < render->numlights; i++)
    {
        float (*ld)[3] = static_cast<float (*)[3]>(render->lights[i]);
        GzCoord ld_L_orig = {ld[0][0], ld[0][1], ld[0][2]};       
        GzCoord ld_L = {0.0f,0.0f,0.0f};         
        normalizeVector(ld_L_orig, ld_L);

        GzCoord E = {0,0,-1};
        
        if (!shouldCalcLight(N, ld_L, E, N))
            continue;
        
        
        float N_dot_L = vectorDotProduct(N, ld_L);                
        GzCoord ld_intensity = {ld[1][0], ld[1][1], ld[1][2]}; 
        
        GzCoord localResult = {0.0f,0.0f,0.0f};
        scalarMultiply(ld_intensity, N_dot_L, localResult);
        addVectors(localResult, AccumulatedDiffuseResult, AccumulatedDiffuseResult);
    }
    
    
    if(mulByK)
        vectorMulElementByElement(render->Kd, AccumulatedDiffuseResult, col);
    else
    {
        col[RED]   = AccumulatedDiffuseResult[RED];
        col[GREEN] = AccumulatedDiffuseResult[GREEN];
        col[BLUE]  = AccumulatedDiffuseResult[BLUE];
        
    }
}

//------------------------------------------------------------------------------
void calc_specular(GzRender *render, GzCoord N_orig, GzColor col, bool mulByK) {
    // N is already sent here after transformation
    GzCoord N = {0.0f,0.0f,0.0f};
    normalizeVector(N_orig, N);
    
    GzCoord AccumulatedSpecResult = {0.0f, 0.0f, 0.0f};
 
    for(int i = 0; i < render->numlights; i++)
    {
        float (*ls)[3] = static_cast<float (*)[3]>(render->lights[i]);
        GzCoord ls_L_orig = {ls[0][0], ls[0][1], ls[0][2]};       
        GzCoord E = {0, 0, -1};        
        
        GzCoord ls_L = {0.0f,0.0f,0.0f};
        normalizeVector(ls_L_orig, ls_L);   
        


        if (!shouldCalcLight(N, ls_L, E, N))
            continue;
        
        
        
        float N_dot_L = vectorDotProduct(N, ls_L);
        GzCoord left = {0.0f,0.0f,0.0f};
        scalarMultiply(N, N_dot_L * 2.0f, left);
        GzCoord R_orig = {0.0f,0.0f,0.0f};
        subtractVector(left, ls_L, R_orig);
        GzCoord R = {0.0f,0.0f,0.0f};
        normalizeVector(R_orig, R);
        
        GzCoord ls_intensity = {ls[1][0], ls[1][1], ls[1][2]};
        
        
        GzCoord localResult = {0.0f,0.0f,0.0f};
        
        float RdotE = vectorDotProduct(R,E);
        if (RdotE < 0) RdotE = 0;
        if (RdotE > 1) RdotE = 1;
        
        scalarMultiply(ls_intensity, 
                       powf(RdotE, render->spec), 
                       localResult);
        
        addVectors(localResult, AccumulatedSpecResult, AccumulatedSpecResult);        
    }
    
     
    if(mulByK)
        vectorMulElementByElement(render->Ks, AccumulatedSpecResult, col);
    else
    {
        col[RED]   = AccumulatedSpecResult[RED];
        col[GREEN] = AccumulatedSpecResult[GREEN];
        col[BLUE]  = AccumulatedSpecResult[BLUE];
        
    }
}

//------------------------------------------------------------------------------
void addThreeColors(GzColor a, GzColor d, GzColor s, GzColor c) {
    c[X] = a[X] + d[X] +s[X];
    c[Y] = a[Y] + d[Y] +s[Y];
    c[Z] = a[Z] + d[Z] +s[Z];
}

//------------------------------------------------------------------------------
void calc_color(GzRender *render, GzCoord N_orig, GzColor col, bool mulByK)
{
    
    GzCoord N = ZEROS;
    normalizeVector(N_orig, N);
    
    
    Matrix *Ncm = render->nStack.leftMulMatricesOnStack();
    if (Ncm == NULL)
    {
        fprintf(stderr, "Got NULL from normal stack in %s.\n",__FUNCTION__);
    }

    
    float array[4] = {N[X], N[Y], N[Z], 1};
    float Ntransformed[4] = {0, 0, 0, 0};
    Ncm->rightMultiply(array, 4, Ntransformed);
    
    GzColor ambient = ZEROS;
    GzColor diffuse = ZEROS;
    GzColor specular = ZEROS;
    
    calc_ambient(render, ambient, mulByK);
    calc_diffuse(render, Ntransformed, diffuse, mulByK);
    calc_specular(render, Ntransformed, specular, mulByK);

    addThreeColors(ambient, diffuse, specular, col);

    render->km_ka = KM_KA;
    render->km_kd = KM_KD;
    render->km_ks = KM_KS;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // THIS PART IS NEW FOR THE PROJECT
    // (the normal homework code is commented out above, all functions it calls are unmodified)

#if 1
    // For each light, determine what layers of paint to add to the model
    scalarMultiply(col, 0.0f, col);
    GzCoord ones = {1.0f, 1.0f, 1.0f};
    vcopy(col, ones);
    GzCoord background_reflectance = BACKGROUND_REFLECTANCE;
    GzCoord background_transmittance = BACKGROUND_TRANSMITTANCE;
    for (int i = 0; i < render->numlights; i++)
    {
        // This light"s direction
        float (*ls)[3] = static_cast<float (*)[3]>(render->lights[i]);
        GzCoord ls_L_orig = {ls[0][0], ls[0][1], ls[0][2]};

        // Ambient light color
        float (*lamb)[3] = static_cast<float (*)[3]>(render->ambientlight);
        GzColor lambcolor = {lamb[1][0], lamb[1][1], lamb[1][2]};

        // This light"s color
        GzColor ls_intensity = {ls[1][0], ls[1][1], ls[1][2]};
        // printVector(ls_intensity, "ls_intensity");
        // printVector(render->Ka, "Ka");
        // printVector(render->Kd, "Kd");
        // printVector(render->Ks, "Ks");

        GzCoord diffuse_thicknesses = {0.0f,0.0f,0.0f};
        calc_diffuse_thickness(render, Ntransformed, ls_L_orig, diffuse_thicknesses);
        // printVector(diffuse_thicknesses, "diffuse_thicknesses");
        GzCoord unlit_thicknesses = {0.0f,0.0f,0.0f};
        calc_unlit_thickness(render, Ntransformed, ls_L_orig, unlit_thicknesses);
        // printVector(unlit_thicknesses, "unlit_thicknesses");

        // printf("\n\n");
        // Use KM Model to add each of these layers of paint to the color of this vertex
        GzCoord diffuse_reflectance = ZEROS;
        GzCoord diffuse_transmittance = ZEROS;
        GzCoord unlit_reflectance = ZEROS;
        GzCoord unlit_transmittance = ZEROS;
        
        // printf("diffuse layer\n");
        GzCoord diffuse_color = DIFFUSE_COLOR;
        kubelka_munk(diffuse_color, diffuse_thicknesses, diffuse_reflectance, diffuse_transmittance);
        // printf("unlit layer\n");
        GzCoord unlit_color = AMBIENT_COLOR;
        kubelka_munk(unlit_color, unlit_thicknesses, unlit_reflectance, unlit_transmittance);
        // printf("\n\n");

        // vmult(diffuse_reflectance, ls_intensity, diffuse_thicknesses);
        // vmult(unlit_reflectance, ls_intensity, unlit_thicknesses);

        GzCoord composite_reflectance = ZEROS;
        GzCoord composite_transmittance = ZEROS;
        km_composite_layers(diffuse_reflectance, diffuse_transmittance, diffuse_reflectance, 
                                diffuse_transmittance, background_reflectance, background_transmittance);
        km_composite_layers(background_reflectance, background_transmittance, unlit_reflectance, 
                                unlit_transmittance, diffuse_reflectance, diffuse_transmittance);
        // printVector(diffuse_reflectance, "diffuse_reflectance");
        // vmult(col, col, lambcolor);
        
        // break;  // ASSUME ONLY ONE COLOR!!!
    }

    vcopy(col, background_reflectance);
#endif

}

//------------------------------------------------------------------------------
int GzPutTriangle(GzRender *render, int numParts, GzToken *nameList, GzPointer *valueList) {
    /* numParts : how many names and values */

    // Do some sanity checking    
    if(NULL == render || NULL == nameList || NULL == valueList || numParts != 3)
    {
        return GZ_FAILURE;
    } 
    
        
    Matrix *Xsm = render->xStack.leftMulMatricesOnStack();
    if (Xsm == NULL)
    {
        fprintf(stderr, "Got NULL from stack in GzPutTriangle.\n");
    } 
    
    
    
    
    GzCoord *c_old;
    GzCoord *n;
    GzColor triEdgeColors[3];
    GzTextureIndex *triTextures;
    
    for (int i = 0; i < numParts; i++)
    {
        int name = nameList[i];
        switch(name)
        {
            case GZ_POSITION:
            {
                // Get ready to read coordinates
                void *l1 = valueList;
                GzCoord **l2 = static_cast<GzCoord **> (l1);
                c_old = static_cast<GzCoord *> (l2[i]);
                
                break;
            }
            
            case GZ_NORMAL:
            {
                void *l1 = valueList;
                GzCoord **l2 = static_cast<GzCoord **> (l1);
                
                n = static_cast<GzCoord *> (l2[i]);
                
                
                if (render->interp_mode == GZ_COLOR) 
                {
                    for (int k = 0; k < 3; k++)
                        calc_color(render, n[k], triEdgeColors[k], (render->tex_fun == false));

                }

                break;
            }
            
            case GZ_TEXTURE_INDEX:
            {
                void *l1 = valueList;
                GzTextureIndex **l2 = static_cast<GzTextureIndex**> (l1);
                triTextures = l2[i];             
                
                break;
            }
            
            default:
                return GZ_FAILURE;            
        }// end of switch statement
    }//end of for loop
    
    
    
   

    GzCoord c[3];
    // Transform triangle coordinates into screen coordinates
    for(int i = 0; i < 3 ; i++)
    {
        float array[4] = {c_old[i][0], c_old[i][1], c_old[i][2], 1};
        float rMulResult[4] ={0, 0, 0, 0};
        Xsm->rightMultiply(array, 4, rMulResult);
        c[i][X] = rMulResult[0] + render->aa_delta_x;
        c[i][Y] = rMulResult[1] + render->aa_delta_y;
        c[i][Z] = rMulResult[2];           
    }

    // Lets see if all 3 triangle coordinates are viewable or not.
    if(isBehindViewPlane(render, c) || isClipped(render, c))
        return GZ_FAILURE;   

    // Find bounding box
    int x_min = floor(findMin(c, 0, 3));
    int x_max = ceil (findMax(c, 0, 3));
    int y_min = floor(findMin(c, 1, 3));
    int y_max = ceil (findMax(c, 1, 3));


    // Iterate over every pixel in the bounding box
    for(int j = y_min; j <= y_max; j++)        
        for(int i = x_min; i <= x_max; i++)
        {
            // Calculate Barycentric coordinates
            GzCoord p = {float(i), float(j), 0};
            float alpha = f(c[1], c[2], p) / f(c[1], c[2], c[0]);
            float beta  = f(c[2], c[0], p) / f(c[2], c[0], c[1]);
            float gamma = f(c[0], c[1], p) / f(c[0], c[1], c[2]);

            // Litmus Test: Is pixel (i,j) inside triangle?
            if ( alpha > 0 && beta > 0 && gamma > 0)                
            {
                //interpolate z value
                float newZ = alpha*(c[0][2]) + beta*(c[1][2]) + gamma*(c[2][2]);


                // Get current Z value for this specific pixel (i,j)
                GzIntensity dummy;
                GzDepth oldZ;
                GzGetDisplay(render->display, i, j, &dummy, &dummy, &dummy, &dummy, &oldZ);


                // Check will this pixel be displayed or not
                if(newZ < oldZ)
                {

                    GzColor interpColor = {0, 0, 0};                    
                    GzColor color = {1.0f, 1.0f, 1.0f}; // color coming from texture
                    if(render->tex_fun != false)
                        calcTexture(render, triTextures, c, newZ, alpha, beta, gamma, color);


                    
                    
                    
                    if(render->interp_mode == GZ_NORMALS) // Phong Shading
                    {
                        GzCoord interpN = {0,0,0};
                        for (int k = 0; k < 3; k++)
                            interpN[k] = alpha*n[0][k] + beta*n[1][k] + gamma*n[2][k];
                        
                        if(render->tex_fun != false)
                        {
                                memcpy(render->Ka, color, sizeof (GzColor));
                                memcpy(render->Kd, color, sizeof (GzColor));
                        }
                
                        

                          
                        calc_color(render, interpN, interpColor, true);
                        
                        GzPutDisplay(render->display, i, j, ctoi(interpColor[X]), ctoi(interpColor[Y]), ctoi(interpColor[Z]), 1, newZ);
                        
                    }
                    else if(render->interp_mode == GZ_COLOR) // Gouraud Shading
                    {
                        float r = alpha * triEdgeColors[0][X] + beta * triEdgeColors[1][X] + gamma * triEdgeColors[2][X];
                        float g = alpha * triEdgeColors[0][Y] + beta * triEdgeColors[1][Y] + gamma * triEdgeColors[2][Y];
                        float b = alpha * triEdgeColors[0][Z] + beta * triEdgeColors[1][Z] + gamma * triEdgeColors[2][Z];
                        
                        // Multiply by Kt which is in color due to earlier texture calculation
                        r *= color[RED];
                        g *= color[GREEN];
                        b *= color[BLUE];



                        GzPutDisplay(render->display, i, j, ctoi(r), ctoi(g), ctoi(b), 1, newZ);
                        
                    }
                    
                    
                    // FLAT Shading 
                                            
                        //GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), 
                        //ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 1, newZ);                   

                }


            }// end of litmus test if
        }   
    
    return GZ_SUCCESS;
}

//------------------------------------------------------------------------------
void calcTexture(GzRender *render, GzTextureIndex *triTextures, GzCoord *c, 
                 float newZ, float alpha, float beta, float gamma, GzColor color) {

    // Step 1: Warp u, v
    float (*t)[2] = (float(*)[2])triTextures;
    float warpedUV[6];
    int w_n = 0;

    for (int l = 0; l < 3; l++)
        for (int m = 0; m < 2; m++) {
            float warpAmount = (c[l][Z] / (INT_MAX - c[l][Z])) + 1;
            warpedUV[w_n] = t[l][m] / warpAmount;
            w_n++;
        }

    // Step 2: Interpolated warpped u,v

    float interpU = alpha * warpedUV[0] + beta * warpedUV[2] + gamma * warpedUV[4];
    float interpV = alpha * warpedUV[1] + beta * warpedUV[3] + gamma * warpedUV[5];


    // Step 3: Unwarp u, v
    
    float unWarpAmount = (newZ / (INT_MAX - newZ))+1;
    float unWarpedU, unWarpedV;
    unWarpedU = interpU * unWarpAmount;
    unWarpedV = interpV * unWarpAmount;
    
    
    // Step 4: Do texture function call

    ((int (*)(float, float, GzColor))render->tex_fun)(unWarpedU, unWarpedV, color);
}

//------------------------------------------------------------------------------


/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

//------------------------------------------------------------------------------

float findMin(GzCoord *array, int dim, int size)
{
    float min = array[0][dim];
    for(int i = 1; i < size; i++)
    {
        if(array[i][dim] < min)
            min = array[i][dim];
    }
    
    return min;   
}
//------------------------------------------------------------------------------

float findMax(GzCoord *array, int dim, int size)
{
    float max = array[0][dim];
    for(int i = 1; i < size; i++)
    {
        if(array[i][dim] > max)
            max = array[i][dim];
    }
    
    return max;   
}
//------------------------------------------------------------------------------
// Used for debugging purposes
void printArray(GzCoord *array, int dim, int size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%f ", array[i][dim]);
    }
    
    printf("\n");
    
}

//------------------------------------------------------------------------------
bool isBehindViewPlane(GzRender *render, GzCoord *triVertices) {
    bool result = false;
    
    if (triVertices[0][Z] < render->camera.position[Z] &&
        triVertices[1][Z] < render->camera.position[Z] &&
        triVertices[2][Z] < render->camera.position[Z])
    {
        result = true;
    }
    
    return result;
}
//------------------------------------------------------------------------------
bool isClipped(GzRender *render, GzCoord *triVertices)
{
    int count = 0;
    
    for(int i = 0; i < 3; i++)
    {
        if ((triVertices[i][X] < 0 || triVertices[i][X] > render->display->xres)
                &&
            (triVertices[i][Y] < 0 || triVertices[i][Y] > render->display->yres)
           )
            count = count + 1;
        
    }
    
    return (count == 3)?true:false;    
}
//------------------------------------------------------------------------------
