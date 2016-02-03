/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/

#include <stdlib.h>
#include <math.h>

#include "Helper.h"

//..............................................................................
void makeSadleDirectionFieldVectors(int xres, int yres, float (**directionVec)[2])
{
    for(int j = 0; j < yres; j++)
        for(int i = 0; i < xres; i++)
	{
            directionVec[j][i][0] = - ( j / (yres - 1.0f) - 0.5f);
            directionVec[j][i][1] =   ( i / (xres - 1.0f) - 0.5f);		   		
    	}
    // Q: Why not normalize the vector here?
    // A: If direction field comes from Tian, you would have
    // to run normalization anyway to make sure it is normalized.
}
//..............................................................................
void normalizeVectorField(int xres, int yres, float (**directionVec)[2])
{
    for(int j = 0; j < yres; j++)
        for(int	i = 0; i < xres; i++)
    	{
            float vecMag = sqrtf((directionVec[j][i][0] * directionVec[j][i][0]) + (directionVec[j][i][1] * directionVec[j][i][1]));
            // We wont want to div by zero, in case mag is zero!
            vecMag = (vecMag == 0.0f)?1:vecMag;
            
            directionVec[j][i][0] = directionVec[j][i][0] / vecMag;
            directionVec[j][i][1] = directionVec[j][i][1] / vecMag; ;
    	}    
}
//..............................................................................
/*
 Box filter is like a rectangle in shape with the property that in a linear interval
 [-L, L] of size (2L + 1) filter values are 1 modulu (2L + 1).  Filter values is 0
 everywhere else.  Therefore to save space, we only generate and store non-zero kernel
 values in a one dimensional array.
 * 
 Though one array can suffice but for code clarity two one dimensional arrays are used.
 One for positive interval and the other for negative interval.  For simplicity 0 is
 included in both arrays. 
 */
void generateBoxFilter(int filterSize, float* posDirectionFilter, float* negDirectionFilter)
{
    for(int i = 0; i < filterSize; i++)
        posDirectionFilter[i] = negDirectionFilter[i] = i;   
}
//..............................................................................
extern short	ctoi(float color);
int writePPMFile2(int xres, int yres, float **result, float **result2, const char *resultppm)
{
    
    FILE *outfile;
    if ((outfile = fopen(resultppm, "wb")) == NULL) 
    {
        printf("Could not open output file for writing %s \n", resultppm);
        return GZ_FAILURE;
    }
    
    fprintf(outfile, "%s\n%d %d\n%d\n", "P6", xres, yres, 255);
    static unsigned char color[3];
    

    for (int y = 0; y < yres; y++)
    { 
        for (int x = 0; x < xres; x++) {              
            
            unsigned char c1 = CalculateFlushableRGB(ctoi(result[y][x]));
            unsigned char c2 = CalculateFlushableRGB(ctoi(result2[y][x]));
            unsigned char c = (c1 + c2)/2;
            
            color[0] = c;
            color[1] = c;
            color[2] = c;
               
            fwrite(color, 1, 3, outfile);
            
        }
    }



    if (fclose(outfile))
        printf("The output file was not closed\n");
    
    
    return GZ_SUCCESS;
}
//..............................................................................
int writePPMFile(int xres, int yres, float **result, const char *resultppm)
{
    
    FILE *outfile;
    if ((outfile = fopen(resultppm, "wb")) == NULL) 
    {
        printf("Could not open output file for writing %s \n", resultppm);
        return GZ_FAILURE;
    }
    
    fprintf(outfile, "%s\n%d %d\n%d\n", "P6", xres, yres, 255);
    static unsigned char color[3];
    

    for (int y = 0; y < yres; y++)
    { 
        for (int x = 0; x < xres; x++) {  

            unsigned char c = CalculateFlushableRGB(ctoi(result[y][x]));

            
            color[0] = c;
            color[1] = c;
            color[2] = c;
    
            fwrite(color, 1, 3, outfile);
            
        }
    }



    if (fclose(outfile))
        printf("The output file was not closed\n");
    
    
    return GZ_SUCCESS;
}
//..............................................................................
/*
 References:
 * (1) http://en.wikipedia.org/wiki/Perlin_noise
 * (2) http://dunnbypaul.net/perlin/
 * 
 */
