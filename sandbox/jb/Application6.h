
#include "disp.h"

// Application6.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct GzDisplay;
class GzRender;

class Application6
{
public:	
    int runAntiAliasingRenderer();
    
private:
    int runRenderer(GzDisplay **display, GzRender **render, float delta_x, float delta_y);
    int emitFile(GzDisplay *display);
};
