#include <stdio.h>
#include <math.h>
#include <string.h>

#include "Transformations.h"
#include "Matrix.h"
#include "Stack.h"
#include "rend.h"
#include "disp.h"
#include "gz.h"
#include "vectormath.h"
#include "kubelkamunkparameters.h"

void sigmoid(GzCoord a) {
    // f(x) = 1/(1+e^(-K(x-0.5)))
    GzCoord ones = ONES;
    vsubc(a, a, 0.5);
    vmultc(a, a, -1.0*SIGMOID_SHARPNESS);
    vexp(a);
    vaddc(a, a, 1);
    vdiv(a, ones, a);
}

void gainAndBias(GzCoord a) {
    // GzCoord ones = ONES;
    vsubc(a, a, BIAS);
    vclamp(a);

    // verr(a);
    sigmoid(a);
    // verr(a);
    // verr(ones);

    vmultc(a, a, GAIN);
}


//------------------------------------------------------------------------------
void calc_unlit_thickness(GzRender *render, GzCoord N_orig, GzCoord ld_L_orig, GzCoord thicknesses) {
    // GzCoord fake_Ka = {0.5f,0.5f,0.53f};

    // N is already Ncm transformed, but not normalized
    GzCoord N = ZEROS;
    normalizeVector(N_orig, N);
    GzCoord ones = ONES;
    
    // L"s direction may also need to be normalized
    GzCoord ld_L = ZEROS;         
    normalizeVector(ld_L_orig, ld_L);

    // float *c = render->camera.position;
    // float *i = render->camera.lookat;
    // GzCoord vector_ci;
    // makeVector(c, i, vector_ci);
    // GzCoord Ee;
    // normalizeVector(vector_ci, Ee);
    // // verr(Ee);
    // float EdotN = vectorDotProduct(Ee, N);
    // // printf("%f\n", EdotN);
        
    // GzCoord E = {0,0,-1};
    // float EdotN = vectorDotProduct(E, N);

    // EdotN = fabs(EdotN);
    // // EdotN = powf(EdotN, E_DOT_N_POWER);
    // GzCoord EdotNvec = ZEROS;
    // vmultc(EdotNvec, ones, EdotN);
    // vsubc(EdotNvec, EdotNvec, E_DOT_N_SUB);
    // vclamp(EdotNvec);
    // vpow(EdotNvec, EdotNvec, E_DOT_N_POWER);

    // if (!shouldCalcLight(N, ld_L, E, N))
    //     return;

    float N_dot_L = vectorDotProduct(N, ld_L);                

    vmultc(ones, ones, render->km_ka);
    scalarMultiply(ones, (1.0f - N_dot_L), thicknesses);
    gainAndBias(thicknesses);
    // vmultc(thicknesses, thicknesses, EdotN);

    // vcopy(thicknesses, EdotNvec);
}

