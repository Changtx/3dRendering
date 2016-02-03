/* CS580 Homework 5 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    "disp.h"

#define PI 3.1415926
#define MAX_Z 2147483647
struct vector
{
	float x,y,z;
};
struct Point
{
	float x,y,z;
};
vector lineVector(Point a ,Point b);


vector CalculateVector(Point a, Point b);



vector CrossProduct(vector m, vector n);
short ctoi(float color);
int GzPushNormMatrix(GzRender *render,GzMatrix m);


int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
   for(int i=0;i<4;i++)
	   for(int j=0;j<4;j++)
		   mat[i][j]=0;
   mat[0][0]=1;
   mat[1][1]=mat[2][2]=cos(degree*PI/180);
   mat[1][2]=-sin(degree*PI/180);
   mat[2][1]=sin(degree*PI/180);
   mat[3][3]=1;
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			mat[i][j]=0;
	mat[0][0]=mat[2][2]=cos(degree*PI/180);
	mat[1][1]=1;
	mat[0][2]=sin(degree*PI/180);
	mat[2][0]=-sin(degree*PI/180);
	mat[3][3]=1;
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			mat[i][j]=0;
	mat[0][0]=mat[1][1]=cos(degree*PI/180);
	mat[2][2]=1;
	mat[1][0]=sin(degree*PI/180);
	mat[0][1]=-sin(degree*PI/180);
	mat[3][3]=1;
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			mat[i][j]=0;
	for(int i=0;i<4;i++)
		mat[i][i]=1;
	
	for(int j=0;j<3;j++)
		mat[j][3]=translate[j];
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			mat[i][j]=0;
	for(int i=0;i<3;i++)
		mat[i][i]=scale[i];
	mat[3][3]=1;
	return GZ_SUCCESS;
}
int TriShading(GzRender * render,GzColor color,GzCoord normal)
{
	GzCoord view;
	view[0]=0;
	view[1]=0;
	view[2]=-1;
	float divisor=sqrt(view[0]*view[0]+view[1]*view[1]+view[2]*view[2]);
	for(int i=0;i<3;i++)
		view[i]=view[i]/divisor;
	float nordiv=sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
	for(int j=0;j<3;j++)
		normal[j]=normal[j]/nordiv;
	int *sign=new int[render->numlights];
	float ne=0;
	for(int i=0;i<3;i++)
		ne+=normal[i]*view[i];
	GzCoord *reflection=new GzCoord[render->numlights];
	for(int j=0;j<render->numlights;j++)
	{
		float nl=normal[0]*((*render).lights[j].direction[0])+normal[1]*((*render).lights[j].direction[1])+normal[2]*((*render).lights[j].direction[2]);
	    if(ne>0&&nl>0)
			{sign[j]=1;
		     reflection[j][0]=2*nl*normal[0]-(render->lights[j].direction[0]);
			 reflection[j][1]=2*nl*normal[1]-(render->lights[j].direction[1]);
			 reflection[j][2]=2*nl*normal[2]-(render->lights[j].direction[2]);
			 float divisor=sqrt(reflection[j][0]*reflection[j][0]+reflection[j][1]*reflection[j][1]+reflection[j][2]*reflection[j][2]);
			 reflection[j][0]=reflection[j][0]/divisor;
			 reflection[j][1]=reflection[j][1]/divisor;
			 reflection[j][2]=reflection[j][2]/divisor;
		}
		else if(ne<0&&nl<0)
			{sign[j]=-1;
		     reflection[j][0]=2*nl*(-normal[0])-(render->lights[j].direction[0]);
			 reflection[j][1]=2*nl*(-normal[1])-(render->lights[j].direction[1]);
			 reflection[j][2]=2*nl*(-normal[2])-(render->lights[j].direction[2]);
			 float divisor=sqrt(reflection[j][0]*reflection[j][0]+reflection[j][1]*reflection[j][1]+reflection[j][2]*reflection[j][2]);
			 reflection[j][0]=reflection[j][0]/divisor;
			 reflection[j][1]=reflection[j][1]/divisor;
			 reflection[j][2]=reflection[j][2]/divisor;
		}
		else
		{sign[j]=0;
		 continue;}
    }
	
	
	     
	GzColor diffuse,specular,ampient;
	for(int i=0;i<3;i++)
		diffuse[i]=specular[i]=ampient[i]=0;
	for(int i=0;i<render->numlights;i++)
      { if(sign[i]==0)
			continue;
	   float re=-reflection[i][2];
	   if(re>1)
		   re=1;
	   if(re<0)
		   re=0;
	   float expo=1;
	   for(int num=0;num<render->spec;num++)
		  expo*=re; 
	   specular[0]+=(render->lights[i].color[0])*expo;
	   specular[1]+=(render->lights[i].color[1])*expo;
	   specular[2]+=(render->lights[i].color[2])*expo;
	}
	for(int i=0;i<3;i++)
		specular[i]=specular[i]*render->Ks[i];
	for(int i=0;i<render->numlights;i++)
	{
		if(sign[i]==1)
		{float ptemp=normal[0]*((*render).lights[i].direction[0])+normal[1]*((*render).lights[i].direction[1])+normal[2]*((*render).lights[i].direction[2]);
		 diffuse[0]+=(render->lights[i].color[0])*ptemp;
		 diffuse[1]+=(render->lights[i].color[1])*ptemp;
		 diffuse[2]+=(render->lights[i].color[2])*ptemp;
		}
		else if(sign[i]==-1)
		 
		{float ntemp=(-normal[0])*((*render).lights[i].direction[0])+(-normal[1])*((*render).lights[i].direction[1])+(-normal[2])*((*render).lights[i].direction[2]);
		 diffuse[0]+=(render->lights[i].color[0])*ntemp;
		 diffuse[1]+=(render->lights[i].color[1])*ntemp;
		 diffuse[2]+=(render->lights[i].color[2])*ntemp;
		}
	}
	for(int j=0;j<3;j++)
		diffuse[j]=diffuse[j]*(render->Kd[j]);
	ampient[0]=(render->Ka[0])*(render->ambientlight.color[0]);
	ampient[1]=(render->Ka[1])*(render->ambientlight.color[1]);
	ampient[2]=(render->Ka[2])*(render->ambientlight.color[2]);

	for(int i=0;i<3;i++)
		color[i]=diffuse[i]+specular[i]+ampient[i];
	return GZ_SUCCESS;
	}
int TriShadingGour(GzRender * render,GzColor color,GzCoord normal)
{
    GzCoord view;
	view[0]=0;
	view[1]=0;
	view[2]=-1;
	float divisor=sqrt(view[0]*view[0]+view[1]*view[1]+view[2]*view[2]);
	for(int i=0;i<3;i++)
		view[i]=view[i]/divisor;         //normalization of eye light
	float nordiv=sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
	for(int j=0;j<3;j++)
		normal[j]=normal[j]/nordiv;      // normalization of normal
	int *sign=new int[render->numlights];
	float ne=0;
	for(int i=0;i<3;i++)
		ne+=normal[i]*view[i];
	GzCoord *reflection=new GzCoord[render->numlights];
	for(int j=0;j<render->numlights;j++)
	{
		float nl=normal[0]*((*render).lights[j].direction[0])+normal[1]*((*render).lights[j].direction[1])+normal[2]*((*render).lights[j].direction[2]);
	    if(ne>0&&nl>0)
			{sign[j]=1;
		     reflection[j][0]=2*nl*normal[0]-(render->lights[j].direction[0]);
			 reflection[j][1]=2*nl*normal[1]-(render->lights[j].direction[1]);
			 reflection[j][2]=2*nl*normal[2]-(render->lights[j].direction[2]);
			 float divisor=sqrt(reflection[j][0]*reflection[j][0]+reflection[j][1]*reflection[j][1]+reflection[j][2]*reflection[j][2]);
			 reflection[j][0]=reflection[j][0]/divisor;
			 reflection[j][1]=reflection[j][1]/divisor;
			 reflection[j][2]=reflection[j][2]/divisor;
		}
		else if(ne<0&&nl<0)
			{sign[j]=-1;
		     reflection[j][0]=2*nl*(-normal[0])-(render->lights[j].direction[0]);
			 reflection[j][1]=2*nl*(-normal[1])-(render->lights[j].direction[1]);
			 reflection[j][2]=2*nl*(-normal[2])-(render->lights[j].direction[2]);
			 float divisor=sqrt(reflection[j][0]*reflection[j][0]+reflection[j][1]*reflection[j][1]+reflection[j][2]*reflection[j][2]);
			 reflection[j][0]=reflection[j][0]/divisor;
			 reflection[j][1]=reflection[j][1]/divisor;
			 reflection[j][2]=reflection[j][2]/divisor;
		}
		else
		{sign[j]=0;
		 continue;}
    }
	
	
	     
	GzColor diffuse,specular,ampient;
	for(int i=0;i<3;i++)
		diffuse[i]=specular[i]=ampient[i]=0;
	for(int i=0;i<render->numlights;i++)
      { if(sign[i]==0)
			continue;
	   float re=-reflection[i][2];
	   if(re>1)
		   re=1;
	   if(re<0)
		   re=0;
	   float expo=1;
	   for(int num=0;num<render->spec;num++)
		  expo*=re; 
	   specular[0]+=(render->lights[i].color[0])*expo;
	   specular[1]+=(render->lights[i].color[1])*expo;
	   specular[2]+=(render->lights[i].color[2])*expo;
	}

	for(int i=0;i<render->numlights;i++)
	{
		if(sign[i]==1)
		{float ptemp=normal[0]*((*render).lights[i].direction[0])+normal[1]*((*render).lights[i].direction[1])+normal[2]*((*render).lights[i].direction[2]);
		 diffuse[0]+=(render->lights[i].color[0])*ptemp;
		 diffuse[1]+=(render->lights[i].color[1])*ptemp;
		 diffuse[2]+=(render->lights[i].color[2])*ptemp;
		}
		else if(sign[i]==-1)
		 
		{float ntemp=(-normal[0])*((*render).lights[i].direction[0])+(-normal[1])*((*render).lights[i].direction[1])+(-normal[2])*((*render).lights[i].direction[2]);
		 diffuse[0]+=(render->lights[i].color[0])*ntemp;
		 diffuse[1]+=(render->lights[i].color[1])*ntemp;
		 diffuse[2]+=(render->lights[i].color[2])*ntemp;
		}
	}
	
	ampient[0]=render->ambientlight.color[0];
	ampient[1]=render->ambientlight.color[1];
	ampient[2]=render->ambientlight.color[2];

	for(int i=0;i<3;i++)
		color[i]=diffuse[i]+specular[i]+ampient[i];
	return GZ_SUCCESS;
	}  


















		





//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 
  
   if(renderClass!=GZ_Z_BUFFER_RENDER)
   {return GZ_FAILURE;}


   else{* render=new GzRender();
      (*render)->display=display;
      
	   // initiate the matrix stack
	   for(int i=0;i<4;i++)
		 for(int j=0;j<4;j++)
			 (*render)->Xsp[i][i]=0;
	   
	 (*render)->Xsp[0][0]=(*render)->display->xres/2;
	 (*render)->Xsp[0][3]=(*render)->display->xres/2;
	 (*render)->Xsp[1][1]=-((*render)->display->yres/2);
	 (*render)->Xsp[1][3]=(*render)->display->yres/2;
	 float temp=tan(((*render)->camera.FOV/2)*PI/180);
	 (*render)->Xsp[2][2]=temp*MAX_Z;
	 (*render)->Xsp[3][3]=1;

	 //initiate default camera
     (*render)->renderClass=GZ_Z_BUFFER_RENDER; //initiate renderClass value
	 (*render)->camera.FOV=DEFAULT_FOV;
	 (*render)->camera.lookat[0]=0;
	 (*render)->camera.lookat[1]=0;
	 (*render)->camera.lookat[2]=0;
	 (*render)->camera.position[X]=DEFAULT_IM_X;
	 (*render)->camera.position[Y]=DEFAULT_IM_Y;
	 (*render)->camera.position[Z]=DEFAULT_IM_Z;
	 (*render)->camera.worldup[X]=0;
     (*render)->camera.worldup[Y]=1;
	 (*render)->camera.worldup[Z]=0;
	 (*render)->matlevel=-1;    // top of stack
	 (*render)->open=0;
	 (*render)->numlights=0;
	
	 return GZ_SUCCESS;

}
}

int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/  if(render==NULL)
	   return GZ_FAILURE;
else{
	delete (render->display);
	delete(render->lights);
    delete(render);
	return GZ_SUCCESS;
}
}






int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	if(render==NULL)
	{return GZ_FAILURE;}
    (*render).matlevel=-1;
	float temp=tan(((*render).camera.FOV/2)*PI/180); //compute xpi
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			{if(j==i)
				(*render).camera.Xpi[i][j]=1;
			else
				(*render).camera.Xpi[i][j]=0;}
	(*render).camera.Xpi[3][2]=temp;
	//compute Xiw
	vector tempvect;
	tempvect.x=(*render).camera.lookat[X]-(*render).camera.position[X];
	tempvect.y=(*render).camera.lookat[Y]-(*render).camera.position[Y];
	tempvect.z=(*render).camera.lookat[Z]-(*render).camera.position[Z];
	vector axis_Z;
	axis_Z.x=(tempvect.x)/sqrt(tempvect.x*tempvect.x+tempvect.y*tempvect.y+tempvect.z*tempvect.z);
	axis_Z.y=(tempvect.y)/sqrt(tempvect.x*tempvect.x+tempvect.y*tempvect.y+tempvect.z*tempvect.z);
	axis_Z.z=(tempvect.z)/sqrt(tempvect.x*tempvect.x+tempvect.y*tempvect.y+tempvect.z*tempvect.z);

	vector up,up1;
	
	up.x=((*render).camera.worldup[X]*axis_Z.x+(*render).camera.worldup[Y]*axis_Z.y+(*render).camera.worldup[Z]*axis_Z.z)*axis_Z.x;
	up.y=((*render).camera.worldup[X]*axis_Z.x+(*render).camera.worldup[Y]*axis_Z.y+(*render).camera.worldup[Z]*axis_Z.z)*axis_Z.y;
	up.z=((*render).camera.worldup[X]*axis_Z.x+(*render).camera.worldup[Y]*axis_Z.y+(*render).camera.worldup[Z]*axis_Z.z)*axis_Z.z;
	up1.x=(*render).camera.worldup[X]-up.x;
	up1.y=(*render).camera.worldup[Y]-up.y;
	up1.z=(*render).camera.worldup[Z]-up.z;
	vector axis_Y,axis_X;
	axis_Y.x=up1.x/(sqrt(up1.x*up1.x+up1.y*up1.y+up1.z*up1.z));
	axis_Y.y=up1.y/(sqrt(up1.x*up1.x+up1.y*up1.y+up1.z*up1.z));
	axis_Y.z=up1.z/(sqrt(up1.x*up1.x+up1.y*up1.y+up1.z*up1.z));
	axis_X.x=axis_Y.y*axis_Z.z-axis_Y.z*axis_Z.y;
	axis_X.y=axis_Y.z*axis_Z.x-axis_Y.x*axis_Z.z;
	axis_X.z=axis_Y.x*axis_Z.y-axis_Y.y*axis_Z.x;
	axis_X.x=axis_X.x/(sqrt(axis_X.x*axis_X.x+axis_X.y*axis_X.y+axis_X.z*axis_X.z));
	axis_X.y=axis_X.y/(sqrt(axis_X.x*axis_X.x+axis_X.y*axis_X.y+axis_X.z*axis_X.z));
	axis_X.z=axis_X.z/(sqrt(axis_X.x*axis_X.x+axis_X.y*axis_X.y+axis_X.z*axis_X.z));
	vector C;
	C.x=(*render).camera.position[X];
	C.y=(*render).camera.position[Y];
	C.z=(*render).camera.position[Z];
	float temp_xc,temp_yc,temp_zc;
	temp_xc=-(axis_X.x*C.x+axis_X.y*C.y+axis_X.z*C.z);
	temp_yc=-(axis_Y.x*C.x+axis_Y.y*C.y+axis_Y.z*C.z);
	temp_zc=-(axis_Z.x*C.x+axis_Z.y*C.y+axis_Z.z*C.z);
	(*render).camera.Xiw[0][0]=axis_X.x;
	(*render).camera.Xiw[0][1]=axis_X.y;
	(*render).camera.Xiw[0][2]=axis_X.z;
	(*render).camera.Xiw[0][3]=temp_xc;
	(*render).camera.Xiw[1][0]=axis_Y.x;
	(*render).camera.Xiw[1][1]=axis_Y.y;
	(*render).camera.Xiw[1][2]=axis_Y.z;
	(*render).camera.Xiw[1][3]=temp_yc;
	(*render).camera.Xiw[2][0]=axis_Z.x;
	(*render).camera.Xiw[2][1]=axis_Z.y;
	(*render).camera.Xiw[2][2]=axis_Z.z;
	(*render).camera.Xiw[2][3]=temp_zc;
	for(int i=0;i<3;i++)
		(*render).camera.Xiw[3][i]=0;
	(*render).camera.Xiw[3][3]=1;
	GzMatrix atomMatrix;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			atomMatrix[i][j]=0;
	for(int i=0;i<4;i++)
		atomMatrix[i][i]=1;
	//initiate the Ximage stack
	GzPushMatrix(render,(*render).Xsp);
	GzPushNormMatrix(render,atomMatrix);
	GzPushMatrix(render,(*render).camera.Xpi);
	GzPushNormMatrix(render,atomMatrix);
	GzPushMatrix(render,(*render).camera.Xiw);
	(*render).open=1;
	return GZ_SUCCESS;

}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/  

	(*render).camera.FOV=(*camera).FOV;
	
	 for(int i=0;i<3;i++)
	 {
		 (*render).camera.lookat[i]=(*camera).lookat[i];
		 (*render).camera.position[i]=(*camera).position[i];
		 (*render).camera.worldup[i]=(*camera).worldup[i];
	 }
	 (*render).camera.worldup[X]=(*render).camera.worldup[X]/(sqrt(((*render).camera.worldup[X])*((*render).camera.worldup[X])+((*render).camera.worldup[Y])*((*render).camera.worldup[Y])+((*render).camera.worldup[Z])*((*render).camera.worldup[Z])));
	 (*render).camera.worldup[Y]=(*render).camera.worldup[Y]/(sqrt(((*render).camera.worldup[X])*((*render).camera.worldup[X])+((*render).camera.worldup[Y])*((*render).camera.worldup[Y])+((*render).camera.worldup[Z])*((*render).camera.worldup[Z])));
	 (*render).camera.worldup[Z]=(*render).camera.worldup[Z]/(sqrt(((*render).camera.worldup[X])*((*render).camera.worldup[X])+((*render).camera.worldup[Y])*((*render).camera.worldup[Y])+((*render).camera.worldup[Z])*((*render).camera.worldup[Z])));
	  (*render).Xsp[2][2] = MAX_Z * tan(((*render).camera.FOV / 2.0) * PI / 180.0);
	 
	 
	 return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/ 
	if((*render).matlevel<MATLEVELS)
	{   
		(*render).matlevel+=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				(*render).Ximage[(*render).matlevel][i][j]=matrix[i][j];
       
    GzPushNormMatrix(render,matrix);
    return GZ_SUCCESS;
	}
	
}
int GzPushNormMatrix(GzRender *render,GzMatrix m)
{ float divisor=sqrt(m[0][0]*m[0][0]+m[0][1]*m[0][1]+m[0][2]*m[0][2]+m[0][3]*m[0][3]);
  for(int i=0;i<4;i++)
	  for(int j=0;j<4;j++)
		  m[i][j]=m[i][j]/divisor;
  for(int i=0;i<4;i++)
	  for (int j=0;j<4;j++)
		 (*render).Xnorm[render->matlevel][i][j]=m[i][j];
  return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if(render->matlevel>=0)
	{
		render->matlevel-=1;
	
	return GZ_SUCCESS;
	}
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for(int i=0;i<numAttributes;i++)
	{
		if(nameList[i]==GZ_RGB_COLOR)
			{float *tempcol=(float *)valueList[i];
			(*render).flatcolor[RED]=tempcol[RED];
			(*render).flatcolor[GREEN]=tempcol[GREEN];
			(*render).flatcolor[BLUE]=tempcol[BLUE];}
		else if(nameList[i]==GZ_DIRECTIONAL_LIGHT)
		{
		 GzLight * dilight=(GzLight *)valueList[i];
		 render->lights[render->numlights]=*dilight;
		 render->numlights+=1;
         }
		else if(nameList[i]==GZ_AMBIENT_LIGHT)
		{
            GzLight* amblight = (GzLight*)valueList[i];
			render->ambientlight = *amblight;
		 }
		else if(nameList[i]==GZ_AMBIENT_COEFFICIENT)	
		{
            GzColor* ambcof=(GzColor *)valueList[i];
			for(int num=0;num<3;num++)
			  render->Ka[num]=ambcof[0][num];

		}
		else if(nameList[i]==GZ_DIFFUSE_COEFFICIENT)
		{
          GzColor *difcof=(GzColor *)valueList[i];
		  for(int num=0;num<3;num++)
			  render->Kd[num]=difcof[0][num];
		}
		else if(nameList[i]==GZ_SPECULAR_COEFFICIENT)
		{
          GzColor *specof=(GzColor *)valueList[i];
		  for(int num=0;num<3;num++)
			  render->Ks[num]=specof[0][num];
		}
		else if(nameList[i]==GZ_DISTRIBUTION_COEFFICIENT)
		{
         float * discof=(float *)valueList[i];
		 render->spec=*discof;
		}
		else if(nameList[i]==GZ_INTERPOLATE)
		{
         int * mode=(int *)valueList[i];
		 render->interp_mode=*mode;
		}
		else if(nameList[i]=GZ_TEXTURE_MAP)
		{ render->tex_fun=(GzTexture)valueList[i];
		}
	}
	return GZ_SUCCESS;
}
	

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList){
/* numParts : how many names and values */

