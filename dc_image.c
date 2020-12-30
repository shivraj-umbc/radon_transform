#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include "stb_image.h"
#include "stb_image_write.h"

#include "dc_image.h"

#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )
//-------------------------------------------------------
// A few image helper functions
//-------------------------------------------------------

// Memory Allocation

byte **malloc2d(int rows, int cols) {
	int y;
	byte **ptr = (byte**)malloc(rows*sizeof(byte*));
	for (y=0; y<rows; y++)
		ptr[y] = (byte*)calloc(cols,sizeof(byte));
	return ptr;
}

byte ***malloc3d(int rows, int cols, int chan) {
	int y,x;
	byte ***ptr = (byte***)malloc(rows*sizeof(byte**));
	for (y=0; y<rows; y++) {
		ptr[y] = (byte**)malloc(cols*sizeof(byte*));
		for (x=0; x<cols; x++)
			ptr[y][x] = (byte*)calloc(chan,sizeof(byte));
	}
	return ptr;
}

void free2d(byte **data, int rows)
{
	int y;
	for (y=0; y<rows; y++)
		free(data[y]);
	free(data);
}

void free3d(byte ***data, int rows, int cols)
{
	int y,x;
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++)
			free(data[y][x]);
		free(data[y]);
	}
	free(data);
}


float **malloc2d_float(int rows, int cols)
{
	int y;
	float **ptr = (float**)malloc(rows*sizeof(float*));
	for (y=0; y<rows; y++)
		ptr[y] = (float*)calloc(cols,sizeof(float));
	return ptr;
}

float **free2d_float(float ***data, int rows)
{
	int y;
	for (y=0; y<rows; y++)
		free(data[y]);
	free(data);
}




// Loading / Saving Images

void SaveRgbPng(byte ***in, const char *fname, int rows, int cols)
{
	printf("SaveRgbaPng %s %d %d\n", fname, rows, cols);
	
	int y,x,c,i=0;
	byte *data = (byte*)malloc(cols*rows*3*sizeof(byte));
	for (y=0; y<rows; y++) {
		//printf("y %d rows %d cols %d\n", y, rows, cols);
		for (x=0; x<cols; x++) {
			for (c=0; c<3; c++) {
				data[i++] = in[rows-y-1][x][c];
			}
		}
	}
	stbi_write_png(fname, cols, rows, 3, data, cols*3);
	free(data);
}

void SaveGrayPng(byte **in, const char *fname, int rows, int cols)
{
	printf("SaveGrayPng %s %d %d\n", fname, rows, cols);
	
	int y,x,c,i=0;
	byte *data = malloc(cols*rows*3*sizeof(byte));
	for (y=0; y<rows; y++) {
		for (x=0; x<cols; x++) {
			data[i++] = in[rows-y-1][x];   // red
			data[i++] = in[rows-y-1][x];   // green
			data[i++] = in[rows-y-1][x];   // blue
//			data[i++] = 255;        // alpha
		}
	}
	stbi_write_png(fname, cols, rows, 3, data, cols*3);
	free(data);
}

void DrawLine(int ax, int ay, int bx, int by, byte*** img, int rows, int cols)
{
	if(ABS(bx-ax) <= 1 && ABS(by-ay) <= 1)
	{
		//byte *data = stbi_load(cols, rows, chan, 3);
		img[ay][ax][0] = 255;
		img[ay][ax][1] = 0;
		img[ay][ax][2] = 0;
		img[by][bx][0] = 255;
		img[by][bx][1] = 0;
		img[by][bx][2] = 0;	
	}
	else
	{
		int mid_x = (ax + bx)/2;
		int mid_y = (ay + by)/2;
		DrawLine(ax, ay, mid_x, mid_y, img, rows, cols);
		DrawLine(mid_x, mid_y, bx, by,img, rows, cols);
	}
}

byte ***LoadRgb(const char *fname, int *rows, int *cols, int *chan)
{
	printf("LoadRgba %s\n", fname);
	
	int y,x,c,i=0;	
	byte *data = stbi_load(fname, cols, rows, chan, 3);
	/*
	// Convert rgb to rgba
	if (*chan==3) {
		int N = *rows * *cols;
		printf("N %d\n", N);
		byte *rgb = data;
		data = malloc(N * 4 * sizeof(byte));
		for (i=0; i<N; i++) {
			//printf("i %d\n", i);
			data[4*i+0] = rgb[3*i+0];
			data[4*i+1] = rgb[3*i+1];
			data[4*i+2] = rgb[3*i+2];
			data[4*i+3] = 255;
		}
		free(rgb);
		printf("done convert\n");
	}
	*chan = 4;
	*/
	printf("channels: %d", *chan);
	if (*chan < 3) {
		printf("error: expected 3 channels (red green blue)\n");
		exit(1);
	}
	
	i=0;
	byte ***img = malloc3d(*rows,*cols,*chan);
	for (y=0; y<*rows; y++)
		for (x=0; x<*cols; x++)
			for (c=0; c<*chan; c++)
				img[*rows-y-1][x][c] = data[i++];
	free(data);
	printf("done read\n");
	return img;
}

