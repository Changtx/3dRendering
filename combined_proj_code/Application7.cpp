/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/
#define INFILE  "ppot.asc"//"ppot.asc"


#define WIDTH   512
#define HEIGHT  512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include"ControlGlobals.h"

#include "Application7.h"
#include "rend.h"
#include "Helper.h"

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */


int Application7::runRenderer(GzDisplay **display, GzRender **render, float delta_x, float delta_y)
{        
	GzCamera        camera;
	GzToken         nameListTriangle[5]; 	/* vertex attribute names */
	GzPointer       valueListTriangle[5]; 	/* vertex attribute pointers */
	GzToken         nameListShader[9];      /* shader attribute names */
	GzPointer       valueListShader[9];     /* shader attribute pointers */
	GzToken         nameListLights[10];     /* light info */        
	GzPointer       valueListLights[10];
        GzToken         nameListAADeltas[2];    /* Anti aliasing delta x and y*/
        GzPointer       valueListAADeltas[2];
	GzCoord		vertexList[3];		/* vertex position coordinates */ 
	GzCoord		normalList[3];		/* vertex normals */ 
        GzCoord		majorDirectionVecList[3];		/* vertex normals */ 
        GzCoord		minorDirectionVecList[3];		/* vertex normals */ 
	GzTextureIndex	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int		status; 
	int		xRes, yRes, dispClass;	/* display parameters */ 
	int             interpStyle;
        float           specpower;
 
	/* Transforms */
	GzMatrix	translateAndScale = 
	{ 
		{1,	0.0,	0.0,	0.0}, 
		{0.0,	1,	0.0,	-3.25}, 
		{0.0,	0.0,	1,	3.5}, 
		{0.0,	0.0,	0.0,	1.0} 
	}; 
	 
	GzMatrix	rotateX = 
	{ 
		{1.0,	0.0,	0.0,	0.0}, 
		{0.0,	.7071,	.7071,	0.0}, 
		{0.0,	-.7071,	.7071,	0.0}, 
		{0.0,	0.0,	0.0,	1.0}
	}; 
	 
	GzMatrix	rotateY = 
	{ 
		{.866,	0.0,	-0.5,	0.0}, 
		{0.0,	1.0,	0.0,	0.0}, 
		{0.5,	0.0,	.866,	0.0}, 
		{0.0,	0.0,	0.0,	1.0}
	};

	/* Light */
        float light1[2][3]          = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
        float light2[2][3]          = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
        float light3[2][3]          = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
        float ambientlight[2][3]    = { {0.0f, 0.0f, 0.0f}, {0.30f, 0.30f, 0.30f} };
        
	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient  = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient  = { 0.7, 0.7, 0.7 };

	/* 
	 * initialize the display and the renderer 
	 */ 

	int width = WIDTH;    // frame buffer and display width
	int height = HEIGHT;   // frame buffer and display height

	status = 0;

	status |= GzNewDisplay(display, GZ_RGBAZ_DISPLAY, width, height);

	status |= GzGetDisplayParams(*display, &xRes, &yRes, &dispClass); 
	 
	status |= GzInitDisplay(*display); 
 
	status |= GzNewRender(render, GZ_Z_BUFFER_RENDER, *display); 

#if 0 	/* set up app-defined camera if desired, else use camera defaults */
//    camera.position[X] = -3;
//    camera.position[Y] = -25;
//    camera.position[Z] = -4;
//
//    camera.lookat[X] = 7.8;
//    camera.lookat[Y] = 0.7;
//    camera.lookat[Z] = 6.5;
//
//    camera.worldup[X] = -0.2;
//    camera.worldup[Y] = 1.0;
//    camera.worldup[Z] = 0.0;
//
//    camera.FOV = 63.7;              /* degrees */
    camera.position[X] = 13.2f;
    camera.position[Y] = -8.7f;
    camera.position[Z] = -14.8f;

    camera.lookat[X] = 0.8f;
    camera.lookat[Y] = 0.7f;
    camera.lookat[Z] = 4.5f;

    camera.worldup[X] = -0.2f;
    camera.worldup[Y] = 1.0f;
    camera.worldup[Z] = 0.0f;

    camera.FOV = 53.7f; /* degrees */

    status |= GzPutCamera(*render, &camera); 
#endif 

	status |= GzBeginRender(*render);
        
        nameListAADeltas[0] = GZ_AASHIFTX;
        valueListAADeltas[0] = (GzPointer)&delta_x;
        
        nameListAADeltas[1] = GZ_AASHIFTY;       
        valueListAADeltas[1] = (GzPointer)&delta_y;
        
                
        status |= GzPutAttribute(*render, 2, nameListAADeltas, valueListAADeltas);
        
        

	/* 
	  renderer is ready for frame --- define lights and shader at start of frame 
	*/

	/*
	 * Tokens associated with light parameters
	 */
	nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[0] = (GzPointer)&light1;
	nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[1] = (GzPointer)&light2;
	nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
	valueListLights[2] = (GzPointer)&light3;
	status |= GzPutAttribute(*render, 3, nameListLights, valueListLights);

	nameListLights[0] = GZ_AMBIENT_LIGHT;
	valueListLights[0] = (GzPointer)&ambientlight;
	status |= GzPutAttribute(*render, 1, nameListLights, valueListLights);

	/*
	 * Tokens associated with shading 
	 */
	nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
	valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
	nameListShader[1]  = GZ_INTERPOLATE;
#if 0
	interpStyle = GZ_COLOR;         /* Gouraud shading */
#else 
	interpStyle = GZ_NORMALS;       /* Phong shading */
#endif

	valueListShader[1] = (GzPointer)&interpStyle;
	nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
	valueListShader[2] = (GzPointer)ambientCoefficient;
	nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
	valueListShader[3] = (GzPointer)specularCoefficient;
	nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
	specpower = 32;
	valueListShader[4] = (GzPointer)&specpower;
	nameListShader[5] = GZ_TEXTURE_MAP;
#if 1
        valueListShader[5] = 0; // No textures please
#else
#if 1
	valueListShader[5] = (GzPointer)tex_fun;  /* or use ptex_fun */
#else
	valueListShader[5] = (GzPointer)ptex_fun;
#endif
#endif

	status |= GzPutAttribute(*render, 6, nameListShader, valueListShader);

	status |= GzPushMatrix(*render, translateAndScale, false);  
	status |= GzPushMatrix(*render, rotateY, true); 
	status |= GzPushMatrix(*render, rotateX, true);

	if (status) return(GZ_FAILURE); 

	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION;
	nameListTriangle[1] = GZ_NORMAL;
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
        nameListTriangle[3] = GZ_MAJOR_CUVATURE_DIRECTION;
        nameListTriangle[4] = GZ_MINOR_CUVATURE_DIRECTION;
	 
	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
		printf("Could not open input from %s \n", INFILE);
		return GZ_FAILURE;
	}



	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
         * 
         * Command to transform Tian's asc format:
         * cat  shuttle.txt | awk ' {if (NF < 12) print ; else if (NF == 12) print $1,$2,$3,$4,$5,$6,0.0,0.0,$7,$8,$9,$10,$11,$12};' > ppot.asc
	*/ 
        
	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]), 
		&(majorDirectionVecList[0][0]), &(majorDirectionVecList[0][1]), 	
		&(majorDirectionVecList[0][2]) , 
		&(minorDirectionVecList[0][0]), &(minorDirectionVecList[0][1]), 	
		&(minorDirectionVecList[0][2])); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]), 
		&(majorDirectionVecList[1][0]), &(majorDirectionVecList[1][1]), 	
		&(majorDirectionVecList[1][2]) , 
		&(minorDirectionVecList[0][0]), &(minorDirectionVecList[0][1]), 	
		&(minorDirectionVecList[0][2])); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]), 
		&(majorDirectionVecList[2][0]), &(majorDirectionVecList[2][1]), 	
		&(majorDirectionVecList[2][2]) , 
		&(minorDirectionVecList[0][0]), &(minorDirectionVecList[0][1]), 	
		&(minorDirectionVecList[0][2])); 
 
	    /* 
	    * Set the value pointers to the first vertex of the
	    * triangle, then feed it to the renderer 
		* NOTE: this sequence matches the nameList token sequence
	    */ 
	    valueListTriangle[0] = (GzPointer)vertexList;
	    valueListTriangle[1] = (GzPointer)normalList;
	    valueListTriangle[2] = (GzPointer)uvList;
            valueListTriangle[3] = (GzPointer)majorDirectionVecList;
            valueListTriangle[4] = (GzPointer)minorDirectionVecList;
	    GzPutTriangle(*render, 5, nameListTriangle, valueListTriangle); 
	} 	

	/* 
	 * Clean up and exit
	 */ 

	if( fclose( infile ) )
		printf( "The input file was not closed\n" ); 
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

