#include <math.h>
#define B 256
#define N 4096

static int p[B+B+2];
static float g3[B+B+2][3];
static float g2[B+B+2][2];
static float g1[B+B+2];
static int start=1;

static void init(void)

#define s_curve(t) (t*t*(3.-2.*t))        //weight computation

#define s_curve_new(t)(t*t*t*(t*(t*6-15)+10))
#define lerp(t,a,b)(a+t*(b-a))

 
// b0 is the integer less than vec[i],b1 is the smallest int greater than vec[i]
#define setup(i, b0, b1, r0, r1) \  
    t = vec[i] + N;\  
    b0 = ((int)t) & BM;\  
    b1 = (b0+1) & BM;\  
    r0 = t - (int)t;\          // a vector from the grid point to the computed point
    r1 = r0 - 1;  

perlin3D(float vac[3])
{
	int bx0,bx1,by0,by1,bz0,bz1,b00,b10,b01,b11;
	float rx0,rx1,ry0,ry1,rz0,rz1,*q,sx,sy,sz,a,b,c,d,u,v,t;
	register int i,j;
	if(start)
	{
		start=0;
		init();
	}
	setup(0,bx0,bx1,rx0,rx1);
	setup(1,by0,by1,ry0,ry1);
	setup(2,bz0,bz1,rz0,rz1);

	i=p[bx0];
	j=p[bx1];

	b00 = p[i + by0];  
    b10 = p[j + by0];  
    b01 = p[i + by1];  
    b11 = p[j + by1];// create 4 randoms ,together with bz0,bz1 to combine 8 ramdom numbers all together 

    sx = s_curve(rx0);  
    sy = s_curve(ry0);  
    sz = s_curve(rz0);
#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )
    q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
    q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
    a = lerp(sx, u, v);  
  
    q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0); 
    q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);  
    b = lerp(sx, u, v);  
  
    c = lerp(sy, a, b);  
  
    q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);  
    q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1); 
    a = lerp(sx, u, v);  
  
    q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);  
    q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);  
    b = lerp(sx, u, v);  
  
    d = lerp(sy, a, b);  
  
    return lerp(sz, c, d);// three linear interpolation  
      
  
}  
static void normalize2(float v[2])  
{  
    float s;  
    s = sqrt(v[0] * v[0] + v[1] * v[1]);  
    v[0] = v[0] / s;  
    v[1] = v[1] / s;  
}  
static void normalize3(float v[3])  
{  
    float s;  
    s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);  
    v[0] = v[0] / s;  
    v[1] = v[1] / s;  
    v[2] = v[2] / s;  
} 

static void init(void)    //random array p,g1,g2,g3
{
	int i,j,k;
	for(i=0;i<B;i++)    //[0.255]
	{
		p[i]=i;
		g1[i]=float((rand()%(B+B))-B)/B;  //almost [-1,1]
        for(j=0;j<2;j++)
        {
        	g2[i][j]=float((rand()%(B+B))-B)/B; //alomost [-1,1]
        }
        normalize2(g2[i]);
        for(j=0;j<3;j++)
        {
        	g3[i][j]=float((rand()%(B+B))-B)/B; //alomost [-1,1]
        }
        normalize3(g3[i]);

	}     //pseudorandom gradient for each point with int coord
	while（--i）
	{
		k=p[i];
		p[i] = p[ j = rand() % B];  //j[0.255]
		p[j]=k;

	}   //the first 256 random number in p[]
	for(i=0;i<B+2;i++)    //[0,257]
	{
		p[B+i]=p[i];    //0->256,1->257,...,257->513
		  
		g1[B + i] = g1[i];  
		for( j = 0; j < 2; j++)  
            g2[B + i][j] = g2[i][j];
        for( j = 0; j < 3; j++)  
            g3[B + i][j] = g3[i][j];  
        
	}
}

