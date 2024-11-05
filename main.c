#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Util.h"
#include <math.h>
#define MACRO_K 4

typedef struct {
    unsigned char r, g, b;
    unsigned int label;
} pixel;

void kmeans(pixel *pixmapIn, int K, int rows, int cols, pixel *pixmapOut){
    //on calcule les dimensions de l'image
    //1024*768 = 786 432
    int imageDimension = rows * cols;
    printf("image dimension : %d\n", imageDimension);

    //fix cluster centers in the two dimensions image randomly
    pixel cluster[K];

    for(int k = 0; k < K; k++){

        //on choisit un pixel au hasard
        int p = rand() % imageDimension; /* generating random coords to initialize the center ? */

        //pour retrouver les indices des composantes correspondant au pixel choisi aléatoirement
        //3n - 2 = r, 3n - 1 = g, 3n = b

        cluster[k].r = pixmapIn[p].r;
        cluster[k].g = pixmapIn[p].g;
        cluster[k].b = pixmapIn[p].b;
        cluster[k].label = k;
    }
    //allocate each pixel of the image to the nearest cluster center

    int associatedK = 0;

    for(int p = 0; p < imageDimension; p++){
        
        int min = 255 * 3; //max distance between two colors
        
        for(int k = 0; k < K; k++){
            
            int colorDistance =sqrt( pow(pixmapIn[p/3].r - cluster[k].r, 2) + pow(pixmapIn[p/3].g - cluster[k].g, 2) + pow(pixmapIn[p/3].b - cluster[k].b, 2));
                
                if(colorDistance < min){
                    min = colorDistance;
                    associatedK = k;
                }
            }
            //assign the pixel to the cluster with the minimum distance
            pixmapIn[p/3].label = associatedK;
    }

    //update the centroids of the clusters

    for(int k = 0; k < K; k++){

        int sum_r = 0;
        int sum_g = 0;
        int sum_b = 0;
        int count = 0;

        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                if(pixmapIn[(i * cols + j)/3].label == k){
                    sum_r += pixmapIn[(i * cols + j)/3].r;
                    sum_g += pixmapIn[(i * cols + j)/3].g;
                    sum_b += pixmapIn[(i * cols + j)/3].b;
                    count++;
                }
            }
        }

        cluster[k].r = sum_r / count;
        cluster[k].g = sum_g / count;
        cluster[k].b = sum_b / count;
    }
    for(int k = 0; k < K; k++){

        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                if(pixmapIn[(i * cols + j)].label == k){
                    pixmapOut[(i * cols + j)/3].r = cluster[k].r;
                    pixmapOut[(i * cols + j)/3].g = cluster[k].g;
                    pixmapOut[(i * cols + j)/3].b = cluster[k].b;
                }
            }
        }
    }
}



int main(int argc, char ** argv) {
    FILE *ifp, *ofp;
    pixel* pixmap, *pixmapout;
    int ich1, ich2, rows, cols, maxval;
    int *label,i,j;
    char file_name[100];
    // args is : file for the moment

    if (argc != 2)
    {
        printf("\n %s  Usage: input_file ( + K ?)  ", argv[0]);
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
    printf(" ich2 is : %d \n ", ich2);
    if (ich2 != '6')
    {
        pm_erreur(" wrong file type ");
    }

    // image dimensions
    cols = pm_getint(ifp);
    rows = pm_getint(ifp);
    printf(" cols is : %d and rows is : %d \n ",cols, rows);
    maxval = pm_getint(ifp);

    //memory allocation
    pixmap = (pixel *) malloc( cols * rows * sizeof(pixel));
    pixmapout =  (pixel *) malloc( cols * rows * sizeof(pixel));
    label = (int *)malloc(cols * rows * sizeof(int));

    //reading

    for( i=0; i < rows; i++){
     for( j=0; j < cols ; j++){
       pixmap[(i * cols + j)/3].r = pm_getrawbyte(ifp);
       pixmap[(i * cols + j)/3].g = pm_getrawbyte(ifp);
       pixmap[(i * cols + j)/3].b = pm_getrawbyte(ifp);
     }
   }

    //implementing kmeans algorithm
     kmeans(pixmap, MACRO_K , rows, cols, pixmapout);

    //reading


    strcpy(file_name,"output.ppm");/* to rename later */
     /* Opening first output file */
    ofp = fopen(file_name,"w");
    if (ofp == NULL) {
      printf("error in opening file %s\n", file_name);
      exit(1);
    }
    /*Writing in the first file */
    fprintf(ofp, "P5\n");
    fprintf(ofp, "%d %d \n", cols, rows);
    fprintf(ofp, "%d\n", maxval);

    for(i=0; i < rows; i++)
      for(j=0; j < cols ; j++){
          fprintf(ofp, "%c",pixmap[(i * cols + j)].r);
          fprintf(ofp, "%c",pixmap[(i * cols + j)].g);
          fprintf(ofp, "%c",pixmap[(i * cols + j)].b);
          /* no need to print label for the moment ? */
       }

}