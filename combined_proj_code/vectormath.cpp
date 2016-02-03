#include    "gz.h"
#include    "rend.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>

#define cerr std::cerr
#define endl std::endl

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

void vcross(GzCoord dest, GzCoord a, GzCoord b) {
    dest[0] = a[1]*b[2] - a[2]*b[1];
    dest[1] = a[2]*b[0] - a[0]*b[2];
    dest[2] = a[0]*b[1] - a[1]*b[0];
}

void verr(GzCoord a) {
    // Prints vertex to cerr as [0, 1, 2]
    cerr << "[" << a[0] << ", " << a[1] << ", " << a[2] << "]\n";
}

void vsqrt(GzCoord dest, GzCoord a) {
    dest[0] = sqrt(a[0]);
    dest[1] = sqrt(a[1]);
    dest[2] = sqrt(a[2]);
}

void vsinh(GzCoord dest, GzCoord a) {
    dest[0] = sinh(a[0]);
    dest[1] = sinh(a[1]);
    dest[2] = sinh(a[2]);
}

void vcosh(GzCoord dest, GzCoord a) {
    dest[0] = cosh(a[0]);
    dest[1] = cosh(a[1]);
    dest[2] = cosh(a[2]);
}

void varccoth(GzCoord dest, GzCoord a) {
    dest[0] = atanh(1.0 / a[0]);
    dest[1] = atanh(1.0 / a[1]);
    dest[2] = atanh(1.0 / a[2]);
}

void varctanh(GzCoord dest, GzCoord a) {
    dest[0] = atanh(a[0]);
    dest[1] = atanh(a[1]);
    dest[2] = atanh(a[2]);
}

void vclamp(GzCoord a) {
    for (int i = 0; i < 3; i++)
    {
        if (a[i] < 0) a[i] = 0;
        if (a[i] > 1) a[i] = 1;
    }
}

void vexp(GzCoord a) {
    for (int i = 0; i < 3; i++) a[i] = expf(a[i]);
}

void vpow(GzCoord dest, GzCoord a, float power) {
    dest[0] = powf(a[0], power);
    dest[1] = powf(a[1], power);
    dest[2] = powf(a[2], power);
}
