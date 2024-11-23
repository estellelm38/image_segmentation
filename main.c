#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Util.h"
#include <math.h>
#include <time.h>
#include <stdbool.h>
int random_clusters = false;
void distribute_centroids(pixel *pixmapIN, int rows, int cols, int K, pixel *cluster_centers) {
    int bins = 16; // Diviser l'espace de couleurs en une grille 16x16x16
    int histo [16][16][16];
    int step = 256 / bins; // Taille de chaque intervalle

    // Construire un histome
    for (int p = 0; p < rows * cols; p++) {
        int r_bin = pixmapIN[p].r / step;
        int g_bin = pixmapIN[p].g / step;
        int b_bin = pixmapIN[p].b / step;
        histo[r_bin][g_bin][b_bin]++;
    }

    // Trouver les bins les plus denses
    for (int k = 0; k < K; k++) {
        int max_count = 0, max_r = 0, max_g = 0, max_b = 0;

        for (int r = 0; r < bins; r++) {
            for (int g = 0; g < bins; g++) {
                for (int b = 0; b < bins; b++) {
                    if (histo[r][g][b] > max_count) {
                        max_count = histo[r][g][b];
                        max_r = r;
                        max_g = g;
                        max_b = b;
                    }
                }
            }
        }

        // Définir le centroïde
        cluster_centers[k].r = max_r * step + step / 2;
        cluster_centers[k].g = max_g * step + step / 2;
        cluster_centers[k].b = max_b * step + step / 2;
        int min_dist = 255*3;
        for (int p = 0; p < rows*cols; p++)
        {   
            float colorDistance = sqrt(pow((float)(pixmapIN[p].r - cluster_centers[k].r)/255, 2) + pow((float)(pixmapIN[p].g - cluster_centers[k].g)/255, 2) + pow((float)(pixmapIN[p].b - cluster_centers[k].b)/255, 2));
            if(colorDistance < min_dist){
                cluster_centers[k].i=pixmapIN[p].i;
                cluster_centers[k].j=pixmapIN[p].j;
                /* this is just a color check in the image in order to get the pixel that is the nearest possible to that cluster*/
            }
        }
        
        // Éviter de réutiliser la même région
        histo[max_r][max_g][max_b] = 0;
    }
}
void initialize_random_centroids(pixel *pixmapIn, int imageDimension, int K, pixel *cluster) {
    for (int k = 0; k < K; k++) {
        // Générer un pixel aléatoire comme centroïde initial
        int p = rand() % imageDimension;

        // Assigner les valeurs RGB et les coordonnées spatiales
        cluster[k].r = pixmapIn[p].r;
        cluster[k].g = pixmapIn[p].g;
        cluster[k].b = pixmapIn[p].b;
        cluster[k].i = pixmapIn[p].i;
        cluster[k].j = pixmapIn[p].j;

        // Étiqueter le cluster avec son index
        cluster[k].label = k;
    }
}

void kmeans(pixel *pixmapIn, int K, int rows, int cols, pixel *pixmapOut,int seed)
{

    //int seed = time(NULL); 
    // int seed = 1731921458;
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
    if(random_clusters)
    {
        initialize_random_centroids(pixmapIn, imageDimension, K, cluster);
    }
    else
    {
        distribute_centroids(pixmapIn,rows,cols,K,cluster);
    }
  
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

                float colorDistance = sqrt(pow((float)(pixmapIn[p].r - cluster[k].r)/255, 2) + pow((float)(pixmapIn[p].g - cluster[k].g)/255, 2) + pow((float)(pixmapIn[p].b - cluster[k].b)/255, 2));
                // printf("color distance : %d\n", colorDistance);
                float realDistance = sqrt(pow((float)(pixmapIn[p].i - cluster[k].i)/rows, 2) + pow((float)(pixmapIn[p].j - cluster[k].j)/cols, 2));
                // printf("real distance : %d\n", realDistance);
                // j'ai un doute sur le calcul de combination, sans parler des poids
                float combination = color_weight * colorDistance + distance_weight * realDistance;
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

    if (argc != 4 && argc != 5)
    {
        printf("\n %s  Usage: input_file K random_clusters?[ != 0 for random], seed", argv[0]);
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
    int seed =0;
    int K = atoi(argv[2]);
    if(atoi(argv[3])!=0 && argc == 5){
        random_clusters = true;
        seed =  atoi(argv[4]);
        printf("random_activated \n");
        printf("seed : %d\n", seed);

    }else if ( atoi(argv[3])!=0){
        printf("ERROR PLEASE INTRODUCE THE SEED AS THE FIFTH PARAMETER WHEN ASKING FOR RANDOMNESS \n");
        exit(EXIT_FAILURE);
    }else{
        random_clusters=false;
        printf("METHOD USED : DISTRIBUTION OF CLUSTERS OVER COLORS \n ");
    }
    printf("Starting segmentation of the image \n");
    kmeans(pixmap, K, rows, cols, pixmapout,seed);
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