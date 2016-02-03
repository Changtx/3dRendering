/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/

#include <string.h>
#include <math.h>

#include "Matrix.h"
#include "Transformations.h"

//#define debug

Matrix::Matrix():dimension(MATRIX_DIMENSION){
    
}

Matrix::Matrix(char *id, unsigned int size):dimension(MATRIX_DIMENSION) 
{
    this->setName(id, size);
}

 void Matrix::setName(char *id, unsigned int size)
 {
     memcpy(this->name, id, size);
     
 }
 
 Matrix::Matrix(GzMatrix mat):dimension(MATRIX_DIMENSION)
{

    memcpy(this->array, mat, sizeof(float)*this->dimension*this->dimension);
    strcpy(this->name, "A Matrix");   
    
}
 
 Matrix::Matrix(Matrix * other):dimension(MATRIX_DIMENSION)
 {
     memcpy(this->array, other->array, sizeof(float)*this->dimension*this->dimension);
 }



Matrix::Matrix(GzMatrix mat, char *id, unsigned int size):dimension(MATRIX_DIMENSION)
{

    memcpy(this->array, mat, sizeof(float)*this->dimension*this->dimension);
    this->setName(id, size);
    
    
    
    
}



Matrix::~Matrix() {
}

unsigned int Matrix::getMatDim()
{
    return this->dimension;
}

void Matrix::printMatrix()
{
#ifdef debug
    unsigned int rows = this->dimension;
    unsigned int cols = rows;
    
    printf("\n");
    printf("%s......\n", this->name);

    for (unsigned int row = 0; row < rows; row++) 
    {
        for (unsigned int col = 0; col < cols; col++) 
        {
            printf("%.15f\t", this->array[row][col]);
        
        }
        printf("\n");
    }
    
     printf("\n");
#endif
            
}


void Matrix::fillMatrix(double d) {
    unsigned int rows = this->dimension;
    unsigned int cols = rows;

    for (unsigned int row = 0; row < rows; row++) 
    {
        for (unsigned int col = 0; col < cols; col++) 
        {
            this->array[row][col] = d;

        }
        
    }
    
}




//Assumption: It is responsibility of caller to free memory occupied by resultant Matrix

Matrix* Matrix::LeftMultiply(Matrix* leftMatrix)
{
    unsigned int rows = leftMatrix->dimension;
    unsigned int cols = rows;
    unsigned int elements = rows;
    
    Matrix *result = new Matrix();
    if (result == NULL)
        return result;
    
    for (unsigned int row = 0; row < rows; row++)
    {
        for(unsigned int col = 0; col < cols; col++)
        {
            float r = 0.0f;
            for(unsigned int traverse = 0; traverse < elements; traverse++ )
            {
                r += leftMatrix->array[row][traverse]*this->array[traverse][col];
            }
            result->array[row][col] = r;
        }
    }//end of outer most for
    
    return result;    
}


bool Matrix::rightMultiply(float *array, unsigned int size, float *result)
{
    if (size != this->dimension)
    {
        return false;
    }
    

    
    
    
    unsigned int rows = this->dimension;
    unsigned int cols = rows;
    
    for (unsigned int row = 0; row < rows; row++)
    {
        float r = 0.0f;
        for(unsigned int col = 0; col < cols; col++)
        {
            r += this->array[row][col]*array[col];
        }
        
        result[row] = r;
    }
    
    result[0] = result[0]/result[3];
    result[1] = result[1]/result[3];
    result[2] = result[2]/result[3];
    result[3] = result[3]/result[3];
    
#ifdef debug
    
    printf("\nModel\n");
    for(unsigned int i = 0; i < size; i++)
    {
        printf("%f\t",array[i]);
    }
    
    printf("\nscreen\n");
    for(unsigned int i = 0; i < size; i++)
    {
        printf("%f\t",result[i]);
    }
#endif
    
    return true;
    
}

void Matrix::makeShadingCaclFriendly()
{
    unsigned int rows = this->dimension;
    unsigned int cols = rows;
    
    // Get rid of any translations by making 3 no in right most col 0  
    for (unsigned int i = 0; i < rows - 1; i++)
    {
        array[i][this->dimension - 1] = 0;
    }
    
    
    // Get rid of any scaling by normalizing row vectors
    
    for (unsigned int i = 0; i < rows - 1; i++)
    {
        float v[3] = {0,0,0};
        float matV[3] = {array[i][0], array[i][1], array[i][2]};
        normalizeVector(matV, v);
        array[i][0] = v[0];
        array[i][1] = v[1];
        array[i][2] = v[2];
        
    }
    
    // Make unitary rotations    
    
    
    float mul = sqrtf((array[0][0]*array[0][0] + 
                       array[0][1]*array[0][1] +
                       array[0][2]*array[0][2]));
    
    float div = 1.0f / mul ;
    

    
    for (unsigned int row = 0; row < rows - 1; row++)
    {        
        for(unsigned int col = 0; col < cols - 1; col++)
        {
            this->array[row][col] = this->array[row][col]*div;
        }   
        
    }
    
    
}


void Matrix::flipSign(unsigned int x, unsigned y)
{
    this->array[x][y] *= (-1.0f); 
}