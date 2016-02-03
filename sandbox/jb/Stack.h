/* 
 * File:   Stack.h
 * Author: aqadeer
 *
 * Created on February 18, 2015, 12:54 AM
 */

#ifndef STACK_H
#define	STACK_H

#include "Matrix.h"

class Stack {
    
public:
    Stack();
    virtual ~Stack();
    Matrix* leftMulMatricesOnStack();
    bool isEmpty();
    bool isFull();
    
    bool push(Matrix *thing);
    bool pop(Matrix **thingHolder);
    
    void emptyStack();
private:
    #define MAX_STACK_CAPACITY 1024

    const unsigned int capacity;
    unsigned int tos;
    Matrix *array[MAX_STACK_CAPACITY];

};

#endif	/* STACK_H */

