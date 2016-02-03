//
//  calPrinDir.cpp
//  test
//
//  Created by Tian Xiang Chang on 4/27/15.
//  Copyright (c) 2015 tianxiang chang. All rights reserved.
//

#include "calPrinDir.h"
#include <ctime>
#include <string>
#include "fstream"
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
using namespace std;

#define	INFILE	"objects/torus.obj"
#define OUTFILE "torus.txt"

struct GzCoord_NEW
{
    float XX;
    float YY;
    float ZZ;
    GzCoord_NEW(string sx, string sy, string sz)
    {
        XX = stof(sx);
        YY = stof(sy);
        ZZ = stof(sz);
    }
    void Print(GzCoord &copy)
    {
        copy[0] = XX;
        copy[1] = YY;
        copy[2] = ZZ;
    }
};
struct GzTextureIndex_NEW
{
    float UU;
    float VV;
    GzTextureIndex_NEW(string su, string sv)
    {
        UU = stof(su);
        VV = stof(sv);
    }
    void Print(GzTextureIndex &copy)
    {
        copy[0] = UU;
        copy[1] = VV;
    }
};
void Tokenize(const string& str, vector<string>& tokens)
{
    string::size_type lastPos = str.find_first_not_of(' ', 0);
    string::size_type pos = str.find_first_of(' ', lastPos);
    
    while ((string::npos != pos) || (string::npos != lastPos))
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(' ', pos);
        pos = str.find_first_of(' ', lastPos);
    }
}
void safeGetline(istream& is, string& t)
{
    t.clear();
    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();
    
    while (true)
    {
        int c = sb->sbumpc();
        switch (c)
        {
            case '\n':
                return;
            case '\r':
                if (sb->sgetc() == '\n')
                    sb->sbumpc();
                return;
            case EOF:
                if (t.empty())
                    is.setstate(std::ios::eofbit);
                return;
            default:
                t += (char)c;
        }
    }
}

