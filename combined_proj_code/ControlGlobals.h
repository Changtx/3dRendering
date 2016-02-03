/**
 * CS-580 Project Spring 2015
 * Y-NOTS Team: James Bedell, Tian Xiang Chang, Lingjing Kong, Abdul Qadeer
*/

//#ifndef CONTROLGLOBALS_H
//#define	CONTROLGLOBALS_H

#include<stdlib.h>

// Control Globals
/*
    const unsigned int noOfItems = 24;
    const char *outputFileNames[noOfItems] ={"result/wn_bc_output_major.ppm",
                                             "result/wn_bc_output_minor.ppm",
                                             "result/wn_bc_output_majmin.ppm",
                                             "result/wn_nbc_output_major.ppm",
                                             "result/wn_nbc_output_minor.ppm",
                                             "result/wn_nbc_output_majmin.ppm",
                                             "result/pn_bc_output_major.ppm",
                                             "result/pn_bc_output_minor.ppm",
                                             "result/pn_bc_output_majmin.ppm",
                                             "result/pn_nbc_output_major.ppm",
                                             "result/pn_nbc_output_minor.ppm",
                                             "result/pn_nbc_output_majmin.ppm",
                                             "result/wnpn_bc_output_major.ppm",
                                             "result/wnpn_bc_output_minor.ppm",
                                             "result/wnpn_bc_output_majmin.ppm",
                                             "result/wnpn_nbc_output_major.ppm",
                                             "result/wnpn_nbc_output_minor.ppm",
                                             "result/wnpn_nbc_output_majmin.ppm",
                                             "result/pnwn_bc_output_maj.ppm",
                                             "result/pnwn_bc_output_min.ppm",
                                             "result/pnwn_bc_output_majmin.ppm",
                                             "result/pnwn_nbc_output_major.ppm",
                                             "result/pnwn_nbc_output_minor.ppm",
                                             "result/pnwn_nbc_output_majmin.ppm"
                                            }; 
#define WHITE_NOISE     666
#define PERLIN_NOISE    777
#define WHITE_PERLIN    888
#define PERLIN_WHITE    999
    const int noiseTypes[noOfItems] = {WHITE_NOISE, WHITE_NOISE,
                                       WHITE_NOISE, WHITE_NOISE,
                                       WHITE_NOISE, WHITE_NOISE,
                                      PERLIN_NOISE, PERLIN_NOISE,
                                      PERLIN_NOISE, PERLIN_NOISE,
                                      PERLIN_NOISE, PERLIN_NOISE,
                                      WHITE_PERLIN, WHITE_PERLIN,
                                      WHITE_PERLIN, WHITE_PERLIN,
                                      WHITE_PERLIN, WHITE_PERLIN,
                                      PERLIN_WHITE, PERLIN_WHITE,
                                      PERLIN_WHITE, PERLIN_WHITE,
                                      PERLIN_WHITE, PERLIN_WHITE
                                     };
    
#define BARRY_CENTRIC       111
#define NO_BARRY_CENTRIC    222
    const int triInterpolationTypes[noOfItems] = {BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC, NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,
                                                 BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC, NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,
                                                 BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC, NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,
                                                 BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC, NO_BARRY_CENTRIC,NO_BARRY_CENTRIC,NO_BARRY_CENTRIC
                                                };
    
#define MAJOR   100
#define MINOR   200
#define MAJMIN  300
    const int directions[noOfItems] ={MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN,
                                      MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN,MAJOR, MINOR, MAJMIN}; 
   
    const char *outFileName = outputFileNames[0];
    int noiseType = noiseTypes[0];
    int triInterpolationType = triInterpolationTypes[0];
    int whichDirection = directions[0];
 */
//For demo 
const unsigned int noOfItems = 9;
    const char *outputFileNames[noOfItems] ={"result/wn_nbc_output_majmin_1.ppm",
                                             "result/pn_bc_output_majmin_1.ppm",
                                             "result/wnpn_bc_output_majmin_1.ppm",
                                             "result/wn_nbc_output_majmin_2.ppm",
                                             "result/pn_bc_output_majmin_2.ppm",
                                             "result/wnpn_bc_output_majmin_2.ppm",
                                             "result/wn_nbc_output_majmin_3.ppm",
                                             "result/pn_bc_output_majmin_3.ppm",
                                             "result/wnpn_bc_output_majmin_3.ppm"                                             
                                            }; 
#define WHITE_NOISE     666
#define PERLIN_NOISE    777
#define WHITE_PERLIN    888
#define PERLIN_WHITE    999
    const int noiseTypes[noOfItems] = {WHITE_NOISE, PERLIN_NOISE,WHITE_PERLIN,
                                       WHITE_NOISE, PERLIN_NOISE,WHITE_PERLIN,
                                       WHITE_NOISE, PERLIN_NOISE,WHITE_PERLIN
                                     };
    
#define BARRY_CENTRIC       111
#define NO_BARRY_CENTRIC    222
    const int triInterpolationTypes[noOfItems] = {NO_BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC,
                                                  NO_BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC,
                                                  NO_BARRY_CENTRIC,BARRY_CENTRIC,BARRY_CENTRIC                                           
                                                };
    
#define MAJOR   100
#define MINOR   200
#define MAJMIN  300
    const int directions[noOfItems] ={MAJMIN,MAJMIN,MAJMIN,MAJMIN,MAJMIN,MAJMIN,MAJMIN,MAJMIN,MAJMIN}; 
   
    const char *outFileName = outputFileNames[0];
    int noiseType = noiseTypes[0];
    int triInterpolationType = triInterpolationTypes[0];
    int whichDirection = directions[0];




//#endif	/* CONTROLGLOBALS_H */

