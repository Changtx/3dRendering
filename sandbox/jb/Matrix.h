/* 
 * File:   Matrix.h
 * Author: aqadeer
 *
 * Created on February 18, 2015, 11:20 PM
 */

#pragma once
#ifndef MATRIX_H
#define	MATRIX_H

#include <stdlib.h>
#include <stdio.h>

#include "gz.h"

// Following Matrix class is pretty primitive.  But strong enough for HW3
// Assumption 1: Matrix is square
// Assumption 2: Each dimension is 4
// Assumption 3: This matrix is only meant to be used by our Graphics library.
class Matrix {
public:
    Matrix();
    Matrix(char *id, unsigned int size);
    Matrix(GzMatrix mat, char *id, unsigned int size);
    Matrix(GzMatrix mat);
    Matrix(Matrix * other);
    virtual ~Matrix();
    
    Matrix* LeftMultiply(Matrix* leftMatrix);
    void printMatrix();
    void fillMatrix(double d);
    unsigned int getMatDim();
    
    bool rightMultiply(float *array, unsigned int size, float *result);
    
    void makeShadingCaclFriendly();
    
    void flipSign(unsigned int x, unsigned y);
    
private:
    void setName(char *id, unsigned int size);
private:
    #define MATRIX_DIMENSION 4
    const unsigned int dimension;
    float array[MATRIX_DIMENSION][MATRIX_DIMENSION];
    char name[1024];

    

};

#endif	/* MATRIX_H */