void calPrinDir::MeshFileInput()
{
    ifstream infile(string(INFILE));
    string sLine;
    vector<string> cache;
    vector<GzCoord_NEW> Vertexs;
    vector<GzCoord_NEW> Normals;
    vector<GzTextureIndex_NEW> Textures;
    
    /* Push zero ahead */
    Vertexs.push_back(GzCoord_NEW("0", "0", "0"));
    Normals.push_back(GzCoord_NEW("0", "0", "1"));
    Textures.push_back(GzTextureIndex_NEW("0", "0"));
    
    AscHelper toread;
    
    int directinput = 3;
    while (!infile.eof())
    {
        cache.clear();
        safeGetline(infile, sLine);
        if (sLine == "") continue;
        if (directinput != 3)
        {
            sscanf(sLine.data(), "%f %f %f %f %f %f %f %f",
                     &(toread.Vertex[directinput][0]), &(toread.Vertex[directinput][1]), &(toread.Vertex[directinput][2]),
                     &(toread.Normal[directinput][0]), &(toread.Normal[directinput][1]), &(toread.Normal[directinput][2]),
                     &(toread.Texture[directinput][0]), &(toread.Texture[directinput][1]));
            if (++directinput == 3)
            {
                mesh.push_back(toread);
            }
            continue;
        }
        Tokenize(sLine, cache);
        if (cache[0].substr(0, 8) == "triangle")
        {
            directinput = 0;
            continue;
        }
        if (cache[0] == "v")
        {
            GzCoord_NEW s = GzCoord_NEW(cache[1], cache[2], cache[3]);
            Vertexs.push_back(s);
            vector<float> temp(3);
            temp[0] = s.XX;
            temp[1] = s.YY;
            temp[2] = s.ZZ;
            verticesList.push_back(temp);
            continue;
        }
        if (cache[0] == "vn")
        {
            GzCoord_NEW s = GzCoord_NEW(cache[1], cache[2], cache[3]);
            Normals.push_back(s);
            vector<float> temp(3);
            temp[0] = s.XX;
            temp[1] = s.YY;
            temp[2] = s.ZZ;
            normList.push_back(temp);
            continue;
        }
        if (cache[0] == "vt")
        {
            Textures.push_back(GzTextureIndex_NEW(cache[1], cache[2]));
            continue;
        }
        if (cache[0] == "f")
        {
            int record[32][3];
            unsigned int index = 0;
            for (unsigned int i = 1; i < cache.size(); i++)
            {
                if (sscanf(cache[i].data(), "%d/%d/%d", &record[index][0], &record[index][1], &record[index][2]) != 3)
                {
                    if (sscanf(cache[i].data(), "%d//%d", &record[index][0], &record[index][2]) != 2)
                    {
                        sscanf(cache[i].data(), "%d", &record[index][0]);
                        record[index][2] = 0;
                    }
                    record[index][1] = 0;
                }
                index++;
            }
            vector<vector<int>> temp(3);
            temp[0].resize(2);
            temp[1].resize(2);
            temp[2].resize(2);
            temp[0][0] = record[0][0] - 1;
            temp[0][1] = record[0][2] - 1;
            Vertexs[record[0][0]].Print(toread.Vertex[0]);
            Textures[record[0][1]].Print(toread.Texture[0]);
            Normals[record[0][2]].Print(toread.Normal[0]);
            for (unsigned int i = 2; i < index; i++)
            {
                Vertexs[record[i - 1][0]].Print(toread.Vertex[1]);
                Textures[record[i - 1][1]].Print(toread.Texture[1]);
                Normals[record[i - 1][2]].Print(toread.Normal[1]);
                /****/
                Vertexs[record[i][0]].Print(toread.Vertex[2]);
                Textures[record[i][1]].Print(toread.Texture[2]);
                Normals[record[i][2]].Print(toread.Normal[2]);
                mesh.push_back(toread);
                /****/
                temp[1][0] = record[i - 1][0] - 1;
                temp[1][1] = record[i - 1][2] - 1;
                temp[2][0] = record[i][0] - 1;
                temp[2][1] = record[i][2] - 1;
                faceList.push_back(temp);
            }
            continue;
        }
    }
    /* Compute vertexnormList */
    int facesize = faceList.size();
    int vertsize = verticesList.size();
    vertexnormList.resize(vertsize);
    for (int i = 0; i < vertsize; i++)
    {
        int totalnorm = 0;
        vertexnormList[i].resize(3);
        vertexnormList[i][0] = 0.f;
        vertexnormList[i][1] = 0.f;
        vertexnormList[i][2] = 0.f;
        for (int j = 0; j < facesize; j++)
        {
            for (int index = 0; index < 3; index++)
            {
                if (faceList[j][index][0] == i)
                {
                    totalnorm++;
                    int vidx = faceList[j][index][1];
                    vertexnormList[i][0] += normList[vidx][0];
                    vertexnormList[i][1] += normList[vidx][1];
                    vertexnormList[i][2] += normList[vidx][2];
                }
            }
        }
        vertexnormList[i][0] /= totalnorm;
        vertexnormList[i][1] /= totalnorm;
        vertexnormList[i][2] /= totalnorm;
    }
    
    /* Compute principle curvature */
    vector<float> lambda1;
    vector<float> lambda2;
    principalCurvature(lambda1, lambda2, dir1, dir2);
}