float makeNoise(int x, int y, int i )
{
   
    int n = (x+i+3) + (y+i+5) * 57;
    n = (n<<13) ^ n;
    return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);    
  
}
//..............................................................................
float smoothNoise(float x, float y, int i)
{    
    float corners = (makeNoise(x-1, y-1, i)+makeNoise(x+1, y-1, i)+makeNoise(x-1, y+1, i)+makeNoise(x+1, y+1, i) ) / 16;
    float sides   = ( makeNoise(x-1, y, i)  +makeNoise(x+1, y, i)  +makeNoise(x, y-1, i)  +makeNoise(x, y+1, i) ) /  8;
    float center  =  makeNoise(x, y, i) / 4;
    return (corners + sides + center);    
}
//..............................................................................
float cosineInterpolate(float x, float y, float w)
{
    float c = w * 3.1415927f;
    float f = (1 - cosf(c)) * 0.5f;
    
    return  (x*(1-f) + y*f);    
}
//..............................................................................
float interpolatedNoise(float x, float y, int i) {
    float integer_X = int(x);
    float fractional_X = x - integer_X;
    float integer_Y = int(y);
    float fractional_Y = y - integer_Y;
    
    float v1 = smoothNoise(integer_X, integer_Y, i);
    float v2 = smoothNoise(integer_X + 1, integer_Y, i);
    float v3 = smoothNoise(integer_X, integer_Y + 1, i);
    float v4 = smoothNoise(integer_X + 1, integer_Y + 1, i);
    
    float i1 = cosineInterpolate(v1, v2, fractional_X);
    float i2 = cosineInterpolate(v3, v4, fractional_X);
    
    return cosineInterpolate(i1, i2, fractional_Y);  
    
}
//..............................................................................
#define PERSISTENCE 0.52145f
#define NUM_OF_OCTAVES 16
float makePerlinNoise(float x, float y)
{
    float total = 0;
    float p = PERSISTENCE;
    int n = NUM_OF_OCTAVES - 1;
    
    float frequency = 0.0f;
    float amplitude = 0.0f;
    
    
    for(int i = 0; i < n; i++)
    {
        frequency = powf(2,i);
        amplitude = powf(p,i);
        
        ///float div = rand() % 512 + 1;
        
        total += interpolatedNoise(x + 0.0001  * frequency, (y + (float)random()/(float)RAND_MAX) * frequency, i) * amplitude;
        //total += interpolatedNoise(((float)random()/(float)RAND_MAX) * frequency, ((float)random()/(float)RAND_MAX) * frequency, i) * amplitude;
        
    }
    
    return total*(rand()%11+0.001); 
}
//..............................................................................
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
#define s_curve_new(t)(t*t*t*(t*(t*6-15)+10))   //weight computation
#define lerp(t,a,b)(a+t*(b-a))
#define B 256
#define BM 255
#define N 4096
#define SUCCESS 1;
static int p[B+B+2];

static double g2[B+B+2][2];

static int start=1;
//..............................................................................

int noise_lk(int height, int width, float ** perlin) {
    int ocavate = 5;
    int zoom = 135;
    double p = 0.5;
    double base = 2;

    for (int m = 0; m < width; m++)
        for (int n = 0; n < height; n++) {
            double getnoise = 0;
            for (int a = 0; a < ocavate - 1; a++) {
                double frequency = pow(base, a);
                double amplitude = pow(p, a);
                getnoise += perlin2D_lk((double(m)) * frequency / zoom, (double(n)) * frequency / zoom) * amplitude;
            }
            int color = (int) (getnoise * 2048.0 + 2048.0);
            if (color > 4095)
                color = 4095;
            if (color < 0)
                color = 0;
            perlin[m][n] = (float) color / 4095.0;
            

        }

    return GZ_SUCCESS;
}
//..............................................................................

double perlin2D_lk(double para1, double para2) {
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, u, v;
    register int i, j;
    if (start) {
        start = 0;
        init_lk();
    }
    setup_lk(para1, &bx0, &bx1, &rx0, &rx1);
    setup_lk(para2, &by0, &by1, &ry0, &ry1);


    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1]; // create 4 randoms ,together with bz0,bz1 to combine 8 ramdom numbers all together 

    sx = s_curve_new(rx0);
    sy = s_curve_new(ry0);



    q = g2[b00];
    u = at2(rx0, ry0);
    q = g2[b10];
    v = at2(rx1, ry0);
    a = lerp(sx, u, v);

    q = g2[b01];
    u = at2(rx0, ry1);
    q = g2[b11];
    v = at2(rx1, ry1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b);
}
//..............................................................................

void setup_lk(double i, int * b0, int *b1, double *r0, double *r1) 
{
    double t = i + N;
    (* b0) = ((int) t) & BM;
    (* b1) = (*b0 + 1) & BM;
    (* r0) = t - (int) t;
    (* r1) = (*r0) - 1;
}
//..............................................................................
void init_lk(void)    //random array p,g1,g2,g3
{
  int i,j,k;
  for(i=0;i<B;i++)    //[0.255]
  {
    p[i]=i;
  
        for(j=0;j<2;j++)
        {
          g2[i][j]=double((rand()%(B+B))-B)/B; //at most [-1,1]
        }
        normalize2_lk(g2[i]);
        
       

  }     //pseudorandom gradient for each point with int coord
  while(--i)
  {
    k=p[i];
    p[i] = p[ j = rand() % B];  //j[0.255]
    p[j]=k;

  }   //the first 256 random number in p[]
  for(i=0;i<B+2;i++)    //[0,257]
  {
    p[B+i]=p[i];    //0->256,1->257,...,257->513
        
    for( j = 0; j < 2; j++)  
            g2[B + i][j] = g2[i][j];
       
        
  }
}   
//..............................................................................
void normalize2_lk(double v[2])  
{  
    double s;  
    s = sqrt(v[0] * v[0] + v[1] * v[1]);  
    v[0] = v[0] / s;  
    v[1] = v[1] / s;  
} 
//..............................................................................