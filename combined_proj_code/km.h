#pragma once

// Project
void calc_unlit_thickness(GzRender *render, GzCoord N_orig, GzCoord ld_L_orig, GzCoord thicknesses);
void calc_diffuse_thickness(GzRender *render, GzCoord N_orig, GzCoord ls_L_orig, GzCoord thicknesses);
void kubelka_munk(GzColor c, GzCoord thickness, GzCoord reflectance, GzCoord transmittance);
void km_composite_layers(GzCoord r, GzCoord t, GzCoord r1, GzCoord t1, GzCoord r2, GzCoord t2);
#define WHITE 0.9999998f
#define BLACK 0.0000001f

void sigmoid(GzCoord a);
void gainAndBias(GzCoord a);