static inline void
fNormalized(vector<float> &norm)
{
    float len = sqrtf(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
    norm[0] /= len;
    norm[1] /= len;
    norm[2] /= len;
}
void calPrinDir::vertex_neighbours(vector<vector<int>> &neighborList)
{
    int numV = verticesList.size();
    int numF = faceList.size();
    
    //loop through all faces
    for (int i = 0; i < numF; i++)
    {
        int a = faceList[i][0][0];
        int b = faceList[i][1][0];
        int c = faceList[i][2][0];
        
        //put vertex 1 and 2 to the neighbor list of vertex 0
        neighborList[a].push_back(b);
        neighborList[a].push_back(c);
        //put vertex 0 and 2 to the neighbor list of vertex 1
        neighborList[b].push_back(a);
        neighborList[b].push_back(c);
        //put vertex 0 and 1 to the neighbor list of vertex 2
        neighborList[c].push_back(a);
        neighborList[c].push_back(b);
    }
    //loop through all neighbor arrays and sort then (rotation same as faces)
    for (int i = 0; i < numV; i++)
    {
        vector<int> Pneighf = neighborList[i];
        int start = 0;
        bool found = false;
        for (int index1 = 0; index1 < Pneighf.size(); index1 += 2)
        {
            found = false;
            for (int index2 = 1; index2 < Pneighf.size(); index2 += 2)
            {
                if (Pneighf[index1] == Pneighf[index2])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                start = index1;
                break;
            }
        }
        vector<int> Pneig(2);
        Pneig[0] = Pneighf[start];
        Pneig[1] = Pneighf[start + 1];
        //add the neighbors with respect to original rotation
        for (int j = 1 + (int)(found); j < Pneighf.size() / 2; j++)
        {
            found = false;
            for (int index1 = 0; index1 < Pneighf.size(); index1 += 2)
            {
                if (Pneighf[index1] == Pneighf[Pneighf.size() - 1])
                {
                    bool exist = false;
                    for (int pp = 0; pp < Pneig.size(); pp++)
                    {
                        if (Pneig[pp] == Pneighf[index1 + 1])
                        {
                            exist = true;
                            break;
                        }
                    }
                    if (!exist)
                    {
                        found = true;
                        Pneig.push_back(Pneighf[index1 + 1]);
                    }
                }
            }
            if (!found)
            {
                for (int index = 0; index < Pneighf.size(); index += 2)
                {
                    bool exist = false;
                    for (int pp = 0; pp < Pneig.size(); pp++)
                    {
                        if (Pneig[pp] == Pneighf[index])
                        {
                            exist = true;
                            break;
                        }
                    }
                    if (!exist)
                    {
                        Pneig.push_back(Pneighf[index]);
                        bool exist2 = false;
                        for (int pp = 0; pp < Pneig.size(); pp++)
                        {
                            if (Pneig[pp] == Pneighf[index + 1])
                            {
                                exist2 = true;
                                break;
                            }
                        }
                        if (!exist2)
                        {
                            Pneig.push_back(Pneighf[index + 1]);
                        }
                    }
                }
            }
            
        }
        //add forgotten neighbors
        if (Pneig.size() < Pneighf.size())
        {
            for (int j = 0; j < Pneighf.size(); j++)
            {
                bool exist = false;
                for (int pp = 0; pp < Pneig.size(); pp++)
                {
                    if (Pneig[pp] == Pneighf[j])
                    {
                        exist = true;
                        break;
                    }
                }
                if (!exist)
                {
                    Pneig.push_back(Pneighf[j]);
                }
            }
        }
        neighborList[i] = Pneig;
    }
    return;
    
}
void VectorRotationMatrix(vector<float> norm, vector<vector<float> >& M, vector<vector<float> >& Minv)
{
    fNormalized(norm);
    srand((unsigned)time(0));
    vector<float> l(3), k(3);
    for (int i = 0; i < 3; i++)
        k[i] = rand() / (float)(RAND_MAX);
    l[0] = k[1] * norm[2] - k[2] * norm[1];
    l[1] = k[2] * norm[0] - k[0] * norm[2];
    l[2] = k[0] * norm[1] - k[1] * norm[0];
    fNormalized(l);
    
    k[0] = l[1] * norm[2] - l[2] * norm[1];
    k[1] = l[2] * norm[0] - l[0] * norm[2];
    k[2] = l[0] * norm[1] - l[1] * norm[0];
    fNormalized(k);
    
    for (int i = 0; i < 3; i++)
    {
        Minv[i][0] = norm[i];
        Minv[i][1] = l[i];
        Minv[i][2] = k[i];
    }
    CvMat *matrix = cvCreateMat(3, 3, CV_32FC1);
    CvMat *inverse = cvCreateMat(3, 3, CV_32FC1);
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            cvmSet(matrix, i, j, Minv[i][j]);
        }
    }
    cvInvert(matrix, inverse, CV_SVD);
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            M[i][j] = cvmGet(inverse, i, j);
        }
    }
}
void calPrinDir::eig2(float& Lambda1, float& Lambda2, vector<float>& I1, vector<float>& I2, float xx, float xy, float yy)
{
    float tmp = sqrt((xx - yy)*(xx - yy) + 4 * xy*xy);
    float v2x = 2 * xy;
    float v2y = yy - xx + tmp;
    float mag = sqrt(v2x*v2x + v2y*v2y);
    if (mag != 0)
    {
        v2x /= mag;
        v2y /= mag;
    }
    float v1x = -v2y;
    float v1y = v2x;
    float mu1 = (0.5*(xx + yy + tmp));
    float mu2 = (0.5*(xx + yy - tmp));
    if (abs(mu1) < abs(mu2))
    {
        Lambda1 = mu1;
        Lambda2 = mu2;
        I2[0] = v1x; I2[1] = v1y;
        I1[0] = v2x; I1[1] = v2y;
    }
    else
    {
        Lambda1 = mu2;
        Lambda2 = mu1;
        I2[0] = v2x; I2[1] = v2y;
        I1[0] = v1x; I1[1] = v1y;
    }
}
void calPrinDir::principalCurvature(vector<float>& lambda1, vector<float>& lambda2, vector<vector<float> >&Dir1, vector<vector<float> >& Dir2)
{
    //num of vertices
    int numV = verticesList.size();
    
    //calculate vertices normals??
    
    //rotation matrices
    vector<vector<vector<float> > > M(numV);
    vector<vector<vector<float> > > Minv(numV);
    for (int i = 0; i < numV; i++)
    {
        M[i].resize(3);
        Minv[i].resize(3);
        for (int j = 0; j < 3; j++)
        {
            M[i][j].resize(3);
            Minv[i][j].resize(3);
        }
    }
    for (int i = 0; i < numV; i++)
    {
        VectorRotationMatrix(vertexnormList[i], M[i], Minv[i]);
    }
    
    //neighbors of all vertices
    vector<vector<int> >neighborList(numV);
    vertex_neighbours(neighborList);
    
    lambda1.resize(numV);
    lambda2.resize(numV);
    Dir1.resize(numV);
    Dir2.resize(numV);
    
    for (int i = 0; i < numV; i++)
    {
        Dir1[i].resize(3);
        Dir2[i].resize(3);
        //sort and unique neighbors
        vector<int> nce = neighborList[i];
        for (int j = 0; j < neighborList[i].size(); j++)
        {
            //int a = neighborList[i][j];
            //int b=neighborList[a];
            vector<int> second = neighborList[neighborList[i][j]];
            for (int s = 0; s < second.size(); s++)
            {
                nce.push_back(second[s]);
            }
        }
        sort(nce.begin(), nce.end());
        nce.erase(unique(nce.begin(), nce.end()), nce.end());
        
        //rotate and make normal
        vector<vector<float> > ve;
        for (int j = 0; j < nce.size(); j++)
        {
            vector<float> temp(3);
            temp[0] = verticesList[nce[j]][0];
            temp[1] = verticesList[nce[j]][1];
            temp[2] = verticesList[nce[j]][2];
            ve.push_back(temp);
        }
        vector<vector<float> > we;
        for (int j = 0; j < ve.size(); j++)
        {
            vector<float> temp(3);
            temp[0] = ve[j][0] * Minv[i][0][0] + ve[j][1] * Minv[i][1][0] + ve[j][2] * Minv[i][2][0];
            temp[1] = ve[j][0] * Minv[i][0][1] + ve[j][1] * Minv[i][1][1] + ve[j][2] * Minv[i][2][1];
            temp[2] = ve[j][0] * Minv[i][0][2] + ve[j][1] * Minv[i][1][2] + ve[j][2] * Minv[i][2][2];
            we.push_back(temp);
        }
        vector<float> f(we.size());
        vector<float> x(we.size());
        vector<float> y(we.size());
        for (int j = 0; j < we.size(); j++)
        {
            f[j] = we[j][0];
            x[j] = we[j][1];
            y[j] = we[j][2];
        }
        //fit patch: f(x,y) = ax^2 + by^2 + cxy + dx + ey + f
        vector<vector<float> >fm(f.size());
        for (int j = 0; j < fm.size(); j++)
        {
            fm[j].resize(6);
            fm[j][0] = x[j] * x[j];
            fm[j][1] = y[j] * y[j];
            fm[j][2] = x[j] * y[j];
            fm[j][3] = x[j];
            fm[j][4] = y[j];
            fm[j][5] = 1.0;
        }
        //find x
        vector<vector<float> >invFM(fm);
        vector<float> abcdef(6);
        CvMat *matrixFM = cvCreateMat(fm.size(), 6, CV_32FC1);
        CvMat *matrixInverse = cvCreateMat(6, fm.size(), CV_32FC1);
        for (int index = 0; index < matrixFM->rows; index++)
            for (int j = 0; j < matrixFM->cols; j++)
                cvmSet(matrixFM, index, j, fm[index][j]);
        cvInvert(matrixFM, matrixInverse, CV_SVD);
        CvMat *bMatrix = cvCreateMat(f.size(), 1, CV_32FC1);
        for (int j = 0; j < f.size(); j++)
            cvmSet(bMatrix, j, 0, f[j]);
        CvMat * result = cvCreateMat(6, 1, CV_32FC1);
        cvMatMul(matrixInverse, bMatrix, result);
        for (int j = 0; j < 6; j++)
            abcdef[j] = cvmGet(result, j, 0);
        float a = abcdef[0];
        float b = abcdef[1];
        float c = abcdef[2];
        //Hessian matrix
        float xx = 2 * a;
        float xy = c;
        float yy = 2 * b;
        vector<float> I1(2);
        vector<float> I2(2);
        eig2(lambda1[i], lambda2[i], I1, I2, xx, xy, yy);
        
        //vector<float> dir1(3), dir2(3);
        Dir1[i][0] = I1[0] * M[i][1][0] + I1[1] * M[i][2][0];
        Dir1[i][1] = I1[0] * M[i][1][1] + I1[1] * M[i][2][1];
        Dir1[i][2] = I1[0] * M[i][1][2] + I1[1] * M[i][2][2];
        
        Dir2[i][0] = I2[0] * M[i][1][0] + I2[1] * M[i][2][0];
        Dir2[i][1] = I2[0] * M[i][1][1] + I2[1] * M[i][2][1];
        Dir2[i][2] = I2[0] * M[i][1][2] + I2[1] * M[i][2][2];
        
        fNormalized(Dir1[i]);
        fNormalized(Dir2[i]);
    }
    return;
}

calPrinDir::calPrinDir()
{
    MeshFileInput();
    FILE *outfile;
    if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
    {
        printf("Could not open output file for writing %s \n", OUTFILE);
        return;
    }
    for (unsigned int i = 0; i < faceList.size(); i++)
    {
        fprintf(outfile, "triangle\n");

        for(int loop=0;loop<3;loop++)
        {
            int index1 = faceList[i][loop][0];
            int index2 = faceList[i][loop][1];
            
            fprintf(outfile, "%f %f %f ", verticesList[index1][0], verticesList[index1][1], verticesList[index1][2]);
            fprintf(outfile, "%f %f %f ", normList[index2][0], normList[index2][1], normList[index2][2]);
            fprintf(outfile, "0 0 ");
            fprintf(outfile, "%f %f %f ", dir1[index1][0], dir1[index1][1], dir1[index1][2]);
            fprintf(outfile, "%f %f %f\n", dir2[index1][0], dir2[index1][1], dir2[index1][2]);
        }

    }
}