//------------------------------------------------------------------------------
void calc_diffuse_thickness(GzRender *render, GzCoord N_orig, GzCoord ls_L_orig, GzCoord thicknesses) {

    // N is transformed but not normalized
    GzCoord N = ZEROS;
    normalizeVector(N_orig, N);
    GzCoord ones = ONES;

    GzCoord ls_L = ZEROS;
    normalizeVector(ls_L_orig, ls_L);

    // float *c = render->camera.position;
    // float *i = render->camera.lookat;
    // GzCoord vector_ci;
    // makeVector(c, i, vector_ci);
    // GzCoord Ee;
    // normalizeVector(vector_ci, Ee);
    // float EdotN = fabs(vectorDotProduct(Ee, N));

    GzCoord E = {0,0,-1};
    // float EdotN = vectorDotProduct(E, N);

    // EdotN = fabs(EdotN);
    // // EdotN = powf(EdotN, E_DOT_N_POWER);
    // GzCoord EdotNvec = ZEROS;
    // vmultc(EdotNvec, ones, EdotN);
    // vsubc(EdotNvec, EdotNvec, E_DOT_N_SUB);
    // vclamp(EdotNvec);
    // vpow(EdotNvec, EdotNvec, E_DOT_N_POWER);

    // if (!shouldCalcLight(N, ls_L, E, N))
    //     return; 

    float N_dot_L = vectorDotProduct(N, ls_L);
    GzCoord left = ZEROS;
    scalarMultiply(N, N_dot_L * 2.0f, left);
    GzCoord R_orig = ZEROS;
    subtractVector(left, ls_L, R_orig);
    GzCoord R = ZEROS;
    normalizeVector(R_orig, R);

    float RdotE = vectorDotProduct(R,E);
    if (RdotE < 0) RdotE = 0;
    if (RdotE > 1) RdotE = 1;

    float RdotE_to_tha_n = powf(RdotE, render->spec);

    GzCoord specular_component = ZEROS;
    vmultc(specular_component, ones, render->km_ks);
    scalarMultiply(specular_component, RdotE_to_tha_n, specular_component);
    subtractVector(ones, specular_component, specular_component);

    vmultc(ones, ones, render->km_kd);
    vectorMulElementByElement(ones, specular_component, thicknesses);
    vmultc(thicknesses, thicknesses, N_dot_L);
    gainAndBias(thicknesses);
    // vmultc(thicknesses, thicknesses, EdotN);

    // vcopy(thicknesses, EdotNvec);
}

