#include    "gz.h"
#include    "rend.h"
#include    "disp.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>

#define cerr std::cerr
#define endl std::endl

#define PI 3.1415926535

int GzRotXMat(float degree, GzMatrix mat) {
    // Create rotate matrix : rotate along x axis
    // Pass back the matrix using mat value

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = cos(degree * PI / 180.0);
	mat[1][2] = -sin(degree * PI / 180.0);
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = sin(degree * PI / 180.0);
	mat[2][2] = cos(degree * PI / 180.0);
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

    return GZ_SUCCESS;
}

int GzRotYMat(float degree, GzMatrix mat) {
    // Create rotate matrix : rotate along y axis
    // Pass back the matrix using mat value

	mat[0][0] = cos(degree * PI / 180.0);
	mat[0][1] = 0;
	mat[0][2] = sin(degree * PI / 180.0);
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = -sin(degree * PI / 180.0);
	mat[2][1] = 0;
	mat[2][2] = cos(degree * PI / 180.0);
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

    return GZ_SUCCESS;
}

int GzRotZMat(float degree, GzMatrix mat) {
    // Create rotate matrix : rotate along z axis
    // Pass back the matrix using mat value

	mat[0][0] = cos(degree * PI / 180.0);
	mat[0][1] = -sin(degree * PI / 180.0);
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = sin(degree * PI / 180.0);
	mat[1][1] = cos(degree * PI / 180.0);
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

    return GZ_SUCCESS;
}

int GzTrxMat(GzCoord translate, GzMatrix mat) {
    // Create translation matrix
    // Pass back the matrix using mat value

	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[X];
	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[Y];
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[Z];
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

    return GZ_SUCCESS;
}

int GzScaleMat(GzCoord scale, GzMatrix mat) {
    // Create scaling matrix
    // Pass back the matrix using mat value

	mat[0][0] = scale[X];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;
	mat[1][0] = 0;
	mat[1][1] = scale[Y];
	mat[1][2] = 0;
	mat[1][3] = 0;
	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[Z];
	mat[2][3] = 0;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

    return GZ_SUCCESS;
}

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display) {
    /* 
    - malloc a renderer struct
    - keep closed until BeginRender inits are done
    - span interpolator needs pointer to display for pixel writes
    - check for legal class GZ_Z_BUFFER_RENDER
    */

    if (renderClass != GZ_Z_BUFFER_RENDER) {
        return GZ_FAILURE;
    }

    GzRender* rend = new GzRender();

    // Basic stuff
    rend->renderClass = renderClass;
    rend->display = display;
    rend->open = false;

    // Lighting stuff
    rend->interp_mode = GZ_FLAT;
    GzColor amb = DEFAULT_AMBIENT;
    vcopy(rend->Ka, amb);
    GzColor diff = DEFAULT_DIFFUSE;
    vcopy(rend->Kd, diff);
    GzColor spec = DEFAULT_SPECULAR;
    vcopy(rend->Ks, spec);
    rend->spec = DEFAULT_SPEC;

    // Texture stuff
    // rend->tex_fun = NULL;

    // Camera stuff
    GzCamera camera;
    GzCoord DEFAULT_POSITION = {DEFAULT_IM_X, DEFAULT_IM_Y, DEFAULT_IM_Z};
    vcopy(camera.position, DEFAULT_POSITION);
    GzCoord DEFAULT_WORLDUP = {0, 1, 0};
    vcopy(camera.worldup, DEFAULT_WORLDUP);
    GzCoord DEFAULT_LOOKAT = {0, 0, 0};
    vcopy(camera.lookat, DEFAULT_LOOKAT);
    camera.FOV = DEFAULT_FOV;
    GzPutCamera(rend, &camera);

    // AA stuff
    rend->aashiftx = 0;
    rend->aashifty = 0;

    *render = rend;
    return GZ_SUCCESS;
}

int GzFreeRender(GzRender *render) {
    /* 
    -free all renderer resources
    */

    delete render;

    return GZ_SUCCESS;
}

