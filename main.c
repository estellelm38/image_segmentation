#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Util.h"
#include <math.h>
#include <time.h>

typedef struct
{
    unsigned char r, g, b;
    int i, j; // to compute distance here
    unsigned int label;
} pixel;

void kmeans(pixel *pixmapIn, int K, int rows, int cols, pixel *pixmapOut)
{
    //int seed = time(NULL); 
    int seed = 1731921458;
    printf("seed : %d\n", seed);
    srand(seed);
    //we save the seed
    FILE *file = fopen("seed.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
    }
    fprintf(file, "%d\n", seed);
    fclose(file);
    
    
    // on calcule les dimensions de l'image
    int imageDimension = rows * cols;
    // printf("image dimension : %d\n", imageDimension);
    int p;

    // fix cluster centers in the two dimensions image randomly
    pixel cluster[K];

    for (int k = 0; k < K; k++)
    {

        // on choisit un pixel au hasard
        p = rand() % imageDimension; /* generating random coords to initialize the center ? */
        // printf("p : %d\n", p);

        // pour retrouver les indices des composantes correspondant au pixel choisi aléatoirement
        // 3n - 2 = r, 3n - 1 = g, 3n = b

        cluster[k].r = pixmapIn[p].r;
        // printf("r : %d\n", cluster[k].r);
        cluster[k].g = pixmapIn[p].g;
        // printf("g : %d\n", cluster[k].g);
        cluster[k].b = pixmapIn[p].b;
        // printf("b : %d\n", cluster[k].b);
        cluster[k].label = k;
        // printf("label : %d\n", cluster[k].label);
        cluster[k].i = pixmapIn[p].i;
        //printf("i : %d\n", cluster[k].i);
        cluster[k].j = pixmapIn[p].j;
        //printf("j : %d\n", cluster[k].j);
    }
    // allocate each pixel of the image to the nearest cluster center
    int i = 0;
    int changes_in_cluster =rows*cols;
    int loops =0;
    while(  changes_in_cluster > rows*cols / 100 )
    {   loops ++;
        printf("i is : %d\n",i);
        changes_in_cluster =0;
        i++;
        // printf("iteration : %d\n", i);
        int associatedK = 0;
        float color_weight = 0.80;
        float distance_weight = 1 - color_weight;
  
        for (p = 0; p < imageDimension; p++)
        {

            float min = 2 * rows * cols; // max distance between two colors
            // printf("min : %d\n", min);

            for (int k = 0; k < K; k++)
            {

                int colorDistance = sqrt(pow((float)(pixmapIn[p].r - cluster[k].r)/255, 2) + pow((float)(pixmapIn[p].g - cluster[k].g)/255, 2) + pow((float)(pixmapIn[p].b - cluster[k].b)/255, 2));
                // printf("color distance : %d\n", colorDistance);
                int realDistance = sqrt(pow((float)(pixmapIn[p].i - cluster[k].i)/rows, 2) + pow((float)(pixmapIn[p].j - cluster[k].j)/cols, 2));
                // printf("real distance : %d\n", realDistance);
                // j'ai un doute sur le calcul de combination, sans parler des poids
                int combination = color_weight * colorDistance + distance_weight * realDistance;
                // printf("combination : %d\n", combination);
                if (combination < min)
                {
                    min = combination;
                    associatedK = k;
                }
            }
            // assign the pixel to the cluster with the minimum distance
            if(pixmapIn[p].label!=associatedK){changes_in_cluster++;}
            pixmapIn[p].label = associatedK;
            // printf("associatedK : %d\n", associatedK);
        }

        for (int k = 0; k < K; k++)
        {

            int sum_r = 0;
            int sum_g = 0;
            int sum_b = 0;
            int count = 0;
            int sumi = 0;
            int sumj = 0;

            for (p = 0; p < imageDimension; p++)
            {
                if (pixmapIn[p].label == k)
                {
                    sum_r += pixmapIn[p].r;
                    sum_g += pixmapIn[p].g;
                    sum_b += pixmapIn[p].b;
                    sumi += pixmapIn[p].i;
                    sumj += pixmapIn[p].j;
                    count++;
                }
            }
            if (count != 0)
            {
                cluster[k].r = sum_r / count;
                cluster[k].g = sum_g / count;
                cluster[k].b = sum_b / count;
                cluster[k].i = sumi / count;
                cluster[k].j = sumj / count;
            }
        }
    }
    printf("LOOPS ARE : %d \n",loops -3);
    for (p = 0; p < imageDimension; p++)
    {
        pixmapOut[p].r = cluster[pixmapIn[p].label].r;
        pixmapOut[p].g = cluster[pixmapIn[p].label].g;
        pixmapOut[p].b = cluster[pixmapIn[p].label].b;
        pixmapOut[p].i = pixmapIn[p].i;
        pixmapOut[p].j = pixmapIn[p].j;
    }
}
int main(int argc, char **argv)
{
    FILE *ifp, *ofp;
    pixel *pixmap, *pixmapout;
    int ich1, ich2, rows, cols, maxval;
    char file_name[100];
    // args is : file for the moment

    if (argc != 3)
    {
        printf("\n %s  Usage: input_file K ", argv[0]);
        exit(0);
    }

    // opening input file
    ifp = fopen(argv[1], "r");
    if (ifp == NULL)
    {
        printf("error in opening file %s\n", argv[1]);
        exit(1);
    }
    // reading magic number
    ich1 = getc(ifp);
    if (ich1 == EOF)
    {
        pm_erreur("EOF / read error / magic number");
    }
    ich2 = getc(ifp);
    if (ich2 == EOF)
    {
        pm_erreur("EOF /read error / magic number");
    }
    if (ich2 != '6')
    {
        pm_erreur(" wrong file type ");
    }

    // image dimensions
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    // printf(" cols is : %d and rows is : %d \n ", cols, rows);
    maxval = pm_getint(ifp);

    // memory allocation
    pixmap = (pixel *)malloc(3 * cols * rows * sizeof(pixel));
    pixmapout = (pixel *)malloc(3 * cols * rows * sizeof(pixel));

    // reading

    for (int p = 0; p < rows * cols; p++)
    {
        pixmap[p].r = pm_getrawbyte(ifp);
        pixmap[p].g = pm_getrawbyte(ifp);
        pixmap[p].b = pm_getrawbyte(ifp);
        pixmap[p].i = p / cols;
        pixmap[p].j = p % cols;
        // printf("%d %d %d %d \n", pixmap[p].i, pixmap[p].j, p, cols);
        //  pixmap[(i * cols + j)/3].r = pm_getrawbyte(ifp);
        //  pixmap[(i * cols + j)/3].g = pm_getrawbyte(ifp);
        //  pixmap[(i * cols + j)/3].b = pm_getrawbyte(ifp);
    }

    // implementing kmeans algorithm
    // kmeans(pixmap, MACRO_K , rows, cols, pixmapout);
    //  implementing kmeans algorithm

    int K = atoi(argv[2]);
    kmeans(pixmap, K, rows, cols, pixmapout);
    printf("kmeans done\n");

    // reading

    strcpy(file_name, "output.ppm"); /* to rename later */
                                     /* Opening first output file */
    ofp = fopen(file_name, "w");
    if (ofp == NULL)
    {
        printf("error in opening file %s\n", file_name);
        exit(1);
    }
    /*Writing in the first file */
    fprintf(ofp, "P6\n");
    fprintf(ofp, "%d %d \n", cols, rows);
    fprintf(ofp, "%d\n", maxval);

    for (int p = 0; p < rows * cols; p++)
    {
        fprintf(ofp, "%c", pixmapout[p].r);
        fprintf(ofp, "%c", pixmapout[p].g);
        fprintf(ofp, "%c", pixmapout[p].b);
    }
}