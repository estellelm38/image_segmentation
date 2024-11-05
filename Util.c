#include <stdio.h>
#include <stdlib.h>
#include "Util.h"

/* Some useful functions to read, write PBM, PGM, PPM files
**
**
** E.B.
*/
char pm_getc(FILE *file)
{
    int ich;
    char ch;

    ich = getc(file);
    if (ich == EOF)
        pm_erreur("EOF / read error");
    ch = (char)ich;

    if (ch == '#')
    {
        do
        {
            ich = getc(file);
            if (ich == EOF)
                pm_erreur("EOF / read error");
            ch = (char)ich;
        } while (ch != '\n' && ch != '\r');
    }

    return ch;
}

bit pm_getbit(FILE *file)
{
    char ch;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch != '0' && ch != '1')
        pm_erreur("read error : != [0|1]");

    return (ch == '1') ? 1 : 0;
}

unsigned char pm_getrawbyte(FILE *file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        pm_erreur("EOF / read error ");
    return (unsigned char)iby;
}

int pm_getint(FILE *file)
{
    char ch;
    int i;

    do
    {
        ch = pm_getc(file);
    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');

    if (ch < '0' || ch > '9')
        pm_erreur("read error : != [0-9]");

    i = 0;
    do
    {
        i = i * 10 + ch - '0';
        ch = pm_getc(file);
    } while (ch >= '0' && ch <= '9');

    return i;
}

void pm_erreur(char *texte)
{
    fprintf(stderr, "\n%s \n\n", texte);
    exit(1);
}

/*Some useful functions for local filtering*/

void binomialFilter3x3(gray *graymapIn, int rows, int cols, gray *graymapOut)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
            {
                graymapOut[i * cols + j] = graymapIn[i * cols + j];
            }
            else
            {
                graymapOut[i * cols + j] = (4 * graymapIn[i * cols + j] +
                                            2 * graymapIn[(i - 1) * cols + j] + 2 * graymapIn[i * cols + (j + 1)] + 2 * graymapIn[(i + 1) * cols + j] + 2 * graymapIn[i * cols + (j - 1)] +
                                            graymapIn[(i - 1) * cols + (j - 1)] + graymapIn[(i - 1) * cols + (j + 1)] + graymapIn[(i + 1) * cols + (j + 1)] + graymapIn[(i + 1) * cols + (j - 1)]) /
                                           16;
            }
        }
    }
}

void binomialFilter5x5(gray *graymapIn, int rows, int cols, gray *graymapOut)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i < 2 || j < 2 || i >= rows - 2 || j >= cols - 2)
            {
                graymapOut[i * cols + j] = graymapIn[i * cols + j];
            }
            else
            {
                graymapOut[i * cols + j] =
                    (36 * graymapIn[i * cols + j] +
                     24 * graymapIn[(i - 1) * cols + j] + 24 * graymapIn[i * cols + (j + 1)] + 24 * graymapIn[(i + 1) * cols + j] + 24 * graymapIn[i * cols + (j - 1)] +
                     16 * graymapIn[(i - 1) * cols + (j - 1)] + 16 * graymapIn[(i - 1) * cols + (j + 1)] + 16 * graymapIn[(i + 1) * cols + (j + 1)] + 16 * graymapIn[(i + 1) * cols + (j - 1)] +
                     6 * graymapIn[(i - 2) * cols + j] + 6 * graymapIn[i * cols + (j + 2)] + 6 * graymapIn[(i + 2) * cols + j] + 6 * graymapIn[i * cols + (j - 2)] +
                     4 * graymapIn[(i - 2) * cols + (j + 1)] + 4 * graymapIn[(i - 1) * cols + (j + 2)] + 4 * graymapIn[(i + 1) * cols + (j + 2)] + 4 * graymapIn[(i + 2) * cols + (j + 1)] + 4 * graymapIn[(i + 2) * cols + (j - 1)] + 4 * graymapIn[(i + 1) * cols + (j - 2)] + 4 * graymapIn[(i - 1) * cols + (j - 2)] + 4 * graymapIn[(i - 2) * cols + (j - 1)]) /
                    256;
            }
        }
    }
}

int compare(const void *a, const void *b)
{
    return (*(gray *)a - *(gray *)b);
}