//..............................................................................

int Application7::emitFile(GzDisplay *display) {
    
    FILE *outfile;
    if ((outfile = fopen(outFileName, "wb")) == NULL) 
    {
        printf("Could not open output file for writing %s \n", outFileName);
        return GZ_FAILURE;
    }


    GzFlushDisplay2File(outfile, display); /* write out or update display to file*/

    if (fclose(outfile))
        printf("The output file was not closed\n");
    
    
    return GZ_SUCCESS;
}
//..............................................................................
// Some more globals.  Will need to access them while putting pixels.
    float **whiteNoise = NULL;    
    dVec **majDirectionVec = NULL;
    dVec **minDirectionVec = NULL;
    bool rendererInDirectionalVecGenratonMode = false;

//..............................................................................

int Application7::runAntiAliasingRenderer() 
{   
    // We need to do Line Integral Convolution (LIC) before actual rendering.
    // LIC is the "thing" which gives that brush stroke like affect
    this->doLIC();
    
#define	AAKERNEL_SIZE	6
#define delta_x 0
#define delta_y 1
#define weight 2

  
   
    float AAFilter[AAKERNEL_SIZE][3] =
    {
        {-0.52, 0.38, 0.128 },
        {0.41, 0.56, 0.119  },
        {0.27, 0.08, 0.294  },
        {-0.17, -0.29, 0.249},
        {0.58, -0.55, 0.104 },
        {-0.31, -0.71, 0.106}
    };
    
    
    // Get 6 different rendered images
    
    GzRender *renders[AAKERNEL_SIZE];
    GzDisplay *display[AAKERNEL_SIZE];
    
    for(int i = 0; i < AAKERNEL_SIZE; i++)
    {

        
        runRenderer(&display[i], &renders[i], AAFilter[i][0], AAFilter[i][1]);

    }
  
    
    // Now lets weighted average all of 6 rendered images
    
    GzDisplay *theDisplay;
    GzRender *dummyRender;
    runRenderer(&theDisplay, &dummyRender, 0.0, 0.0f); // This call just to initialize display properly
    
    GzIntensity dummy;
    GzDepth dummy2;
    
    for(int row = 0; row < theDisplay->yres; row++)
        for(int col = 0; col < theDisplay->xres; col++)
        {
            float result[3] = {0.0f, 0.0f, 0.0f};
            short oColors[3];
            float totalWeight = 0.0f;
            for(int i = 0; i < AAKERNEL_SIZE; i++)
            {
                GzGetDisplay(display[i], row, col, &oColors[RED], &oColors[GREEN], &oColors[BLUE], &dummy, &dummy2);
                result[RED] += oColors[RED] * AAFilter[i][2];
                result[GREEN] += oColors[GREEN] * AAFilter[i][2];
                result[BLUE] += oColors[BLUE] * AAFilter[i][2];
                
                totalWeight += AAFilter[i][2];
            }
            
            if(totalWeight == 0)
                totalWeight = 1;
            
            GzPutDisplay(theDisplay, row, col, result[RED]/totalWeight, result[GREEN]/totalWeight, result[BLUE]/totalWeight, 1, 0);
        }
    
    
    
    this->emitFile(theDisplay);
    
    int status = 0;
    
        for(int i = 0; i < AAKERNEL_SIZE; i++)
    {
        status |= GzFreeRender(renders[i]); 
	status |= GzFreeDisplay(display[i]);
    }

    status |= GzFreeRender(dummyRender);
    status |= GzFreeDisplay(theDisplay);
    
    
    this->cleanupLIC(HEIGHT, WIDTH);
           
    if (status)
        return (GZ_FAILURE);
    else
        return (GZ_SUCCESS);       
}
//..............................................................................
void Application7::initLIC(int rows, int cols) {
    whiteNoise = new float*[rows];
    assert(whiteNoise != NULL);
    
    majDirectionVec = new dVec*[rows];
    assert(majDirectionVec != NULL);
    
    minDirectionVec = new dVec*[rows];
    assert(minDirectionVec != NULL);
    
    for (int i = 0; i < rows; i++) 
    {
        whiteNoise[i] = new float[cols];
        assert(whiteNoise[i] != NULL);

        majDirectionVec[i] = new dVec[cols]();
        assert(majDirectionVec[i] != NULL);
        

        minDirectionVec[i] = new dVec[cols]();
        assert(minDirectionVec[i] != NULL);
        
    }
}
//..............................................................................
void Application7::cleanupLIC(int rows, int cols) {
    
    for (int i = 0; i < rows; i++) 
    {
        if(minDirectionVec[i] != NULL)
        {
            delete [] minDirectionVec[i];
            minDirectionVec[i] = NULL;
        }
                
        if(majDirectionVec[i] != NULL)
        {
            delete [] majDirectionVec[i];
            majDirectionVec[i] = NULL;
        }
                
        if(whiteNoise[i] != NULL)
        {
            delete [] whiteNoise[i];
            whiteNoise[i] = NULL;
        } 
        

        

    }
    
    delete [] minDirectionVec;
    minDirectionVec = NULL;
    
    delete [] majDirectionVec;
    majDirectionVec = NULL;
    
    delete [] whiteNoise;
    whiteNoise = NULL;    

}
//..............................................................................
float **perlinNoise = NULL;

