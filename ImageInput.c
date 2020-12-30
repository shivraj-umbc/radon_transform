#include <stdio.h>
#include <stdlib.h>
#include "dc_image.h"
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )

int main()
{
	int y,x;
	int rows, cols, chan;

    byte ***img = LoadRgb(".\\puppy.bmp", &rows, &cols, &chan);
    printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);

	byte **gray = malloc2d(rows, cols);
	for (y=0; y<rows; y++){
		for (x=0; x<cols; x++) {
			int r = img[y][x][0];
			int g = img[y][x][1];
			int b = img[y][x][2];
			gray[y][x] =  (r+g+b) / 3;
		}
	}

    SaveGrayPng(gray, "output.png", rows, cols);
	
}