void medianFilter3x3(gray *graymapIn, int rows, int cols, gray *graymapOut)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
            {
                graymapOut[i * cols + j] = graymapIn[i * cols + j];
            }
            else
            {
                gray arr[] = {graymapIn[i * cols + j],
                              graymapIn[(i - 1) * cols + (j - 1)],
                              graymapIn[(i - 1) * cols + (j)],
                              graymapIn[(i - 1) * cols + (j + 1)],
                              graymapIn[(i)*cols + (j - 1)],
                              graymapIn[i * cols + j],
                              graymapIn[i * cols + (j + 1)],
                              graymapIn[(i + 1) * cols + (j - 1)],
                              graymapIn[(i + 1) * cols + j],
                              graymapIn[(i + 1) * cols + (j + 1)]};

                int n = 9;

                qsort(arr, n, sizeof(gray), compare);
                graymapOut[i * cols + j] = arr[4];
            }
        }
    }
}

void medianFilter5x5(gray *graymapIn, int rows, int cols, gray *graymapOut)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i < 2 || j < 2 || i >= rows - 2 || j >= cols - 2)
            {
                graymapOut[i * cols + j] = graymapIn[i * cols + j];
            }
            else
            {
                gray arr[] = {
                    graymapIn[(i - 2) * cols + (j - 2)], graymapIn[(i - 2) * cols + (j - 1)], graymapIn[(i - 2) * cols + (j)], graymapIn[(i - 2) * cols + (j + 1)], graymapIn[(i - 2) * cols + (j + 2)],
                    graymapIn[(i - 1) * cols + (j - 2)], graymapIn[(i - 1) * cols + (j - 1)], graymapIn[(i - 1) * cols + (j)], graymapIn[(i - 1) * cols + (j + 1)], graymapIn[(i - 1) * cols + (j + 2)],
                    graymapIn[i * cols + (j - 2)], graymapIn[i * cols + (j - 1)], graymapIn[i * cols + j], graymapIn[i * cols + (j + 1)], graymapIn[i * cols + (j + 2)],
                    graymapIn[(i + 1) * cols + (j - 2)], graymapIn[(i + 1) * cols + (j - 1)], graymapIn[(i + 1) * cols + (j)], graymapIn[(i + 1) * cols + (j + 1)], graymapIn[(i + 1) * cols + (j + 2)],
                    graymapIn[(i + 2) * cols + (j - 2)], graymapIn[(i + 2) * cols + (j - 1)], graymapIn[(i + 2) * cols + (j)], graymapIn[(i + 2) * cols + (j + 1)], graymapIn[(i + 2) * cols + (j + 2)]};

                int n = 25;

                qsort(arr, n, sizeof(gray), compare);
                graymapOut[i * cols + j] = arr[12];
            }
        }
    }
}

/*Some useful functions to compute histograms and to stretch and equalize them*/

void computeHistogram(gray *graymap, int rows, int cols, int *histogram)
{

    for (int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int pixel_value = graymap[i * cols + j];
            histogram[pixel_value]++;
        }
    }

    FILE *ofcsv = fopen("histogramme.csv", "w");

    if (ofcsv == NULL)
    {
        printf("error in opening file\n");
        exit(1);
    }

    for (int i = 0; i < 256; i++)
    {
        fprintf(ofcsv, "%d\n", histogram[i]);
    }

    fclose(ofcsv);
}

void stretch(gray *graymapIn, int rows, int cols, gray *graymapOut)
{

    gray min = 255;
    gray max = 0;
    int i;
    int j;
    // find min and max values
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            gray pixel_value = graymapIn[i * cols + j];
            if (pixel_value < min)
            {
                min = pixel_value;
            }
            if (pixel_value > max)
            {
                max = pixel_value;
            }
        }
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (max != min)
            {
                graymapOut[i * cols + j] = (255 * (graymapIn[i * cols + j] - min)) / (max - min);
            }
            else
            {
                graymapOut[i * cols + j] = 255;
            }
        }
    }
}

void equalizeHistogram(gray *graymapIn, int rows, int cols, gray *graymapOut)
{
    int histogram[256];
    int cum_histogram[256] = {0};
    int dimensions = rows * cols;

    computeHistogram(graymapIn, rows, cols, histogram);
    cum_histogram[0] += histogram[0];
    for (int i = 1; i < 256; i++)
    {
        cum_histogram[i] += cum_histogram[i - 1] + histogram[i];
    }
    int transformation[256];
    for (int i = 0; i < 256; i++)
    {
        transformation[i] = (cum_histogram[i] * 255) / dimensions;
    }

    for (int i = 0; i < dimensions; i++)
    {
        graymapOut[i] = transformation[graymapIn[i]];
    }

    FILE *ofcsv = fopen("cum_histogram.csv", "w");

    if (ofcsv == NULL)
    {
        printf("error in opening file\n");
        exit(1);
    }

    for (int i = 0; i < 256; i++)
    {
        fprintf(ofcsv, "%d\n", cum_histogram[i]);
    }

    fclose(ofcsv);
}
