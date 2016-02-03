/* Texture functions for cs580 GzLib	*/
#include	<stdlib.h>
#include	<stdio.h>
#include    <iostream>
#include	<math.h>
#include	"gz.h"
#include    "rend.h"

#define cerr std::cerr
#define endl std::endl

#define RED_SCALE 20
#define GREEN_SCALE 20
#define BLUE_SCALE 20

GzColor	*image;
int xs, ys;
int reset = 1;

int xy2i(float x, float y);

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char	pixel[3];
	unsigned char	dummy;
	char		foo[8];
	int		i;
	FILE		*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen ("texture", "rb");
		if (fd == NULL) {
			fprintf (stderr, "texture file not found\n");
			return GZ_FAILURE;
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
		if (image == NULL) {
			fprintf (stderr, "malloc for texture image failed\n");
			return GZ_FAILURE;
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
	if (u < 0) u = 0;
	if (u > 1) u = 1;
	if (v < 0) v = 0;
	if (v > 1) v = 1;

	// float u_ = u;
	// float v_ = v;

	u *= xs-1;
	v *= ys-1;

	int ufl = floor(u);
	float uf = 1-(u - floor(u));
	int vfl = floor(v);
	float vf = 1-(v - floor(v));

	/* determine texture cell corner values and perform bilinear interpolation */
	vmultc(color, color, 0);
	GzColor temp;
	// vadd(color, color, image[xy2i(ufl, vfl)]);
	vmultc(temp, image[xy2i(ufl,vfl)], uf*vf);
	vadd(color, color, temp);
	vmultc(temp, image[xy2i(ufl,vfl+1)], uf*(1-vf));
	vadd(color, color, temp);
	vmultc(temp, image[xy2i(ufl+1,vfl)], (1-uf)*vf);
	vadd(color, color, temp);
	vmultc(temp, image[xy2i(ufl+1,vfl+1)], (1-uf)*(1-vf));
	vadd(color, color, temp);

	// cerr << "u was " << u_ << ", v was " << v_ << endl;
	// cerr << "u became " << u << ", v became " << v << " (xs=" << xs << ", ys=" << ys << ")" << endl;
	// cerr << "xy2i(u, v) = " << xy2i(u, v) << endl;

	/* set color to interpolated GzColor value and return */

	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	color[0] = sin(u * RED_SCALE);
	color[1] = cos(v * GREEN_SCALE);
	color[2] = sin((u+v) * BLUE_SCALE)*cos((u-v) * BLUE_SCALE);
	vmultc(color, color, 0.5);
	vaddc(color, color, 0.5);

	return GZ_SUCCESS;
}

int xy2i(float x, float y) {
	return (xs)*y + x;
}