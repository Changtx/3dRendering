float dot(GzCoord a, GzCoord b);

void vmult(GzCoord dest, GzCoord a, GzCoord b);
void vdiv(GzCoord dest, GzCoord a, GzCoord b);
void vadd(GzCoord dest, GzCoord a, GzCoord b);
void vsub(GzCoord dest, GzCoord a, GzCoord b);
void vmultc(GzCoord dest, GzCoord a, float b);
void vdivc(GzCoord dest, GzCoord a, float b);
void vaddc(GzCoord dest, GzCoord a, float b);
void vsubc(GzCoord dest, GzCoord a, float b);
void vnorm(GzCoord a);
void vcopy(GzCoord dest, GzCoord a);
float vmag(GzCoord a);
void vcross(GzCoord dest, GzCoord a, GzCoord b);
void verr(GzCoord a);
void vsqrt(GzCoord dest, GzCoord a);
void vsinh(GzCoord dest, GzCoord a);
void vcosh(GzCoord dest, GzCoord a);
void varccoth(GzCoord dest, GzCoord a);
void varctanh(GzCoord dest, GzCoord a);
void vclamp(GzCoord a);
void vexp(GzCoord a);
void vpow(GzCoord dest, GzCoord a, float pow);