
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

inline double findnoise2(double x,double y);
inline double interpolate(double a,double b,double x);
double noise(double x,double y);
      
 

double * noise(int height,int width，int * display)
{
   int ocavate=10;
   int zoom=285;
   double p=0.5;
   double base=2;
   double perlin[height][width];
   for(int m=0;m<width;m++)
    for(int n=0;n<height;n++)
    {
        double getnoise=0;
        for(int a=0;a<ocvate-1;a++)
        {
            double frequency=pow(base,a);
            double amplitude=pow(p,a);
            getnoise+=perlin2D((double(m))*frequency/zoom,(double(n))*frequency/zoom)*amplitude;
        }
        int color=(int)(getnoise*2048.0+2048.0);
        if(color>4095)
          color=4095;
        if(color<0)
          color=0;
        display[m][n]=color;
    }

return display;

}

inline double interpolate(double a,double b,double x)
{
 double ft=x * 3.1415927;
 double f=(1.0-cos(ft))* 0.5;
 return a*(1.0-f)+b*f;

}
inline double findnoise2(double x,double y)

{

 int n=(int)x+(int)y*57;

 n=(n<<13)^n;

 int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;

 return 1.0-((double)nn/1073741824.0);

}
double noise(double x,double y)

{

 double floorx=(double)((int)x);//This is a cheap way to floor a double integer.

 double floory=(double)((int)y);

 double s,t,u,v;//Integer declaration

 s=findnoise2(floorx,floory);

 t=findnoise2(floorx+1,floory);

 u=findnoise2(floorx,floory+1);//Get the surrounding pixels to calculate the transition.

 v=findnoise2(floorx+1,floory+1);

 double int1=interpolate(s,t,x-floorx);//Interpolate between the values.

 double int2=interpolate(u,v,x-floorx);//Here we use x-floorx, to get 1st dimension.

 return interpolate(int1,int2,y-floory);//Here we use y-floory, to get the 2nd dimension.

}
