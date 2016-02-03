
#if 0
#define DIFFUSE_COLOR {0.5, 0.1, 0.5}
#define AMBIENT_COLOR {0.1, 0.5, 0.5}
#else
#define DIFFUSE_COLOR {0.5, 0.5, 0.1}
#define AMBIENT_COLOR {0.5, 0.2, 0.2}
#endif

#define KM_KA 0.8
#define KM_KD 0.6
#define KM_KS 0.9

#define BIAS 0.45
#define GAIN 0.2
#define SIGMOID_SHARPNESS 6
#define E_DOT_N_SUB 0.3
#define E_DOT_N_POWER 3

#define ZEROS {0.0f, 0.0f, 0.0f}
#define ONES {1.0f, 1.0f, 1.0f}

#define BACKGROUND_REFLECTANCE {0.8f, 0.8f, 0.8f}
#define BACKGROUND_TRANSMITTANCE {0.2f, 0.2f, 0.2f}

#define COLOR_ON_BLACK_FACTOR 0.75

#define UPPER_CLIPPING_THRESHOLD 0.995
#define LOWER_CLIPPING_THRESHOLD 0.005
