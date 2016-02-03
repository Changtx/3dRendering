
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "ctime"       
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


double noise(double x,double y);
static void init(void);
double  perlin2D (double para1,double para2);
void setup(double i, int * b0, int *b1,double *r0,double *r1) 
   {
    double t =i + N;
   (* b0) = ((int)t) & BM;  
   (* b1) = (*b0+1) & BM; 
   (* r0 )= t - (int)t;
   (* r1)=(*r0) -1; } 
static void normalize2(double v[2])  
{  
    double s;  
    s = sqrt(v[0] * v[0] + v[1] * v[1]);  
    v[0] = v[0] / s;  
    v[1] = v[1] / s;  
} 
static void init(void)    //random array p,g1,g2,g3
{
  int i,j,k;
  for(i=0;i<B;i++)    //[0.255]
  {
    p[i]=i;
  
        for(j=0;j<2;j++)
        {
          g2[i][j]=double((rand()%(B+B))-B)/B; //alomost [-1,1]
        }
        normalize2(g2[i]);
        
       

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
 

int noise(int height,int width,double ** perlin)
{
   int ocavate=5;
   int zoom=135;
   double p=0.5;
   double base=2;
   
   for(int m=0;m<width;m++)
    for(int n=0;n<height;n++)
    {
        double getnoise=0;
        for(int a=0;a<ocavate-1;a++)
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
        perlin[m][n]=(double)color/4095.0;
        
    }

return SUCCESS;

}

double  perlin2D (double para1,double para2)
{
  int bx0,bx1,by0,by1,bz0,bz1,b00,b10,b01,b11;
  double rx0,rx1,ry0,ry1,rz0,rz1,*q,sx,sy,sz,a,b,c,d,u,v;
  register int i,j;
  if(start)
  {
    start=0;
    init();
  }
  setup(para1,&bx0,&bx1,&rx0,&rx1);
  setup(para2,&by0,&by1,&ry0,&ry1);
  

  i=p[bx0];
  j=p[bx1];

  b00 = p[i + by0];  
    b10 = p[j + by0];  
    b01 = p[i + by1];  
    b11 = p[j + by1];// create 4 randoms ,together with bz0,bz1 to combine 8 ramdom numbers all together 

    sx = s_curve_new(rx0);  
    sy = s_curve_new(ry0);  
    


    q = g2[b00]; u = at2(rx0, ry0);  
    q = g2[b10]; v = at2(rx1, ry0); 
    a = lerp(sx, u, v);  
  
    q = g2[b01]; u = at2(rx0, ry1);  
    q = g2[b11]; v = at2(rx1, ry1);  
    b = lerp(sx, u, v);  
  
    return lerp(sy, a ,b);
  
}  