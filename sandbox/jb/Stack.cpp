/* 
 * File:   Stack.cpp
 * Author: aqadeer
 * 
 * Created on February 18, 2015, 12:54 AM
 */

#include "Stack.h"
//#define debug

Stack::Stack():capacity(MAX_STACK_CAPACITY) {
    this->tos = 0;
}



Stack::~Stack() 
{
    while(this->isEmpty() != true)
    {
        Matrix *dummy;
        this->pop(&dummy);
        delete dummy;
    }
}


void Stack::emptyStack()
{
    Matrix *dummy;
    while(this->isEmpty() == false)
    {
        this->pop(&dummy);
        delete dummy;
    }
    
}

bool Stack::isEmpty()
{
 return (this->tos == 0?true:false);   
}

bool Stack::isFull()
{
    return (this->tos == this->capacity?true:false);     
}



// Assumption: This Matrix pointer lifetime is not managed by stack.
bool Stack::push(Matrix *thing)
{
    if (this->isFull())
    {
        return false;
    }
    
    
    this->array[tos] = thing;
    tos = tos + 1;
    
    thing->printMatrix();
    return true;    
}

bool Stack::pop(Matrix **thingHolder)
{
    if (this->isEmpty())
    {
        return false;
    }
    
    tos = tos - 1;
    *thingHolder = this->array[tos];
    
    return true;
}


Matrix* Stack::leftMulMatricesOnStack()
{
    
    if(this->isEmpty())
    {
        return NULL;
    }
    
    Matrix *result;
    
    while(this->tos > 1)
    {
        Matrix *l, *r;
        this->pop(&r);
        this->pop(&l);
  
        result = r->LeftMultiply(l);
        
#ifdef debug
        printf("\nLeft\n");
        l->printMatrix();
        printf("\nRight\n");
        r->printMatrix();
        printf("\nMul result\n");
        result->printMatrix();
#endif
        

        delete l;
        delete r;
        this->push(result); 

        
    }
    
    this->pop(&result);
    this->push(result);
    
     
    
    result->printMatrix();
    
    return result;    
}


