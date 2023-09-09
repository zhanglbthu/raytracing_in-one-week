#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "vec3.h"
/****************************************************************************
    bmp.c - read and write bmp images.
    Distributed with Xplanet.
    Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/
struct BMPHeader
{
    char bfType[3];      /* "BM" */
    int bfSize;          /* Size of file in bytes */
    int bfReserved;      /* set to 0 */
    int bfOffBits;       /* Byte offset to actual bitmap data (= 54) */
    int biSize;          /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    int biWidth;         /* Width of image, in pixels */
    int biHeight;        /* Height of images, in pixels */
    short biPlanes;      /* Number of planes in target device (set to 1) */
    short biBitCount;    /* Bits per pixel (24 in this case) */
    int biCompression;   /* Type of compression (0 if no compression) */
    int biSizeImage;     /* Image size, in bytes (0 if no compression) */
    int biXPelsPerMeter; /* Resolution in pixels/meter of display device */
    int biYPelsPerMeter; /* Resolution in pixels/meter of display device */
    int biClrUsed;       /* Number of colors in the color table (if 0, use
                            maximum allowed by biBitCount) */
    int biClrImportant;  /* Number of important colors.  If 0, all colors
                            are important */
};
class image
{
public:
    image(int w, int h)
    {
        width = w;
        height = h;
        data = new vec3[width * height];
    }
    ~image() { delete[] data; }
    int get_width() const { return width; }
    int get_height() const { return height; }
    const vec3 &get_pixel(int x, int y) const
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }
    void set_all_pixels(const vec3 &color)
    {
        for (int i = 0; i < width * height; ++i)
            data[i] = color;
    }
    void set_pixel(int x, int y, const vec3 &color)
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] = color;
    }
    void add_pixel(int x, int y, const vec3 &color)
    {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] += color;
    }
    int save_bmp(const char *filename)
    {
        int i, j, ipos;
        int bytesPerLine;
        unsigned char *line;
        vec3 *rgb = data;
        FILE *file;
        struct BMPHeader bmph;

        /* The length of each line must be a multiple of 4 bytes */

        bytesPerLine = (3 * (width + 1) / 4) * 4;

        strcpy(bmph.bfType, "BM");
        bmph.bfOffBits = 54;
        bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
        bmph.bfReserved = 0;
        bmph.biSize = 40;
        bmph.biWidth = width;
        bmph.biHeight = height;
        bmph.biPlanes = 1;
        bmph.biBitCount = 24;
        bmph.biCompression = 0;
        bmph.biSizeImage = bytesPerLine * height;
        bmph.biXPelsPerMeter = 0;
        bmph.biYPelsPerMeter = 0;
        bmph.biClrUsed = 0;
        bmph.biClrImportant = 0;

        file = fopen(filename, "wb");
        if (file == NULL)
            return (0);

        fwrite(&bmph.bfType, 2, 1, file);
        fwrite(&bmph.bfSize, 4, 1, file);
        fwrite(&bmph.bfReserved, 4, 1, file);
        fwrite(&bmph.bfOffBits, 4, 1, file);
        fwrite(&bmph.biSize, 4, 1, file);
        fwrite(&bmph.biWidth, 4, 1, file);
        fwrite(&bmph.biHeight, 4, 1, file);
        fwrite(&bmph.biPlanes, 2, 1, file);
        fwrite(&bmph.biBitCount, 2, 1, file);
        fwrite(&bmph.biCompression, 4, 1, file);
        fwrite(&bmph.biSizeImage, 4, 1, file);
        fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
        fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
        fwrite(&bmph.biClrUsed, 4, 1, file);
        fwrite(&bmph.biClrImportant, 4, 1, file);

        line = (unsigned char *)malloc(bytesPerLine);
        if (line == NULL)
        {
            fprintf(stderr, "Can't allocate memory for BMP file.\n");
            return (0);
        }

        for (i = 0; i < height; i++)
        {
            for (j = 0; j < width; j++)
            {
                ipos = (width * i + j);
                line[3 * j] = ClampColorComponent(rgb[ipos][2]);
                line[3 * j + 1] = ClampColorComponent(rgb[ipos][1]);
                line[3 * j + 2] = ClampColorComponent(rgb[ipos][0]);
            }
            fwrite(line, bytesPerLine, 1, file);
        }

        free(line);
        fclose(file);

        return (1);
    }

private:
    int width;
    int height;
    vec3 *data;

    unsigned char ClampColorComponent(double comp)
    {
        int intComp = int(256 * comp);
        if (intComp < 0)
            return 0;
        if (intComp > 255)
            return 255;
        return (unsigned char)intComp;
    }
};

#endif
