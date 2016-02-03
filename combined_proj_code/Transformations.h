/* 
 * File:   Transformations.h
 * Author: aqadeer
 *
 * Created on February 22, 2015, 2:52 AM
 */

#ifndef TRANSFORMATIONS_H
#define	TRANSFORMATIONS_H

#include <string.h>
#include <math.h>


#include "gz.h"
#include "rend.h"


int prepareXsp(GzMatrix result, unsigned int Xs, unsigned int Ys, float FOV);
int prepareXpc(GzCamera *camera);
int prepareXcw(GzCamera *camera);
void printMatrix(GzCamera *camera, GzMatrix mat, unsigned int rows, unsigned int cols);


//Some helper functions
void normalizeVector(GzCoord vector, GzCoord result);
float vectorDotProduct(GzCoord x, GzCoord y);
void scalarMultiply(GzCoord x, float scalar, GzCoord result);
void scalarMultiplyAndAdd(GzCoord x, float scalar, GzCoord result);
void makeVector(GzCoord point1, GzCoord point2, GzCoord vector);
void vectorCorssProduct(GzCoord x, GzCoord y, GzCoord result);
void addVectors(GzCoord x, GzCoord y, GzCoord result);
void subtractVector(GzCoord from, GzCoord to, GzCoord result);
void vectorMulElementByElement(GzCoord x, GzCoord y, GzColor r);
void printVector(GzCoord x, char* name);

// Object Translation
int GzRotXMat(float degree, GzMatrix mat);
int GzRotYMat(float degree, GzMatrix mat);
int GzRotZMat(float degree, GzMatrix mat);
int GzTrxMat(GzCoord translate, GzMatrix mat);
int GzScaleMat(GzCoord scale, GzMatrix mat);


#endif	/* TRANSFORMATIONS_H */