int GzBeginRender(GzRender  *render) {
    /* 
    - set up for start of each frame - init frame buffer
    */

    render->open = true;

    // set defaults for other values
    render->matlevel = 0;

    GzMatrix ident = {1,0,0,0,
                      0,1,0,0,
                      0,0,1,0,
                      0,0,0,1};

    memcpy(render->Ximage[0], ident, sizeof(ident));
    memcpy(render->Xnorm[0], ident, sizeof(ident));

    int xs = render->display->xres;
    int ys = render->display->yres;
    int zmax = MAXIMUM_Z;

    float d = 1.0 / tan((render->camera.FOV / 2.0) * PI / 180.0);
    float t = 2.0;

    GzMatrix Xsp = {xs/t, 0,     0,      xs/t,
                    0,    -ys/t, 0,      ys/t,
                    0,    0,     zmax/d, 0,
                    0,    0,     0,      1};

    int success = GZ_SUCCESS;

    success |= GzPushMatrix(render, Xsp);
    success |= GzPushMatrix(render, render->camera.Xpi);
    success |= GzPushMatrix(render, render->camera.Xiw);

    return success;
}

int GzPutCamera(GzRender *render, GzCamera *camera) {
    /*
    - overwrite renderer camera structure with new camera definition
    */
    
    vcopy(render->camera.position, camera->position);
    vcopy(render->camera.lookat, camera->lookat);
    vcopy(render->camera.worldup, camera->worldup);

    render->camera.FOV = camera->FOV;

    float d = 1.0 / tan((render->camera.FOV / 2.0) * PI / 180.0);

    GzMatrix Xpi = {1,    0,     0,      0,
                    0,    1,     0,      0,
                    0,    0,     1,      0,
                    0,    0,     1/d,    1};

    memcpy(&(render->camera.Xpi), Xpi, sizeof(Xpi));

    // Calculate the normalized Z component of the camera space from the lookat point

    // Vector from camera.position to camera.lookat
    GzCoord lookat, Zvec;
    vsub(lookat, render->camera.lookat, render->camera.position);
    vdivc(Zvec, lookat, sqrt(dot(lookat, lookat)));

    // Calculate the normalized Y component of the camera space from the upwards direction
    GzCoord up;
    vcopy(up, render->camera.worldup);
    vnorm(up);

    // Subtract off the component orthogonal to Z and renormalize
    GzCoord upp, ZupZ;
    vmultc(ZupZ, Zvec, dot(up, Zvec));
    vsub(upp, up, ZupZ);

    GzCoord Yvec;
    vdivc(Yvec, upp, sqrt(dot(upp, upp)));

    // Calculate the normalized X component of the camera space from a cross product of the other two
    GzCoord Xvec;
    vcross(Xvec, Yvec, Zvec);

    GzCoord Cvec = {dot(render->camera.position, Xvec),
                    dot(render->camera.position, Yvec),
                    dot(render->camera.position, Zvec)};

    GzMatrix Xiw = {Xvec[X], Xvec[Y], Xvec[Z], -Cvec[X],
                    Yvec[X], Yvec[Y], Yvec[Z], -Cvec[Y],
                    Zvec[X], Zvec[Y], Zvec[Z], -Cvec[Z],
                    0,       0,       0,       1};

    memcpy(&(render->camera.Xiw), Xiw, sizeof(Xiw));

    return GZ_SUCCESS;  
}

int GzPushMatrix(GzRender *render, GzMatrix matrix) {
    /*
    - push a matrix onto the Ximage stack
    - check for stack overflow
    */

    if (render->matlevel == MATLEVELS) {
        return GZ_FAILURE;
    }

    GzMatrix result;
    matmultiply(result, render->Ximage[render->matlevel], matrix);

    GzMatrix matrix_norm = {1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1};

    // Don't want to change Xnorm for Xsp or Xpi.  These are always pushed at matlevel=0 and matlevel=1
    if (render->matlevel > 1) {
        // Calculate rotational part only
        float K = 1.0 / sqrt(matrix[0][0]*matrix[0][0] + matrix[0][1]*matrix[0][1] + matrix[0][2]*matrix[0][2]);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                matrix_norm[i][j] = matrix[i][j] / K;
            }
        }
    }

    GzMatrix result_norm;
    matmultiply(result_norm, render->Xnorm[render->matlevel], matrix_norm);

    render->matlevel++;
    memcpy(&(render->Ximage[render->matlevel]), result, sizeof(GzMatrix));
    memcpy(&(render->Xnorm[render->matlevel]), result_norm, sizeof(GzMatrix));

    return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render) {
    /*
    - pop a matrix off the Ximage stack
    - check for stack underflow
    */
    if (render->matlevel == 0) {
        return GZ_FAILURE;
    }

    render->matlevel--;

    return GZ_SUCCESS;
}