int Application7::generateLICData(const int rows, const int cols)
{
    perlinNoise = new float*[HEIGHT];
    for(int i = 0; i < HEIGHT; i++)
        perlinNoise[i] = new float[WIDTH];
    noise_lk(HEIGHT,WIDTH, perlinNoise);
  
    writePPMFile(WIDTH, HEIGHT, perlinNoise, "perlin_lk.ppm");
    
    makeWhiteNoise(rows, cols, whiteNoise);        
    makeDirectionFieldVectors();     
    return GZ_SUCCESS;    
}
//..............................................................................
void Application7::makeWhiteNoise(int  rows,  int  cols,  float  **noise)
{
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
    	{
            // Generate a random no between 0 and 1
            unsigned short r = rand() % 2;
            noise[i][j] = r;
    	}
}
//..............................................................................
void Application7::makeDirectionFieldVectors()
{    
    // We need to run renderer once to generate directional vectors
    rendererInDirectionalVecGenratonMode = true;
    
    GzRender *render;
    GzDisplay *display;
    runRenderer(&display, &render, 0, 0);
    
    GzFreeRender(render);
    GzFreeDisplay(display);
    
    rendererInDirectionalVecGenratonMode = false;    
      
}
//..............................................................................
/*
 References:
 * (1) http://en.wikipedia.org/wiki/Line_integral_convolution
 * (2) http://www.zhanpingliu.org/research/flowvis/FlowVis.htm#Ref_33
 * (3) http://prosjekt.ffi.no/unik-4660/lectures04/chapters/pdfs/vec_viz.pdf
 * (4) http://w3.impa.br/~rdcastan/Visualization/docs/LIC.pdf
 * (5) http://mesh.brown.edu/taubin/pdfs/taubin-iccv95b.pdf
 * (6) http://www3.nd.edu/~emathew1/CurvatureVizWorkshop2010.pdf
 * (7) http://mesh.brown.edu/taubin/pdfs/taubin-iccv95a.pdf
 * (8) http://www.ann.jussieu.fr/~frey/papers/scientific%20visualisation/Maillot%20J.,%20Interactive%20texture%20mapping.pdf
 * (9) http://www.cs.rutgers.edu/~decarlo/readings/winkenbach-sg96.pdf
 * (10) http://www.cc.gatech.edu/~jarek/papers/Hidden.pdf
 * (11) http://stevezero.com/eecs/cs294proj1/
 * (12) http://www.zju.edu.cn/jzus/2005/A05S1/A05S121.pdf
 * (13) http://brickisland.net/cs177fa12/?p=214
 * (14) http://www.mathworks.com/matlabcentral/fileexchange/24937-curvature-calculations-on-triangular-3d-surface
 * (15) http://master.grad.hr/hdgg/kog_stranica/kog10gif/kog10_nagy.pdf
 * (16) http://www.zhanpingliu.org/research/flowvis/LIC/LIC.htm
 * 
 */
