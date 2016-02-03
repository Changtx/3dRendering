// Application6.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct GzDisplay;
struct GzRender;

class Application6
{
public:	
	int runRenderer();
protected:
	GzDisplay* displays[6]; // the displays
	GzRender* renders[6];   // the renderers
    GzDisplay* finaldisplay;
};