int GzPutAttribute(GzRender *render, int numAttributes, GzToken *nameList, GzPointer *valueList) {
    /*
    - set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
    - later set shaders, interpolaters, texture maps, and lights
    */

    for (int i = 0; i < numAttributes; i++) {
        GzToken attrName = nameList[i];
        GzPointer attrVal = valueList[i];

        switch (attrName) {
            case GZ_RGB_COLOR:
            {
                float* color = (float*) *((GzColor*) attrVal);

                if (checkColor(color)) { 
                    cerr << "Color RGB out of range\n";
                    return GZ_FAILURE;
                }
                
                vcopy(render->Ka, color);
                break;
            }
            case GZ_INTERPOLATE:
            {
                render->interp_mode = *((int*)attrVal);
                break;
            }
            case GZ_DIRECTIONAL_LIGHT:
            {
                if (render->numlights >= MAX_LIGHTS) return GZ_FAILURE;

                render->lights[render->numlights] = *((GzLight*) attrVal);
                vnorm(render->lights[render->numlights].direction);
                render->numlights++;

                break;
            }
            case GZ_AMBIENT_LIGHT:
            {
                render->ambientlight = *((GzLight*)attrVal);
                vnorm(render->ambientlight.direction);
                break;
            }
            case GZ_AMBIENT_COEFFICIENT:
            {
                float* color = (float*) *((GzColor*) attrVal);

                if (checkColor(color)) { 
                    cerr << "Color Ka out of range\n";
                    return GZ_FAILURE;
                }

                vcopy(render->Ka, color);
                break;
            }
            case GZ_DIFFUSE_COEFFICIENT:
            {
                float* color = (float*) *((GzColor*) attrVal);

                if (checkColor(color)) { 
                    cerr << "Color Kd out of range\n";
                    return GZ_FAILURE;
                }
                
                vcopy(render->Kd, color);
                break;
            }
            case GZ_SPECULAR_COEFFICIENT:
            {
                float* color = (float*) *((GzColor*) attrVal);

                if (checkColor(color)) { 
                    cerr << "Color Ks out of range\n";
                    return GZ_FAILURE;
                }
                
                vcopy(render->Ks, color);
                break;
            }
            case GZ_DISTRIBUTION_COEFFICIENT:
            {
                render->spec = *((float*)attrVal);
                break;
            }
            case GZ_TEXTURE_MAP:
            {
                render->tex_fun = (GzTexture) attrVal;
                break;
            }
            case GZ_AASHIFTX:
            {
                render->aashiftx = *((float*)attrVal);
                break;
            }
            case GZ_AASHIFTY:
            {
                render->aashifty = *((float*)attrVal);
                break;
            }
            default:
                cerr << "UNKNOWN ATTRIBUTE CODE: " << attrName << "\nExiting due to error.\n\n";
                return GZ_FAILURE;
        }
    }

    return GZ_SUCCESS;
}