//..............................................................................
void Application7::calcLIC(int xres, int yres, 
            float (**directionVec)[2], 
            float ***pnoise, 
            float kernelLen, 
            float *posDirectionFilter, float *negDirectionFilter, 
            float **result) 
{
 
    float **noise = *pnoise;
    ///kernelLen = 100; //<------------------ Uncomment if more smoothing needed
    
   
    // We need to calcLIC for xres * yres pixels
    for (int j = 0; j < yres; j++)
        for (int i = 0; i < xres; i++)
        {
            float accumulated_texture[2] = {0.0f, 0.0f}; 
            float accumulated_weight[2] = {0.0f, 0.0f};
            
            // For both advection directions, pos and neg
            for(int advDir = 0;  advDir < 2;  advDir ++)
            {
                
                			///init the step counter, curve-length measurer, and streamline seed///
		unsigned int advctStepCounter = 0;
		float streamLineCurLen = 0.0f;
            	float currentClipPoint[2] = {i + 0.5f, j + 0.5f};
                float *wgtFilter = (advDir == 0) ? negDirectionFilter : posDirectionFilter;
                
                unsigned int MAX_ADVCTS = kernelLen * 3;
                while(streamLineCurLen < kernelLen && advctStepCounter < MAX_ADVCTS) 
                {
                    int y = int(currentClipPoint[0]);
                    int x = int(currentClipPoint[1]);
                    float foreFrontPoint[2] = {directionVec[x][y][0],
                                               directionVec[x][y][1]};
                    foreFrontPoint[0] = directionVec[x][y][0];
                    foreFrontPoint[1] = directionVec[x][y][1];
                    
                    // Test: Are we at critical point
                    					
                    if (foreFrontPoint[0] == 0.0f && foreFrontPoint[1] == 0.0f) 
                    {
                        accumulated_texture[advDir] = (advctStepCounter == 0) ? 0.0f : accumulated_texture[advDir];
                        accumulated_weight[advDir] = (advctStepCounter == 0) ? 1.0f : accumulated_weight[advDir];
                        break;
                    }

                    // If advection is towards -ve direction, we need to fix fore front point sign
                    if(advDir == 1)
                    {
                        foreFrontPoint[0] = foreFrontPoint[0] * -1.0f;
                        foreFrontPoint[1] = foreFrontPoint[1] * -1.0f;
                        
                    }
                    
                    #define	 LINE_SQUARE_CLIP_MAX	100000.0f
                    #define	 VECTOR_COMPONENT_MIN   0.050000f
                    
                    float segLen = LINE_SQUARE_CLIP_MAX;
                    segLen = LINE_SQUARE_CLIP_MAX;
                    // One of following 4 should be true
                    // ToFix: What happens when things are equal?
                    segLen = (foreFrontPoint[X] < -VECTOR_COMPONENT_MIN) ? (int(currentClipPoint[X]) - currentClipPoint[X]) / foreFrontPoint[X] : segLen;
                    segLen = (foreFrontPoint[X] >  VECTOR_COMPONENT_MIN) ? (int( int(currentClipPoint[X]) + 1.5f) - currentClipPoint[X]) / foreFrontPoint[X] : segLen;
                    
                    float tmpLen = 0.0f;
                    segLen = (foreFrontPoint[Y] < -VECTOR_COMPONENT_MIN) ? (((tmpLen = ( int(currentClipPoint[Y]) - currentClipPoint[Y]) / foreFrontPoint[Y]  )  <  segLen    ) ? tmpLen : segLen): segLen;
                    segLen = (foreFrontPoint[Y] >  VECTOR_COMPONENT_MIN) ? (((tmpLen = ( int( int(currentClipPoint[Y]) + 1.5f ) - currentClipPoint[Y] ) / foreFrontPoint[Y]  )  <  segLen    ) ? tmpLen : segLen): segLen;
                    
                    
                    
                    float streamLinePrvLen = streamLineCurLen;
                    streamLineCurLen += segLen;
                    segLen += 0.0004f;

                    
                    segLen = (streamLineCurLen > kernelLen) ? ((streamLineCurLen = kernelLen) - streamLinePrvLen) : segLen;

                    // obtain next clip point
                    float nextClipPoint[2] = {currentClipPoint[X] + foreFrontPoint[X]*segLen,
                                              currentClipPoint[Y] + foreFrontPoint[Y]*segLen};
                    
                    float sample[2] = {(currentClipPoint[X] + nextClipPoint[X])*0.5f,
                                       (currentClipPoint[Y] + nextClipPoint[Y])*0.5f};

                    // What is texture value at the sampling point
                    int l = (ceil(sample[X]));
                    int k = (ceil(sample[Y]));
                    if ( l < 0 || k < 0 || l >= xres  || k  >= yres) 
                    {
                        break;
                    }

                    int xx = (int(sample[X]));
                    int yy = (int(sample[Y]));
                    if(xx < 0 || xx >= WIDTH || yy < 0 || yy >= HEIGHT)
                    {
                        printf("(int(sample[X])) = %d, (int(sample[Y])) = %d\n", (int(sample[X])), (int(sample[Y])));
                        printf("currentclippoint[x] = %f, currentclipPoint[y] = %f, forefrontpoint[x] = %f, forefrontpoint[y] = %f and seglen = %f\n",
                                currentClipPoint[X], currentClipPoint[Y],foreFrontPoint[X], foreFrontPoint[Y], segLen );
                        fflush(stdout);
                    }
                    
                    //float texVal = noise[int(sample[Y])][int(sample[X])];
                    float texVal = noise[yy][xx];

                    ///update the accumulated weight and the accumulated composite texture (texture x weight)///
                    #define	 DISCRETE_FILTER_SIZE	WIDTH
                    float len2ID = (DISCRETE_FILTER_SIZE - 1) / kernelLen;
                    float W_ACUM = wgtFilter[int(streamLineCurLen * len2ID)];
                    float smpWgt = W_ACUM - accumulated_weight[advDir];
                    accumulated_weight[advDir] = W_ACUM;
                    accumulated_texture[advDir] += texVal * smpWgt;

                    ///update the step counter and the "current" clip point///
                    advctStepCounter++;
                    currentClipPoint[X] = nextClipPoint[X];
                    currentClipPoint[Y] = nextClipPoint[Y];
                    

                    ///check if the streamline has gone beyond the flow field///
                    if (currentClipPoint[X] < 0.0f || currentClipPoint[X] >= xres 
                     || currentClipPoint[Y] < 0.0f || currentClipPoint[Y] >= yres) 
                        break;               
                } //end of while                
            } //end of advection direction handling	

            ///normalize the accumulated composite texture///
            
            // Will need to do ctoi() later when making ppm
            result[j][i] = (accumulated_texture[X] + accumulated_texture[Y]) / (accumulated_weight[X] + accumulated_weight[Y]);
               
            
        } //end of xres * yres loop
    
    
    
}
//..............................................................................
// Even more globals
float **licMaj = NULL;
float **licMin = NULL;
//..............................................................................
void Application7::doLIC()
{
    this->initLIC(HEIGHT, WIDTH);
    // LIC depends on Perlin noise and direction vectors.  Lets generate them.
    generateLICData(HEIGHT, WIDTH);
    
    
    const int filterSize = 2048;
    float *posDirectionFilter = new float[filterSize];
    assert(posDirectionFilter != NULL);
    float *negDirectionFilter = new float[filterSize];
    assert(negDirectionFilter != NULL);
    
    generateBoxFilter(filterSize, posDirectionFilter, negDirectionFilter);
    //..........................................................................
    #define  LOWPASS_FILTR_LENGTH	10.00000f
    licMin = new float*[HEIGHT];
    assert(licMin != NULL);
    for(int i = 0; i < HEIGHT; i++)
    {
        licMin[i] = new float[WIDTH];
        assert(licMin[i]);
    }
    
    float ***array[2] = {0,0};
    

    switch (noiseType)
    {
        case WHITE_NOISE:
        {
            array[0] = &whiteNoise;
            array[1] = &whiteNoise;
            break;
        }
        
        case PERLIN_NOISE:
        {
            array[0] = &perlinNoise;
            array[1] = &perlinNoise;
            break;               
        }
        
        case WHITE_PERLIN:
        {
            array[0] = &whiteNoise;
            array[1] = &perlinNoise;
            break;
        }
        
        case PERLIN_WHITE:
        {
            array[0] = &perlinNoise;
            array[1] = &whiteNoise;
            break;
        }
    }

    
    calcLIC(WIDTH, HEIGHT, 
            minDirectionVec, 
            array[0], 
            LOWPASS_FILTR_LENGTH, 
            posDirectionFilter, negDirectionFilter, 
            licMin); 
    //..........................................................................
    licMaj = new float*[HEIGHT];
    assert(licMaj != NULL);
    for(int i = 0; i < HEIGHT; i++)
    {
        licMaj[i] = new float[WIDTH];
        assert(licMaj[i]);
    }
    
    calcLIC(WIDTH, HEIGHT, 
            majDirectionVec, 
            array[1], 
            LOWPASS_FILTR_LENGTH, 
            posDirectionFilter, negDirectionFilter, 
            licMaj); 
    
    writePPMFile(WIDTH, HEIGHT, licMin, "minor.ppm");
    writePPMFile(WIDTH, HEIGHT, licMaj, "major.ppm");
    const char *resultppm = "lic.ppm";
    writePPMFile2(WIDTH, HEIGHT, licMin,licMaj, resultppm);   
}
//..............................................................................
bool useOldCalc = false;
bool addColor = false;
bool useNewCalc = false;
int Application7::renderWaterColoredImages()
{
    /*
    for(unsigned int i = 0; i < noOfItems; i++)
    {
        
        outFileName = outputFileNames[i];
        noiseType = noiseTypes[i];
        triInterpolationType = triInterpolationTypes[i];
        whichDirection = directions[i];
        
        printf("Rending image %s\n",outFileName);
        
        this->runAntiAliasingRenderer();
        
    }
    */

    useOldCalc = true;
    addColor = true;
    for(unsigned int i = 0; i < 3; i++)
    {
        
        outFileName = outputFileNames[i];
        noiseType = noiseTypes[i];
        triInterpolationType = triInterpolationTypes[i];
        whichDirection = directions[i];
        
        printf("Rending image %s\n",outFileName);
        
        this->runAntiAliasingRenderer();
        
    }
    
    useOldCalc = false;
    useNewCalc = true;
    addColor = true;
    for(unsigned int i = 3; i < 6; i++)
    {
        
        outFileName = outputFileNames[i];
        noiseType = noiseTypes[i];
        triInterpolationType = triInterpolationTypes[i];
        whichDirection = directions[i];
        
        printf("Rending image %s\n",outFileName);
        
        this->runAntiAliasingRenderer();
        
    }
    
    useOldCalc = false;
    useNewCalc = true;
    addColor = false;
    for(unsigned int i = 6; i < 9; i++)
    {
        
        outFileName = outputFileNames[i];
        noiseType = noiseTypes[i];
        triInterpolationType = triInterpolationTypes[i];
        whichDirection = directions[i];
        
        printf("Rending image %s\n",outFileName);
        
        this->runAntiAliasingRenderer();
        
    }
    
    
    return GZ_SUCCESS; // Live happily ever after that :-)   
}
//.................. The End ...................................................