/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
//compute Xsm
	GzMatrix  tempmatrix,Xsm,Xn;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		 {Xsm[i][j]=(*render).Ximage[0][i][j];//Xsp
	      Xn[i][j]=(*render).Xnorm[0][i][j];
	}
	 for(int m=1;m<=(*render).matlevel;m++){
		 for(int i=0;i<4;i++)
			 for(int j=0;j<4;j++)
				 {tempmatrix[i][j]=0;
		          for(int k=0;k<4;k++)
					  tempmatrix[i][j]+=Xsm[i][k]*(*render).Ximage[m][k][j];
		         }
	     for(int i=0;i<4;i++)
		     for(int j=0;j<4;j++)
			     Xsm[i][j]=tempmatrix[i][j];
	  }
	 for(int m=1;m<=(*render).matlevel;m++){
		 for(int i=0;i<4;i++)
			 for(int j=0;j<4;j++)
				 {tempmatrix[i][j]=0;
		          for(int k=0;k<4;k++)
					  tempmatrix[i][j]+=Xn[i][k]*(*render).Xnorm[m][k][j];
		         }
	     for(int i=0;i<4;i++)
		     for(int j=0;j<4;j++)
			     Xn[i][j]=tempmatrix[i][j];
	  }


	for(int i=(*render).matlevel;i>=0;i--)
		GzPopMatrix(render);
	GzPushMatrix(render,Xsm);
	GzPushNormMatrix(render,Xn);
	GzCoord * tri_transf=(GzCoord *)calloc(3,sizeof(GzCoord));
	GzCoord * normal_transf=(GzCoord *)calloc(3,sizeof(GzCoord));
	GzTextureIndex* texturecoord = new GzTextureIndex[3];	
    int view_z=1;
	for(int i=0;i<numParts;i++)
	{   
		
	
		if(nameList[i]==GZ_POSITION)
		{   
			
			GzCoord  *tri_temp=(GzCoord *)valueList[i]; //obtain the vertex coordinate of the triangle
			
            for(int num=0;num<3;num++)
			{for(int m=0;m<3;m++)
			  {   tri_transf[num][m]=0;
				  for(int n=0;n<3;n++)
					tri_transf[num][m]+=(*render).Ximage[render->matlevel][m][n]*tri_temp[num][n];
				  tri_transf[num][m]+=(*render).Ximage[render->matlevel][m][3];
		     }
			float w=(*render).Ximage[0][3][3];
			for(int k=0;k<3;k++)
				w+=(*render).Ximage[render->matlevel][3][k]*tri_temp[num][k];
		    
			for(int m=0;m<3;m++)
				tri_transf[num][m]/=w;
			

	        
			
		    if(tri_transf[num][2]<(*render).camera.position[Z])
					{view_z=0;
			          
			        }
			}
		}
		
		if(nameList[i]==GZ_NORMAL)
		{
          GzCoord* Normal = (GzCoord*)valueList[i];
		  Normal[0][0]=Normal[0][0]/sqrt(Normal[0][0]*Normal[0][0]+Normal[0][1]*Normal[0][1]+Normal[0][2]*Normal[0][2]);//normalization for normals
		  Normal[0][1]=Normal[0][1]/sqrt(Normal[0][0]*Normal[0][0]+Normal[0][1]*Normal[0][1]+Normal[0][2]*Normal[0][2]);
		  Normal[0][2]=Normal[0][2]/sqrt(Normal[0][0]*Normal[0][0]+Normal[0][1]*Normal[0][1]+Normal[0][2]*Normal[0][2]);
		  Normal[1][0]=Normal[1][0]/sqrt(Normal[1][0]*Normal[1][0]+Normal[1][1]*Normal[1][1]+Normal[1][2]*Normal[1][2]);
		  Normal[1][1]=Normal[1][1]/sqrt(Normal[1][0]*Normal[1][0]+Normal[1][1]*Normal[1][1]+Normal[1][2]*Normal[1][2]);
		  Normal[1][2]=Normal[1][2]/sqrt(Normal[1][0]*Normal[1][0]+Normal[1][1]*Normal[1][1]+Normal[1][2]*Normal[1][2]);
		  Normal[2][0]=Normal[2][0]/sqrt(Normal[2][0]*Normal[2][0]+Normal[2][1]*Normal[2][1]+Normal[2][2]*Normal[2][2]);
		  Normal[2][1]=Normal[2][1]/sqrt(Normal[2][0]*Normal[2][0]+Normal[2][1]*Normal[2][1]+Normal[2][2]*Normal[2][2]);
		  Normal[2][2]=Normal[2][2]/sqrt(Normal[2][0]*Normal[2][0]+Normal[2][1]*Normal[2][1]+Normal[2][2]*Normal[2][2]);
		  GzMatrix NorMatrix;
          NorMatrix[0][0]=render->Xnorm[render->matlevel][0][0];
		  NorMatrix[0][1]=render->Xnorm[render->matlevel][0][1];
		  NorMatrix[0][2]=render->Xnorm[render->matlevel][0][2];
		  NorMatrix[1][0]=render->Xnorm[render->matlevel][1][0];
		  NorMatrix[1][1]=render->Xnorm[render->matlevel][1][1];
		  NorMatrix[1][2]=render->Xnorm[render->matlevel][1][2];
		  NorMatrix[2][0]=render->Xnorm[render->matlevel][2][0];
		  NorMatrix[2][1]=render->Xnorm[render->matlevel][2][1];
		  NorMatrix[2][2]=render->Xnorm[render->matlevel][2][2];
		  NorMatrix[3][0]=render->Xnorm[render->matlevel][3][0];
		  NorMatrix[3][1]=render->Xnorm[render->matlevel][3][1];
		  NorMatrix[3][2]=render->Xnorm[render->matlevel][3][2];
		 
		  for(int u=0;u<3;u++)
			  {normal_transf[u][0]=NorMatrix[0][0] * Normal[u][X] + NorMatrix[0][1] * Normal[u][Y] + NorMatrix[0][2] * Normal[u][Z];
		       normal_transf[u][1]=NorMatrix[1][0] * Normal[u][X] + NorMatrix[1][1] * Normal[u][Y] + NorMatrix[1][2] * Normal[u][Z];
		       normal_transf[u][2]=NorMatrix[2][0] * Normal[u][X] + NorMatrix[2][1] * Normal[u][Y] + NorMatrix[2][2] * Normal[u][Z];
		      }
		      



		  for(int con=0;con<3;con++)
			 {normal_transf[con][0]=normal_transf[con][0]/sqrt(normal_transf[con][0]*normal_transf[con][0]+normal_transf[con][1]*normal_transf[con][1]+normal_transf[con][2]* normal_transf[con][2]);
		      normal_transf[con][1]= normal_transf[con][1]/sqrt(normal_transf[con][0]*normal_transf[con][0]+normal_transf[con][1]*normal_transf[con][1]+normal_transf[con][2]* normal_transf[con][2]);
			  normal_transf[con][2]=normal_transf[con][2]/sqrt( normal_transf[con][0]*normal_transf[con][0]+normal_transf[con][1]*normal_transf[con][1]+normal_transf[con][2]*normal_transf[con][2]);
		     }
		}
		if(nameList[i]==GZ_TEXTURE_INDEX)
		{

			GzTextureIndex* temptext = (GzTextureIndex*)valueList[i];
			
			 texturecoord[0][0]=temptext[0][0]/(tri_transf[0][Z] / (INT_MAX - tri_transf[0][Z]) + 1);
			  texturecoord[0][1]=temptext[0][1]/(tri_transf[0][Z] / (INT_MAX - tri_transf[0][Z]) + 1);
			   texturecoord[1][0]=temptext[1][0]/(tri_transf[1][Z] / (INT_MAX - tri_transf[1][Z]) + 1);
			  texturecoord[1][1]=temptext[1][1]/(tri_transf[1][Z] / (INT_MAX - tri_transf[1][Z]) + 1);
			   texturecoord[2][0]=temptext[2][0]/(tri_transf[2][Z] / (INT_MAX - tri_transf[2][Z]) + 1);
			  texturecoord[2][1]=temptext[2][1]/(tri_transf[2][Z] / (INT_MAX - tri_transf[2][Z]) + 1);
		}
	}




			if(view_z==1){
			Point M,N,O,P;  // define three vertices of a triangle
			Point M_n,N_n,O_n;
			 
					// get the parameters of the vertexes in the triangle
					M.x=tri_transf[0][0];
					M.y=tri_transf[0][1];
					M.z=tri_transf[0][2];
					N.x=tri_transf[1][0];
					N.y=tri_transf[1][1];
					N.z=tri_transf[1][2];
					O.x=tri_transf[2][0];
					O.y=tri_transf[2][1];
					O.z=tri_transf[2][2];
					//get the parameters of the normals of the triangle
					M_n.x=normal_transf[0][0];
					M_n.y=normal_transf[0][1];
					M_n.z=normal_transf[0][2];
					N_n.x=normal_transf[1][0];
					N_n.y=normal_transf[1][1];
					N_n.z=normal_transf[1][2];
					O_n.x=normal_transf[2][0];
					O_n.y=normal_transf[2][1];
					O_n.z=normal_transf[2][2];

					// get the parameters for expression Ax+By+Cz+d=0
					float A=(N.y - M.y)*(O.z - N.z) - (N.z - M.z)*(O.y - N.y);
					float B = (N.z - M.z)*(O.x - N.x) - (N.x - M.x)*(O.z - N.z);
					float C = (N.x - M.x)*(O.y - N.y) - (N.y - M.y)*(O.x - N.x);
					float D=-A*(M.x)-B*(M.y)-C*(M.z);
					//calculate the area to render
					float area_xmin,area_ymin,area_xmax,area_ymax;

					area_xmin=min(min(M.x,N.x),O.x);
					area_ymin=min(min(M.y,N.y),O.y);
					area_xmax=max(max(M.x,N.x),O.x);
					area_ymax=max(max(M.y,N.y),O.y);

					int area_xma,area_xmi,area_yma,area_ymi;
					area_xmi=floor(area_xmin);
					area_xma=ceil(area_xmax);
					area_ymi=floor(area_ymin);
					area_yma=ceil(area_ymax);

					GzColor one, two, three;
			        GzCoord tempP;

					if (render->interp_mode == GZ_COLOR) {
						if(render->tex_fun==NULL)
			              {TriShading(render,one,normal_transf[0]);
			               TriShading(render,two,normal_transf[1]);
						   TriShading(render,three,normal_transf[2]);}
						else
						{  TriShadingGour(render,one,normal_transf[0]);
			               TriShadingGour(render,two,normal_transf[1]);
						   TriShadingGour(render,three,normal_transf[2]);}
					}
					float area=abs((tri_transf[0][X]*tri_transf[1][Y]+tri_transf[0][Y]*tri_transf[2][X]+tri_transf[1][X]*tri_transf[2][Y]-tri_transf[1][Y]*tri_transf[2][X]-tri_transf[0][Y]*tri_transf[1][X]-tri_transf[0][X]*tri_transf[2][Y])/2);
					for(int i=area_xmi;i<=area_xma;i++){
						for(int j=area_ymi;j<=area_yma;j++){
							P.x=i;
							P.y=j;
							P.z=(-A*i-B*j-D)/C;
							//judge is the point P in the triangle or not--LEE
							vector MN,MO,MP,NM,NO,NP,OM,ON,OP;
							MN=lineVector(M,N);
							MO=lineVector(M,O);
							MP=lineVector(M,P);
							NM=lineVector(N,M);
							NO=lineVector(N,O);
							NP=lineVector(N,P);
							OM=lineVector(O,M);
							ON=lineVector(O,N);
							OP=lineVector(O,P);
							vector v1,v2,v3,v4,v5,v6;
							v1=CrossProduct(MN,MO);
							v2=CrossProduct(MN,MP);
							v3=CrossProduct(NO,NM);
							v4=CrossProduct(NO,NP);
							v5=CrossProduct(OM,ON);
							v6=CrossProduct(OM,OP);
							float doc1,doc2,doc3;
							doc1=v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
							doc2=v3.x*v4.x+v3.y*v4.y+v3.z*v4.z;
							doc3=v5.x*v6.x+v5.y*v6.y+v5.z*v6.z;
							if((doc1>=0)&(doc2>=0)&(doc3>=0)){    // point P is in the triangle
								GzIntensity r, g, b, a;
								GzDepth z = 0;
								GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);
								tempP[X] = i;
						        tempP[Y] = j;
						        tempP[Z] = 1;
							 float tri1,tri2,tri3;
									  tri1=abs((tri_transf[1][X]*tempP[Y]+tri_transf[1][Y]*tri_transf[2][X]+tempP[X]*tri_transf[2][Y]-tempP[Y]*tri_transf[2][X]-tri_transf[1][Y]*tempP[X]-tri_transf[1][X]*tri_transf[2][Y])/2);
									  tri2=abs((tri_transf[0][X]*tempP[Y]+tri_transf[0][Y]*tri_transf[2][X]+tempP[X]*tri_transf[2][Y]-tempP[Y]*tri_transf[2][X]-tri_transf[0][Y]*tempP[X]-tri_transf[0][X]*tri_transf[2][Y])/2);
                                      tri3=abs((tri_transf[0][X]*tempP[Y]+tri_transf[0][Y]*tri_transf[1][X]+tempP[X]*tri_transf[1][Y]-tempP[Y]*tri_transf[1][X]-tri_transf[0][Y]*tempP[X]-tri_transf[0][X]*tri_transf[1][Y])/2);

								//Compare if the new z is smaller than the old z. If new z is smaller, then draw this pixel. Not change the alpha value.
								if (P.z < z)
								{  

									GzTextureIndex UV;
                                   UV[0] = (tri1 * texturecoord[0][0] + tri2 *  texturecoord[1][0] + tri3 *  texturecoord[2][0]) / area * (P.z / (INT_MAX - P.z) + 1);
							       UV[1] = (tri1 * texturecoord[0][1] + tri2 *  texturecoord[1][1] + tri3 *  texturecoord[2][1]) / area * (P.z / (INT_MAX - P.z) + 1);


									GzColor textureColor;
							if (render->tex_fun != NULL) 
							{
								render->tex_fun(UV[0], UV[1], textureColor);
							}
									if(render->interp_mode==GZ_COLOR)
									{ 
									  float Rr = (tri1*one[0] + tri2*two[0] + tri3*three[0]) / area;
								      float Gg= (tri1*one[1] + tri2*two[1] + tri3*three[1]) / area;
								      float Bb = (tri1*one[2] + tri2*two[2] + tri3*three[2]) / area;

									  if (render->tex_fun != NULL) 
								{
									Rr *= textureColor[RED];
									Gg *= textureColor[GREEN];
									Bb *= textureColor[BLUE];
								}

								if (Rr > 1.0)
								{
									Rr = 1.0;
								}
								if (Gg > 1.0)
								{
									Gg = 1.0;
								}
								if (Bb > 1.0)
								{
									Bb = 1.0;
								}
								r = (GzIntensity)ctoi(Rr);
								g = (GzIntensity)ctoi(Gg);
								b = (GzIntensity)ctoi(Bb);
								z = P.z;
							}
								else if(render->interp_mode==GZ_NORMALS)
									{
									  float tri1,tri2,tri3;
									  tri1=abs((tri_transf[1][X]*tempP[Y]+tri_transf[1][Y]*tri_transf[2][X]+tempP[X]*tri_transf[2][Y]-tempP[Y]*tri_transf[2][X]-tri_transf[1][Y]*tempP[X]-tri_transf[1][X]*tri_transf[2][Y])/2);
									  tri2=abs((tri_transf[0][X]*tempP[Y]+tri_transf[0][Y]*tri_transf[2][X]+tempP[X]*tri_transf[2][Y]-tempP[Y]*tri_transf[2][X]-tri_transf[0][Y]*tempP[X]-tri_transf[0][X]*tri_transf[2][Y])/2);
                                      tri3=abs((tri_transf[0][X]*tempP[Y]+tri_transf[0][Y]*tri_transf[1][X]+tempP[X]*tri_transf[1][Y]-tempP[Y]*tri_transf[1][X]-tri_transf[0][Y]*tempP[X]-tri_transf[0][X]*tri_transf[1][Y])/2);

									  GzCoord int_normal;
									  int_normal[X]=(tri1*normal_transf[0][X]+tri2*normal_transf[1][X]+tri3*normal_transf[2][X])/area;
									  int_normal[Y]=(tri1*normal_transf[0][Y]+tri2*normal_transf[1][Y]+tri3*normal_transf[2][Y])/area;
									  int_normal[Z]=(tri1*normal_transf[0][Z]+tri2*normal_transf[1][Z]+tri3*normal_transf[2][Z])/area;

									  int_normal[X]=int_normal[X]/sqrt(int_normal[X]*int_normal[X]+int_normal[Y]*int_normal[Y]+int_normal[Z]*int_normal[Z]);
									  int_normal[Y]=int_normal[Y]/sqrt(int_normal[X]*int_normal[X]+int_normal[Y]*int_normal[Y]+int_normal[Z]*int_normal[Z]);
									  int_normal[Z]=int_normal[Z]/sqrt(int_normal[X]*int_normal[X]+int_normal[Y]*int_normal[Y]+int_normal[Z]*int_normal[Z]);
									  
									  if (render->tex_fun != NULL) 
								{
									render->Ka[RED] = render->Kd[RED] = textureColor[RED];
									render->Ka[GREEN] = render->Kd[GREEN] = textureColor[GREEN];
									render->Ka[BLUE] = render->Kd[BLUE] = textureColor[BLUE];
								}
									  
									  
									  GzColor color;
									  TriShading(render,color,int_normal);
									  r = (GzIntensity)ctoi(color[0]);
								      g = (GzIntensity)ctoi(color[1]);
								      b = (GzIntensity)ctoi(color[2]);
								      z = P.z;
									}
								







									else
									{r = (GzIntensity)ctoi((float)render->flatcolor[0]);
									g = (GzIntensity)ctoi((float)render->flatcolor[1]);
									b = (GzIntensity)ctoi((float)render->flatcolor[2]);
									z = P.z;
									}
									GzPutDisplay(render->display, i, j, r, g, b, a, z);
									

								}
							}

                         }
					}
				}
			

	
	
	return GZ_SUCCESS;
}


/* NOT part of API - just for general assistance */


short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}
vector lineVector(Point a ,Point b)
{
	vector l_vector;
	l_vector.x=b.x-a.x;
	l_vector.y=b.y-a.y;
	l_vector.z=b.z-a.z;
	return l_vector;
}
vector CrossProduct(vector m, vector n)
{
	vector product;
	product.x = m.y*n.z - m.z*n.y;
	product.y = m.z*n.x - m.x*n.z;
	product.z = m.x*n.y - m.y*n.x;
	return product;

}


