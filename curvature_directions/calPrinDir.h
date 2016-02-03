//
//  calPrinDir.h
//  test
//
//  Created by Tian Xiang Chang on 4/27/15.
//  Copyright (c) 2015 tianxiang chang. All rights reserved.
//
#include "gz.h"
#include <vector>
#include <stdio.h>

using namespace std;

#ifndef APP5HEADER
#define APP5HEADER



class AscHelper
{
public:
    GzCoord Vertex[3];
    GzCoord Normal[3];
    GzTextureIndex Texture[3];
public:
    AscHelper() {}
    AscHelper(const AscHelper &src)
    {
        memcpy(this, &src, sizeof(AscHelper));
    }
};
class ObjHelper
{
public:
    GzCoord Vertex;
    GzCoord Normal;
    GzCoord MinCurve;
    GzCoord MaxCurve;
public:
    ObjHelper() {}
    ObjHelper(const ObjHelper &src)
    {
        memcpy(this, &src, sizeof(ObjHelper));
    }
};

class calPrinDir
{
    bool initiated;
    /* principle direction functions */
    void MeshFileInput(void);
    void vertex_neighbours(std::vector<vector<int>> &neighborList);
    void eig2(float& Lambda1, float& Lambda2, vector<float>& I1, vector<float>& I2, float xx, float xy, float yy);
public:
    vector<AscHelper> mesh;
    vector<vector<float>> dir1;
    vector<vector<float>> dir2;
    
    
    vector<vector<float>> normList;
    vector<vector<float>> verticesList;
    vector<vector<float>> vertexnormList;
    vector<vector<vector<int>>> faceList;
    
    calPrinDir();
    ~calPrinDir() {}
    
//    int fastRender(bool = false);
//    virtual int Render();
    
    void principalCurvature(vector<float>& lambda1, vector<float>& lambda2, vector<vector<float>> &Dir1, vector<vector<float>> &Dir2);
};
#endif