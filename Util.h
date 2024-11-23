/* Headers for PBM+ */
/* E.B.             */

typedef unsigned char bit;
typedef unsigned char gray;
typedef struct
{
    unsigned char r, g, b;
    int i, j; // to compute distance here
    unsigned int label;
} pixel;
char pm_getc(FILE *file);
bit pm_getbit(FILE *file);
unsigned char pm_getrawbyte(FILE *file);
int pm_getint(FILE *file);

void pm_erreur(char *);

int compare(const void *a, const void *b);

void binomialFilter3x3(gray *graymapIn, int rows, int cols, gray *graymapOut);

void binomialFilter5x5(gray *graymapIn, int rows, int cols, gray *graymapOut);

void medianFilter3x3(gray *graymapIn, int rows, int cols, gray *graymapOut);

void medianFilter5x5(gray *graymapIn, int rows, int cols, gray *graymapOut);

void computeHistogram(gray *graymap, int rows, int cols, int *histogram);

void stretch(gray *graymapIn, int rows, int cols, gray *graymapOut);

void equalizeHistogram(gray *graymapIn, int rows, int cols, gray *graymapOut);