int GzPutTriangle(GzRender *render, int numParts, GzToken *nameList, GzPointer *valueList) {
    /* 
    - pass in a triangle description with tokens and values corresponding to
          GZ_NULL_TOKEN:        do nothing - no values
          GZ_POSITION:      3 vert positions in model space
    - Invoke the scan converter and return an error code
    */

    // these are type GzCoord* because they are arrays of three coordinates
    GzCoord* vertices;
    GzCoord* normals;
    GzTextureIndex* texture;

    for (int i = 0; i < numParts; i++) {
        GzToken attrName = nameList[i];
        GzPointer attrVal = valueList[i];
        // fprintf(stderr, "Called putTriangle() with token ID %d\n", attrName);

        switch (attrName) {
            case GZ_POSITION:
            {
                vertices = ((GzCoord*) attrVal);
                transformPoints(vertices, render->Ximage[render->matlevel]);
                break;
            }
            case GZ_NORMAL:
            {
                normals = ((GzCoord*) attrVal);
                transformPoints(normals, render->Xnorm[render->matlevel]);
                break;
            }
            case GZ_TEXTURE_INDEX:
            {
                texture = (GzTextureIndex*) attrVal;
                break;
            }
            case GZ_NULL_TOKEN:
            {
                break;
            }
            default:
                return GZ_FAILURE;
        }
    }

    int triangleStatus = sortVertices(vertices, normals, texture);
    if (triangleStatus == DEGENERATE_TRIANGLE) return GZ_FAILURE;
    addAA(render, vertices);
    putNormalTriangle(render, vertices, normals, texture);

    return GZ_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* NOT part of API - just for general assistance */

short ctoi(float color) {
    short c = (short)((int)(color * ((1 << 12) - 1)));
    return c;
}

void swap(GzCoord* vertices, int rowi, int rowj, int n) {    
    float temp;

    for (int i = 0; i < n; i++)
    {
        temp = vertices[rowi][i];
        vertices[rowi][i] = vertices[rowj][i];
        vertices[rowj][i] = temp;
    }
}

void swapt(GzTextureIndex* vertices, int rowi, int rowj, int n) {    
    float temp;

    for (int i = 0; i < n; i++)
    {
        temp = vertices[rowi][i];
        vertices[rowi][i] = vertices[rowj][i];
        vertices[rowj][i] = temp;
    }
}

int sortVertices(GzCoord* vertices, GzCoord* normals, GzTextureIndex* texture) {
    // sort the vertices CLOCKWISE

    if (vertices[0][1] > vertices[1][1]) {
        swap(vertices, 0, 1, 3);
        swap(normals, 0, 1, 3);
        swapt(texture, 0, 1, 2);
    }
    if (vertices[1][1] > vertices[2][1]) {
        swap(vertices, 1, 2, 3);
        swap(normals, 1, 2, 3);
        swapt(texture, 1, 2, 2);
    }
    if (vertices[0][1] > vertices[1][1]) {
        swap(vertices, 0, 1, 3);
        swap(normals, 0, 1, 3);
        swapt(texture, 0, 1, 2);
    }

    if ((vertices[0][1] == vertices[1][1]) & (vertices[1][1] == vertices[2][1])) {
        // cerr << "Degenerate triangle";
        return DEGENERATE_TRIANGLE;
    }
    if ((vertices[0][2] < 0) | (vertices[1][2] < 0) | (vertices[2][2] < 0)) {
        cerr << "Negative Z makes me cry!  Triangle is:\n";
        cerr << vertices[0][0] << ", " << vertices[0][1] << ", " << vertices[0][2] << "\n";
        cerr << vertices[1][0] << ", " << vertices[1][1] << ", " << vertices[1][2] << "\n";
        cerr << vertices[2][0] << ", " << vertices[2][1] << ", " << vertices[2][2] << "\n";
        // return DEGENERATE_TRIANGLE;
    }
    if (vertices[0][1] == vertices[1][1]) {
        return HORIZONTAL_TOP_EDGE;
    }
    if (vertices[1][1] == vertices[2][1]) {
        return HORIZONTAL_BOTTOM_EDGE;
    }

    float midx = ((vertices[1][1] - vertices[0][1])/(vertices[2][1] - vertices[0][1]))*(vertices[2][0]-vertices[0][0]) + vertices[0][0];
    if (midx < vertices[1][0]) {
        swap(vertices, 1, 2, 3);
        swap(normals, 1, 2, 3);
        swapt(texture, 1, 2, 2);
    }

    return NORMAL_TRIANGLE;
}

int addAA(GzRender* render, GzCoord* vertices) {
    vertices[0][0] += render->aashiftx;
    vertices[1][0] += render->aashiftx;
    vertices[2][0] += render->aashiftx;
    vertices[0][1] += render->aashifty;
    vertices[1][1] += render->aashifty;
    vertices[2][1] += render->aashifty;

    return GZ_SUCCESS;
}

int transformPoints(GzCoord* vertex, GzMatrix transform) {

    for (int i = 0; i < 3; i++) {
        GzCoord temp = {0, 0, 0};
        float w;
        temp[0] = vertex[i][0]*transform[0][0] + vertex[i][1]*transform[0][1] + vertex[i][2]*transform[0][2] + transform[0][3];
        temp[1] = vertex[i][0]*transform[1][0] + vertex[i][1]*transform[1][1] + vertex[i][2]*transform[1][2] + transform[1][3];
        temp[2] = vertex[i][0]*transform[2][0] + vertex[i][1]*transform[2][1] + vertex[i][2]*transform[2][2] + transform[2][3];
        w       = vertex[i][0]*transform[3][0] + vertex[i][1]*transform[3][1] + vertex[i][2]*transform[3][2] + transform[3][3];

        vdivc(vertex[i], temp, w);
    }

    return GZ_SUCCESS;
}

int putNormalTriangle(GzRender* render, GzCoord* vertices, GzCoord* normals, GzTextureIndex* texture) {

    float minx, maxx, miny, maxy;
    getBoundingBox(vertices, &minx, &maxx, &miny, &maxy);

    // Default color is flat coloring
    GzIntensity r, g, b, a;
    r = ctoi(render->flatcolor[0]);
    g = ctoi(render->flatcolor[1]);
    b = ctoi(render->flatcolor[2]);
    a = 0;

    // Color values at vertices - easier to calculate here once
    GzColor c_a, c_b, c_c;
    if (render->interp_mode == GZ_COLOR) {
        GzColor white = {1, 1, 1};
        vcopy(render->Ka, white);
        vcopy(render->Kd, white);
        vcopy(render->Ks, white);
        getColor(c_a, render, vertices[0], normals[0]);
        getColor(c_b, render, vertices[1], normals[1]);
        getColor(c_c, render, vertices[2], normals[2]);
    }

    // Interpolate u,v at vertices for perspective
    float vpz0 = vertices[0][Z] / (MAXIMUM_Z - vertices[0][Z]);
    float vpz1 = vertices[1][Z] / (MAXIMUM_Z - vertices[1][Z]);
    float vpz2 = vertices[2][Z] / (MAXIMUM_Z - vertices[2][Z]);
    texture[0][0] /= (vpz0 + 1);
    texture[0][1] /= (vpz0 + 1);
    texture[1][0] /= (vpz1 + 1);
    texture[1][1] /= (vpz1 + 1);
    texture[2][0] /= (vpz2 + 1);
    texture[2][1] /= (vpz2 + 1);

    // Calculate values required for linear Z-interpolation
    GzCoord d1, d2, zcross;
    vsub(d1, vertices[1], vertices[0]);
    vsub(d2, vertices[2], vertices[0]);
    vcross(zcross, d1, d2);
    float zd = -dot(zcross, vertices[0]);

    // Now, for each pixel in the bounding box...
    for (int y = max(round(miny) - 1, 0); y <= min(maxy, render->display->yres); y++) {
        for (int x = max(round(minx) - 1, 0); x <= min(maxx, render->display->xres); x++) {
            // Linear expression evaluation! Oh boy!
            // Lol no.  Bilinear interpolation is now the way to go.

            // Z-interpolation!
            float z = -(zcross[0]*x + zcross[1]*y + zd) / zcross[2];

            // Z-buffer!
            GzIntensity r_, g_, b_, a_;
            GzDepth z_;
            // Get current values just to check z-buffer
            GzGetDisplay(render->display, x, y, &r_, &g_, &b_, &a_, &z_);
            if (z_ <= z) continue;

            // Now get barycentric coordinates to determine if we're in the triangle
            float barya, baryb, baryc;
            barycentric(&barya, &baryb, &baryc, x, y, z, vertices[0], vertices[1], vertices[2]);
            if ((barya < 0) || (baryb < 0) || (baryc < 0)) continue;

            // Texture fun!
            // u,v interpolation
            float u = (barya*texture[0][0] + baryb*texture[1][0] + baryc*texture[2][0]) * ((z / (MAXIMUM_Z - z)) + 1);
            float v = (barya*texture[0][1] + baryb*texture[1][1] + baryc*texture[2][1]) * ((z / (MAXIMUM_Z - z)) + 1);
            // cerr << "At x=" << x << ", y=" << y << ", the texture coordinates are u=" << u << ", v=" << v << "\n";

            GzColor color;
            (*(render->tex_fun))(u, v, color);
            if (render->interp_mode == GZ_NORMALS) {
                vcopy(render->Kd, color);
                vcopy(render->Ka, color);
            }

            // Figure out color!
            switch (render->interp_mode) {
                case (GZ_FLAT): {
                    // Values already set to what they should be
                    break;
                }
                case (GZ_COLOR): {
                    // Interpolate r,g,b from c_a, c_b, c_c
                    if (interpolateRGB(&r, &g, &b, x, y, z, vertices[0], vertices[1], vertices[2], c_a, c_b, c_c)) {
                        return GZ_FAILURE;
                    }
                    r *= color[0];
                    g *= color[1];
                    b *= color[2];
                    break;
                }
                case (GZ_NORMALS): {
                    // Interpolate normal at x,y,z from normals
                    GzCoord newnormal;
                    if (interpolateNormal(newnormal, x, y, z, vertices[0], vertices[1], vertices[2], normals[0], normals[1], normals[2])) {
                        return GZ_FAILURE;
                    }
                    GzCoord here = {(float) x, (float) y, z};
                    GzColor newcolor;
                    getColor(newcolor, render, here, newnormal);
                    r = ctoi(newcolor[0]);
                    g = ctoi(newcolor[1]);
                    b = ctoi(newcolor[2]);
                    break;
                }

            }

            GzPutDisplay(render->display, x, y, r, g, b, a, z);

        }
        // fprintf(stderr, "Done handling y=%d\n", y);
    }
    // fprintf(stderr, "%s\n", "Done with whole damn loop!");
    
    // Put the texture mapping coordinates back to normal!
    texture[0][0] *= (vpz0 + 1);
    texture[0][1] *= (vpz0 + 1);
    texture[1][0] *= (vpz1 + 1);
    texture[1][1] *= (vpz1 + 1);
    texture[2][0] *= (vpz2 + 1);
    texture[2][1] *= (vpz2 + 1);

    return GZ_SUCCESS;
}

void getBoundingBox(GzCoord* vertices, float* minx, float* maxx, float* miny, float* maxy) {
    *minx = min(vertices[0][0], min(vertices[1][0], vertices[2][0]));
    *maxx = max(vertices[0][0], max(vertices[1][0], vertices[2][0]));
    *miny = min(vertices[0][1], min(vertices[1][1], vertices[2][1]));
    *maxy = max(vertices[0][1], max(vertices[1][1], vertices[2][1]));
}

int getColor(GzColor dest, GzRender* render, GzCoord vertex, GzCoord normal) {

    GzCoord normal2;
    vcopy(normal2, normal);

    vnorm(normal2);

    GzColor C;
    vmult(C, render->Ka, render->ambientlight.color);

    // C += \Sum_L{[Ks * Ie_L * (R . E)^s] + [Kd * Ie_L * (N . L)]}
    for (int i = 0; i < render->numlights; i++) {
        // Calculate E, R
        GzCoord E = {0, 0, -1};

        float N_dot_L = dot(normal2, render->lights[i].direction);
        int nls = sign(N_dot_L);
        int nes = sign(dot(normal2, E));

        // check if both are negative
        if (nls + nes == -2) {
            vmultc(normal2, normal2, -1);
        }
        // check if different signs
        if (nls + nes == 0) {
            continue;
        }

        GzCoord R;
        vmultc(R, normal2, 2*N_dot_L);
        vsub(R, R, render->lights[i].direction);
        vnorm(R);

        float R_dot_E = dot(R, E);
        float yer_boat = pow(R_dot_E, render->spec);

        // C += Ks * Ie_L * (R . E)^s
        GzCoord toadd;
        vmultc(toadd, render->lights[i].color, yer_boat);
        vmult(toadd, toadd, render->Ks);

        vadd(C, C, toadd);

        // C += Kd * Ie_L * (N . L)
        vmultc(toadd, render->lights[i].color, dot(normal2, render->lights[i].direction));
        vmult(toadd, toadd, render->Kd);

        vadd(C, C, toadd);
    }

    vcopy(dest, C);

    return GZ_SUCCESS;
}

float min(float a, float b) {
    if (a < b) return a;
    return b;
}

float max(float a, float b) {
    if (a < b) return b;
    return a;
}

int round(float a) {
    float x = floor(a + 0.5);
    return x;
}

int matmultiply(GzMatrix dest, GzMatrix a, GzMatrix b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += a[i][k] * b[k][j];
            }
            dest[i][j] = sum;
        }
    }

    return GZ_SUCCESS;
}