//------------------------------------------------------------------------------
void kubelka_munk(GzColor C, GzCoord thickness, GzCoord reflectance, GzCoord transmittance) {
    // two-RGB-source model from Curtis et al 1997

    for (int i = 0; i < 3; ++i)
    {
        if (C[i] > UPPER_CLIPPING_THRESHOLD) C[i] = UPPER_CLIPPING_THRESHOLD;
        if (C[i] < LOWER_CLIPPING_THRESHOLD) C[i] = LOWER_CLIPPING_THRESHOLD;
    }

    GzColor C_w = ZEROS;
    vcopy(C_w, C);
    GzColor C_b = ZEROS;
    vcopy(C_b, C);
    vmultc(C_b, C_b, COLOR_ON_BLACK_FACTOR); // half as bright as the color on white

    // S = (1/b) * arccoth(((b^2) - (a-C_w)(a-1))/(b(1-C_w)))
    // K = S(a - 1)
    // a = 0.5(C_w + (C_b - C_w + 1)/C_b)
    // b = sqrt(a^2 - 1)

    // GzCoord ones = ONES;
    GzCoord a = ZEROS;
    vsub(a, C_b, C_w);
    vaddc(a, a, 1);
    vdiv(a, a, C_b);
    vadd(a, C_w, a);
    vmultc(a, a, 0.5);

    GzCoord b = ZEROS;
    vmult(b, a, a);
    vsubc(b, b, 1);
    vsqrt(b, b);

    GzColor S = ZEROS;
    GzColor foo = ZEROS;
    for (int i = 0; i < 3; ++i)
    {
        S[i] = atanh((b[i]*(1-C_w[i]))/(b[i]*b[i] - (a[i] - C_w[i])*(a[i] - 1))) / b[i];
    }
    // // ((b^2) - (a-C_w)(a-1))
    // vsub(S, a, C_w);
    // vsubc(foo, a, 1);
    // vmult(S, S, foo);
    // vmult(foo, b, b);
    // vsub(S, foo, S);
    // // (b(1-C_w))
    // vsub(foo, ones, C_w);
    // vmult(foo, b, foo);
    // // divide
    // vdiv(S, S, foo);
    // varccoth(S, S);
    // vdiv(S, S, b);

    GzColor K = ZEROS;
    vsubc(K, a, 1);
    vmult(K, S, K);

    // c = a*sinh(b*S*thickness) + b*cosh(b*S*thickness)
    GzColor c = ZEROS;
    GzCoord bst = ZEROS;
    GzCoord sinhbst = ZEROS;
    GzCoord coshbst = ZEROS;
    vmult(bst, b, S);
    vmult(bst, bst, thickness);
    vsinh(sinhbst, bst);
    vcosh(coshbst, bst);
    vmult(c, a, sinhbst);
    vmult(foo, b, coshbst);
    vadd(c, c, foo);

    // R = sinh(b*S*thickness) / c
    vdiv(reflectance, sinhbst, c);
    // printVector(reflectance, "R");

    // T = b/c
    vdiv(transmittance, b, c);

    // printVector(thickness, "thickness");
    // printVector(C_w, "input color C_w");
    // printVector(C_b, "input color C_b");
    // printVector(K, "A scattering coefficient");
    // printVector(S, "S scattering coefficient");
    // printVector(a, "a");
    // printVector(b, "b");
    // printVector(c, "c");
    // printVector(reflectance, "R");
    // printVector(transmittance, "T");
    // printf("%f\n", atanh(1.0/2.5));
    // printf("\n\n");

    // // one-RGB-source model from Wang & Wang 2007

    // // A is the absorption coefficient for a pigment as a function of wavelength.
    // //   We are approximating at the colors R, G, and B only (however they are defined)
    // GzColor A = {WHITE, WHITE, WHITE};

    // // S is the scattering coefficient for a pigment as a function of wavelength.
    // // S = 2C / (A-C)^2
    // GzColor S = ZEROS;
    // vsub(S, A, C);
    // vmult(S, S, S);
    // vdiv(S, C, S);
    // vmultc(S, S, 2.0f);
    // // printVector(thickness, "thickness");
    // // printVector(C, "input color");
    // // printVector(A, "A scattering coefficient");
    // // printVector(S, "S scattering coefficient");

    // // But if something caused an out-of-bounds issue, fix it
    // for (int i = 0; i < 3; i++)
    //     if (C[i] > WHITE) S[i] = BLACK;

    // // a, b, and c are just helper variables to make life easier
    // // a = 1 + A/S
    // GzCoord a = ZEROS;
    // vdiv(a, A, S);
    // vaddc(a, a, 1);
    // // printVector(a, "a");
    // // b = (a^2 - 1)^0.5
    // GzCoord b = ZEROS;
    // vmult(b, a, a);
    // vsubc(b, b, 1);
    // vsqrt(b, b);
    // // printVector(b, "b");
    // // c = a*sinh(b*S*thickness) + b*cosh(b*S*thickness)
    // GzCoord bst = ZEROS;
    // GzCoord sinhbst = ZEROS;
    // GzCoord c = ZEROS;
    // GzCoord c2 = ZEROS;
    // vmult(bst, b, S);
    // vmult(bst, bst, thickness);
    // vsinh(sinhbst, bst);
    // vcosh(c2, bst);
    // vmult(c, a, sinhbst);
    // vmult(c2, b, c2);
    // vadd(c, c, c2);
    // // printVector(bst, "b*S*thickness");
    // // printVector(c, "c");

    // // R = sinh(b*S*thickness) / c
    // vdiv(reflectance, sinhbst, c);
    // // printVector(reflectance, "R");

    // // T = b/c
    // vdiv(transmittance, b, c);
    // // printVector(transmittance, "T");
}

//------------------------------------------------------------------------------
void km_composite_layers(GzCoord r_dest, GzCoord t_dest, GzCoord r1, GzCoord t1, GzCoord r2, GzCoord t2) {
    // R = r1 + t1*t1*r2/(1-r1*r2)
    // T = t1*t2/(1-r1*r2)

    // printVector(r1, "R1");
    // printVector(t1, "T1");
    // printVector(r2, "R2");
    // printVector(t2, "T2");

    // 1-r1*r2
    GzCoord r = ZEROS;
    GzCoord t = ZEROS;
    GzCoord r1r2 = ZEROS;
    GzCoord ones = ONES;
    vmult(r1r2, r1, r2);
    vsub(r1r2, ones, r1r2);

    vmult(r, t1, t1);
    vmult(r, r, r2);
    vdiv(r, r, r1r2);
    vadd(r, r1, r);

    vmult(t, t1, t2);
    vdiv(t, t, r1r2);

    vcopy(r_dest, r);
    vcopy(t_dest, t);

    // printVector(r, "R_out");
    // printVector(t, "T_out");
    // printf("\n\n");
}
