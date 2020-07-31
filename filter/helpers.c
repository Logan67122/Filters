#include "helpers.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            double avg = (image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0;
            image[i][j].rgbtRed = round(avg);
            image[i][j].rgbtBlue = round(avg);
            image[i][j].rgbtGreen = round(avg);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            BYTE tmp1 = image[i][width - j - 1].rgbtRed;
            BYTE tmp2 = image[i][width - j - 1].rgbtBlue;
            BYTE tmp3 = image[i][width - j - 1].rgbtGreen;
            image[i][width - j - 1].rgbtRed = image[i][j].rgbtRed;
            image[i][width - j - 1].rgbtBlue = image[i][j].rgbtBlue;
            image[i][width - j - 1].rgbtGreen = image[i][j].rgbtGreen ;
            image[i][j].rgbtRed = tmp1;
            image[i][j].rgbtBlue = tmp2;
            image[i][j].rgbtGreen = tmp3;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE(*image1)[width + 2] = calloc(height + 2, (width + 2) * sizeof(RGBTRIPLE));
    for (int i = 0; i < height + 2; i++)
    {
        for (int j = 0; j < width + 2; j++)
        {
            image1[i][j].rgbtRed = 0;
            image1[i][j].rgbtBlue = 0;
            image1[i][j].rgbtGreen = 0;
        }
    }
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {
            image1[i][j].rgbtRed = image[i - 1][j - 1].rgbtRed;
            image1[i][j].rgbtBlue = image[i - 1][j - 1].rgbtBlue;
            image1[i][j].rgbtGreen = image[i - 1][j - 1].rgbtGreen;
        }
    }
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {
            int s1 = 0, s2 = 0, s3 = 0;
            for (int k = i - 1; k < i + 2; k++)
            {
                for (int l = j - 1; l < j + 2; l++)
                {
                    s1 += image1[k][l].rgbtRed;
                    s2 += image1[k][l].rgbtBlue;
                    s3 += image1[k][l].rgbtGreen;
                }
            }
            int c = 9;
            if ((i == 1 || i == height) && (j == 1 || j == width))
            {
                c = 4;
            }
            else if (i == 1 || i == height || j == 1 || j == width)
            {
                c = 6;
            }
            image[i - 1][j - 1].rgbtRed = round(s1 / (double)c);
            image[i - 1][j - 1].rgbtBlue = round(s2 / (double)c);
            image[i - 1][j - 1].rgbtGreen = round(s3 / (double)c);
        }
    }
    free(image1);
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    RGBTRIPLE(*image1)[width + 2] = calloc(height + 2, (width + 2) * sizeof(RGBTRIPLE));
    
    for (int i = 0; i < height + 2; i++)
    {
        for (int j = 0; j < width + 2; j++)
        {
            image1[i][j].rgbtRed = 0;
            image1[i][j].rgbtBlue = 0;
            image1[i][j].rgbtGreen = 0;
        }
    }
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {
            image1[i][j].rgbtRed = image[i - 1][j - 1].rgbtRed;
            image1[i][j].rgbtBlue = image[i - 1][j - 1].rgbtBlue;
            image1[i][j].rgbtGreen = image[i - 1][j - 1].rgbtGreen;
        }
    }
    for (int i = 1; i < height + 1; i++)
    {
        for (int j = 1; j < width + 1; j++)
        {
            int sx1 = 0, sx2 = 0, sx3 = 0;
            int sy1 = 0, sy2 = 0, sy3 = 0;
            for (int k = i - 1; k < i + 2; k++)
            {
                for (int l = j - 1; l < j + 2; l++)
                {
                    sx1 += image1[k][l].rgbtRed * Gx[k - i + 1][l - j + 1];
                    sx2 += image1[k][l].rgbtBlue * Gx[k - i + 1][l - j + 1];
                    sx3 += image1[k][l].rgbtGreen * Gx[k - i + 1][l - j + 1];
                    sy1 += image1[k][l].rgbtRed * Gy[k - i + 1][l - j + 1];
                    sy2 += image1[k][l].rgbtBlue * Gy[k - i + 1][l - j + 1];
                    sy3 += image1[k][l].rgbtGreen * Gy[k - i + 1][l - j + 1];
                }
            }
            int tmp1, tmp2, tmp3;
            tmp1 = round(sqrt(sx1 * sx1 + sy1 * sy1));
            tmp2 = round(sqrt(sx2 * sx2 + sy2 * sy2));
            tmp3 = round(sqrt(sx3 * sx3 + sy3 * sy3));
            if (tmp1 > 255)
            {
                tmp1 = 255;
            }
            if (tmp2 > 255)
            {
                tmp2 = 255;
            }
            if (tmp3 > 255)
            {
                tmp3 = 255;
            }
            image[i - 1][j - 1].rgbtRed = tmp1;
            image[i - 1][j - 1].rgbtBlue = tmp2;
            image[i - 1][j - 1].rgbtGreen = tmp3;
        }
    }
    free(image1);
    return;
}
