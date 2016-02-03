/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/


#ifndef APP7_H
#define APP7_H

//#include "disp.h"


struct GzDisplay;
class GzRender;

class Application7
{
public:	
    int renderWaterColoredImages();
    
    
private:
    int runAntiAliasingRenderer();
    int runRenderer(GzDisplay **display, GzRender **render, float delta_x, float delta_y);
    int emitFile(GzDisplay *display);
    
    
    
    void doLIC();
    
    
    
    
    void initLIC(int xres, int yres);
    void cleanupLIC(int xres, int yres);
    
    void makeWhiteNoise(int  xres,  int  yres,  float **noise);    
    void makeDirectionFieldVectors();
    
    
    int generateLICData(int xres, int yres);
    
    void calcLIC(int xres, int yres, 
            float (**directionVec)[2], 
            float ***noise, 
            float kernelLen, 
            float* posDirectionFilter, float* negDirectionFilter, 
            float **result);
};

#endif	/* APP7_H */