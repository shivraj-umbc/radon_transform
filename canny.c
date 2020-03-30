#include <stdio.h>
#include <stdlib.h>
#include "dc_image.h"


#define CANNY_THRESH 45
#define CANNY_BLUR   10

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )

int main()
{
	int y,x;
	int rows, cols, chan;

	//-----------------
	// Read the image    [y][x][c]   y number rows   x cols  c 3
	//-----------------
	byte ***img = LoadRgb(".\\puppy.jpg", &rows, &cols, &chan);
	printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);

	
	SaveRgbPng(img, "out/1_img.png", rows, cols);
	
	//-----------------
	// Convert to Grayscale
	//-----------------
	byte **gray = malloc2d(rows, cols);
	for (y=0; y<rows; y++){
		for (x=0; x<cols; x++) {
			int r = img[y][x][0];
			int g = img[y][x][1];
			int b = img[y][x][2];
			gray[y][x] =  (r+g+b) / 3;
		}
	}
	
	SaveGrayPng(gray, "out/2_gray.png", rows, cols);

	//-----------------
	// Box Blur   ToDo: Gaussian Blur is better
	//-----------------
	
	// Box blur is separable, so separately blur x and y
	int k_x=CANNY_BLUR, k_y=CANNY_BLUR;
	
	// blur in the x dimension
	byte **blurx = (byte**)malloc2d(rows, cols);
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			
			// Start and end to blur
			int minx = x-k_x/2;      // k_x/2 left of pixel
			int maxx = minx + k_x;   // k_x/2 right of pixel
			minx = MAX(minx, 0);     // keep in bounds
			maxx = MIN(maxx, cols);
			
			// average blur it
			int x2;
			int total = 0;
			int count = 0;
			for (x2=minx; x2<maxx; x2++) {
				total += gray[y][x2];    // use "gray" as input
				count++;
			}
			blurx[y][x] = total / count; // blurx is output
		}
	}
	
	SaveGrayPng(blurx, "out/3_blur_just_x.png", rows, cols);
	
	// blur in the y dimension
	byte **blur = (byte**)malloc2d(rows, cols);
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			
			// Start and end to blur
			int miny = y-k_y/2;      // k_x/2 left of pixel
			int maxy = miny + k_y;   // k_x/2 right of pixel
			miny = MAX(miny, 0);     // keep in bounds
			maxy = MIN(maxy, rows);
			
			// average blur it
			int y2;
			int total = 0;
			int count = 0;
			for (y2=miny; y2<maxy; y2++) {
				total += blurx[y2][x];    // use blurx as input
				count++;
			}
			blur[y][x] = total / count;   // blur is output
		}
	}
	
	SaveGrayPng(blur, "out/3_blur.png", rows, cols);
	
	
	//-----------------
	// Take the "Sobel" (magnitude of derivative)
	//  (Actually we'll make up something similar)
	//-----------------
	
	byte **sobel = (byte**)malloc2d(rows, cols);
	
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			int mag=0;
			
			if (y>0)      mag += ABS(blur[y-1][x] - blur[y][x]);
			if (x>0)      mag += ABS(blur[y][x-1] - blur[y][x]);
			if (y<rows-1) mag += ABS(blur[y+1][x] - blur[y][x]);
			if (x<cols-1) mag += ABS(blur[y][x+1] - blur[y][x]);
			
			sobel[y][x] = 3*mag;
		}
	}
	
	SaveGrayPng(sobel, "out/4_sobel.png", rows, cols);
	
	//-----------------
	// Non-max suppression
	//-----------------
	byte **nonmax = malloc2d(rows, cols);    // note: *this* initializes to zero!
	
	for (y=1; y<rows-1; y++)
	{
		for (x=1; x<cols-1; x++)
		{
			// Is it a local maximum
			int is_y_max = (sobel[y][x] > sobel[y-1][x] && sobel[y][x]>=sobel[y+1][x]);
			int is_x_max = (sobel[y][x] > sobel[y][x-1] && sobel[y][x]>=sobel[y][x+1]);
			if (is_y_max || is_x_max)
				nonmax[y][x] = 255;
			else
				nonmax[y][x] = 0;
		}
	}
	
	SaveGrayPng(nonmax, "out/5_nonmax.png", rows, cols);
	
	//-----------------
	// Final Threshold
	//-----------------
	byte **edges = malloc2d(rows, cols);    // note: *this* initializes to zero!
	
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			if (nonmax[y][x] > CANNY_THRESH)
				edges[y][x] = 255;
			else
				edges[y][x] = 0;
		}
	}
	
	SaveGrayPng(edges, "out/6_edges.png", rows, cols);
	
	printf("Done!\n");

	return 0;
}

/*

	printf("load image\n");
	byte *data = stbi_load("puppy.jpg", &cols, &rows, &chan, 4);

	printf("data = %p\n", data);
	int rt=stbi_write_png("output.png", cols, rows, 4, data, cols*4);
*/