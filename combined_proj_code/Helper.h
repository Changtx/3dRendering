/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/

#ifndef HELPER_H
#define	HELPER_H

#include "disp.h"

void makeSaddleDirectionFieldVectors(int xres, int yres, float (**directionVec)[2]);
void normalizeVectorField(int xres, int yres, float (**directionVec)[2]);
void generateBoxFilter(int filterSize, float* posDirectionFilter, float* negDirectionFilter);

int writePPMFile(int xres, int yres, float **result, const char *resultppm);
int writePPMFile2(int xres, int yres, float **result, float **result2, const char *resultppm);


float makeNoise(int x, int y, int i);
float smoothNoise(float x, float y, int i);
float cosineInterpolate(float x, float y, float w);
float interpolatedNoise(float x, float y, int i);
float makePerlinNoise(float x, float y);

int noise_lk(int height,int width, float ** perlin);
double  perlin2D_lk (double para1,double para2);
void setup_lk(double i, int * b0, int *b1, double *r0, double *r1);
void init_lk(void);
void normalize2_lk(double v[2]);



#endif	/* HELPER_H */