float dot(GzCoord a, GzCoord b) {
    float x = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    return x;
}

void vmult(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[0] * b[0];
    dest[1] = a[1] * b[1];
    dest[2] = a[2] * b[2];
}

void vadd(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

void vsub(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
}

void vdiv(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[0] / b[0];
    dest[1] = a[1] / b[1];
    dest[2] = a[2] / b[2];
}

void vmultc(GzCoord dest, GzCoord a, float b) {
    dest[0] = a[0] * b;
    dest[1] = a[1] * b;
    dest[2] = a[2] * b;
}

void vaddc(GzCoord dest, GzCoord a, float b) {
    dest[0] = a[0] + b;
    dest[1] = a[1] + b;
    dest[2] = a[2] + b;
}

void vsubc(GzCoord dest, GzCoord a, float b) {
    dest[0] = a[0] - b;
    dest[1] = a[1] - b;
    dest[2] = a[2] - b;
}

void vdivc(GzCoord dest, GzCoord a, float b) {
    dest[0] = a[0] / b;
    dest[1] = a[1] / b;
    dest[2] = a[2] / b;
}

float vmag(GzCoord a) {
    float x = sqrt(dot(a, a));
    return x;
}

void vnorm(GzCoord a) {
    float m = vmag(a);
    vdivc(a, a, m);
}

void vcopy(GzCoord dest, GzCoord a) {
    dest[0] = a[0];
    dest[1] = a[1];
    dest[2] = a[2];
}

int checkColor(float* color) {
    int status = 0;
    status |= (color[0] > 1.0) | (color[0] < 0.0);
    status |= (color[1] > 1.0) | (color[1] < 0.0);
    status |= (color[2] > 1.0) | (color[2] < 0.0);
    return status;
}

void vcross(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[1]*b[2] - a[2]*b[1];
    dest[1] = a[2]*b[0] - a[0]*b[2];
    dest[2] = a[0]*b[1] - a[1]*b[0];
}

void verr(GzCoord a) {
    // Prints vertex to cerr as [0, 1, 2]
    cerr << "[" << a[0] << ", " << a[1] << ", " << a[2] << "]\n";
}

int interpolateRGB(GzIntensity* r, GzIntensity* g, GzIntensity* b, float x, float y, float z, 
    GzCoord v_a, GzCoord v_b, GzCoord v_c, GzColor c_a, GzColor c_b, GzColor c_c) {
    // Interpolates the vertex colors for Gourard shading

    GzColor temp, temp2;

    float barya, baryb, baryc;
    barycentric(&barya, &baryb, &baryc, x, y, z, v_a, v_b, v_c);

    vmultc(temp, c_a, barya);
    vmultc(temp2, c_b, baryb);
    vadd(temp, temp, temp2);
    vmultc(temp2, c_c, baryc);
    vadd(temp, temp, temp2);

    *r = ctoi(temp[0]);
    *g = ctoi(temp[1]);
    *b = ctoi(temp[2]);

    return GZ_SUCCESS;
}

int interpolateNormal(GzCoord normal, float x, float y, float z, 
    GzCoord v_a, GzCoord v_b, GzCoord v_c, GzCoord n_a, GzCoord n_b, GzCoord n_c) {
    // Interpolates the normals from vertices for Phong shading

    GzCoord temp, temp2;

    float barya, baryb, baryc;
    barycentric(&barya, &baryb, &baryc, x, y, z, v_a, v_b, v_c);

    vmultc(temp, n_a, barya);
    vmultc(temp2, n_b, baryb);
    vadd(temp, temp, temp2);
    vmultc(temp2, n_c, baryc);
    vadd(temp, temp, temp2);

    vcopy(normal, temp);

    return GZ_SUCCESS;
}

void barycentric(float* a, float* b, float*c, float x, float y, float z, GzCoord v_a, GzCoord v_b, GzCoord v_c) {

    float A;
    GzCoord vcva, vbva, vcvaXvbva;

    vsub(vcva, v_c, v_a);
    vsub(vbva, v_b, v_a);
    vcross(vcvaXvbva, vcva, vbva);

    GzCoord d_a, d_b, d_c;
    GzCoord here = {x, y, z};
    vsub(d_a, v_a, here);
    vsub(d_b, v_b, here);
    vsub(d_c, v_c, here);

    // Area of whole triangle
    A = vmag(vcvaXvbva);

    // Each coordinate is fraction of that vertex's sub-triangle area over full area (with sign checking)
    GzCoord xcb, xba, xac;
    vcross(xcb, d_c, d_b);
    *a = vmag(xcb) / A;
    vcross(xac, d_a, d_c);
    *b = vmag(xac) / A;
    vcross(xba, d_b, d_a);
    *c = vmag(xba) / A;

    // This allows this method to be used in putTriangle in place of LEE!
    *a *= sign(dot(vcvaXvbva, xcb));
    *b *= sign(dot(vcvaXvbva, xac));
    *c *= sign(dot(vcvaXvbva, xba));

}

int sign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}
