#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dc_image.h"
#include <math.h>
#define MAX(a,b)  ((a) > (b) ? (a) : (b))
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )

#define u32 unsigned int



typedef struct ContigSeg
{
	u32 first, last, sum;
} ContigSeg;

typedef struct LineSeg
{
	ContigSeg start;    // The starting segment
	ContigSeg longest;  // The longest segment
	ContigSeg end;      // The ending segment
} LineSeg;

typedef struct K_Radon
{
    LineSeg**** T;
    u32 N; // N = width =height  populate this with row/height of the image
    int L; //level
    int K; //skew
 //
    int S; // stack
    int F; //Offset
    int H; //height=2^L
}K_Radon;

 // Array of peaks
typedef struct XY {
        int x, y, npeaks;
    } XY;

K_Radon K_radon_transform(int N, byte** gray)
{
    _Bool flag_1 = 0;
    _Bool flag_2 = 0;
    int L=0, S=0, K=0, H=0, F=0;
    u32 nLev = 1;
	u32 subsize = 1;
	while (subsize < N) {
		subsize *= 2;
		nLev++;
	}

	//printf("nlev %d\n", nLev);

    LineSeg**** T;
    T = (LineSeg****)malloc(nLev* sizeof(LineSeg***));

    //Initialization of these variables before performing loops
    
    //Copy problems of size 1 Take a look at this
     T[0] = (LineSeg***)malloc(N * sizeof(LineSeg**));
     
    S =0, L = 0;
   

	for (int s=0; s<N; s++)
	{
        T[0][s] = (LineSeg**)malloc(1 * sizeof(LineSeg*));
        T[0][s][0] = (LineSeg*)malloc(N * sizeof(LineSeg));
        for(int f = 0; f < N; f++){
            if(gray[s][f] != 0){
                

                T[0][s][0][f].start.first = s;
			    T[0][s][0][f].start.last = s+1;
			    T[0][s][0][f].start.sum = 1;
			    T[0][s][0][f].longest.first  = s;
			    T[0][s][0][f].longest.last  = s+1;
			    T[0][s][0][f].longest.sum = 1;
			    T[0][s][0][f].end.first   = s;
			    T[0][s][0][f].end.last   = s+1;
			    T[0][s][0][f].end.sum = 1;
            }else{
                T[0][s][0][f].start.first = s;
			    T[0][s][0][f].start.last = s;
                T[0][s][0][f].start.sum = 0;
                T[0][s][0][f].longest.first  = s;
                T[0][s][0][f].longest.last  = s;
                T[0][s][0][f].longest.sum = 0;
                T[0][s][0][f].end.first   = s+1;
                T[0][s][0][f].end.last   = s+1;
                T[0][s][0][f].end.sum = 0;
            }
        }
    }

    ////////
    L = 1;
    S = N/2;
    H = 2;
    while(H<=N){
       T[L] = (LineSeg***)malloc(S * sizeof(LineSeg**));
       for(int s = 0; s < S ; s++){
           //printf("s = %d , L= %d \n", s,L);
           K = H;
           T[L][s] = (LineSeg**)malloc(K * sizeof(LineSeg*));
           for(int k = 0; k< K; k++){
               F = N+k;
                T[L][s][k] = (LineSeg*)malloc(F * sizeof(LineSeg));
                for(int f = 0; f <F; f++){
                     LineSeg C;
                     LineSeg A;
                     LineSeg B;
                    //calculate A and B and combine into C
                    if(f < (N + k/2))
                    {
                     A = T[L-1][2*s][k/2][f];
                     flag_1 = 1;
                    }
                    else
                    {
                        flag_1 = 0;
                    }
                    if(f-(k+1)/2 >= 0)
                    {
			        B = T[L-1][(2*s)+1][k/2][f-(k+1)/2];
                    flag_2 = 1;
                    }
                    else
                    {
                        flag_2 = 0;
                    }

                    if(flag_1 == 1 && flag_2 == 1)
                    {          
                        flag_1 =0, flag_2 =0;  
              
                        if(s ==0 && k == 0){
                    
                           // printf("A start.first %d, start.last %d, start.sum %d  \n", A.start.first, A.start.last,  A.start.sum);
                            //printf("B start.first %d, start.last %d, start.sum %d  \n", B.start.first, B.start.last,  B.start.sum);
                        }
                        
                    // Append the segments to a list of segments
                    //  xx xxxx xx|xxx xx xxxx
                    ContigSeg segs[6];   // appended segments
                    //int nSegs=6;
                    segs[0]=A.start;
                    segs[1]=A.longest;
                    segs[2]=A.end;
                    segs[3]=B.start;
                    segs[4]=B.longest;
                    segs[5]=B.end;

                   // printf("==== segs ====\n");
                    // for (int i=0; i<nSegs; i++)
                    //     printf("segs[%d] idx1 %d idx2 %d sum %d\n",
                    //         i, segs[i].first, segs[i].last, segs[i].sum);
                    
                    //Remove zero length segments
                    ContigSeg noZerosegs[6];
                    int numnonzerosegs = 0;
                    for(int i=0; i<6; i++)
                    {
                            if(segs[i].first != segs[i].last)
                            {
                                noZerosegs[numnonzerosegs++] = segs[i];
                            }
                    }
                    // printf("%d\n", numnonzerosegs);
                    // printf("====non zero segs ====\n");
                    // for (int i=0; i<numnonzerosegs; i++)
                    //     printf("noZerosegs[%d] first  %d last %d sum %d\n",
                    //         i, noZerosegs[i].first, noZerosegs[i].last, noZerosegs[i].sum);

                    if(numnonzerosegs!=0)
                    {
                        // Remove Duplicates
                        ContigSeg nodup_segs[6];
                        int nNodup_segs=1;
                        nodup_segs[0] = noZerosegs[0];
                        for (int i=1; i<numnonzerosegs; i++)
                        {
                            if ( (noZerosegs[i].first!=nodup_segs[nNodup_segs-1].first ||
                                noZerosegs[i].last!=nodup_segs[nNodup_segs-1].last) )
                                nodup_segs[nNodup_segs++] = noZerosegs[i];
                        }

                        // printf("==== nodup_segs ====\n");
                        // for (int i=0; i<nNodup_segs; i++)
                        //     printf("nodup_segs[%d] idx1 %d idx2 %d sum %d\n",
                        //         i, nodup_segs[i].first, nodup_segs[i].last, nodup_segs[i].sum);
                        
                        // Merge the segments into a single list of combined segments
                       // printf("%d\n", k);
                        ContigSeg merged_segs[6];
                        int nMerged_segs=0;
                        for (int i=0; i<nNodup_segs; i++)
                        {
            //				if (i<nNodup_segs-1 && nodup_segs[i].last == nodup_segs[i+1].first)
                            if (i<nNodup_segs-1 && nodup_segs[i+1].first - nodup_segs[i].last <= 4)
                            {
                                //printf("%d\n", nodup_segs[i+1].first - nodup_segs[i].last);
                                merged_segs[nMerged_segs].first = nodup_segs[i].first;
                                merged_segs[nMerged_segs].last = nodup_segs[i+1].last;
                                merged_segs[nMerged_segs++].sum = nodup_segs[i].sum + nodup_segs[i+1].sum;
                                i++;
                            }
                            else
                            {
                                merged_segs[nMerged_segs++] = nodup_segs[i];
                            }
                        }
                        
            //			printf("==== merged_segs ====\n");
                        // for (int i=0; i<nMerged_segs; i++)
                        //     printf("merged_segs[%d] idx1 %d idx2 %d sum %d\n",
                        //         i, merged_segs[i].first, merged_segs[i].last, merged_segs[i].sum);
                    
                        // Keep the start, longest, and end into C
                        C.start = merged_segs[0];                  // What if these are length 0 ??
                        C.end   = merged_segs[nMerged_segs-1];
                        
                        int longest_idx = 0;
                        for (int i=1; i<nMerged_segs; i++)
                        {
                            if (merged_segs[i].sum > merged_segs[longest_idx].sum)
                                longest_idx = i;
                        }
                        C.longest = merged_segs[longest_idx];			
                    }
                    else
                    {
                        C.start.first = A.start.first;
                        C.start.last = A.start.first;
                        C.start.sum = 0;
                        C.longest.first  = A.start.first;
                        C.longest.last  = A.start.first;
                        C.longest.sum = 0;
                        C.end.first =  B.end.last;
                        C.end.last = B.end.last;
                        C.end.sum = 0;			
                    }               
                    //
                    
                    }
                else if(flag_1 == 0 && flag_2 == 1)
                {
                    C = B;
                }
                else if(flag_1 == 1 && flag_2 == 0)
                {
                    C = A;
                }
                 T[L][s][k][f] = C;
                } 
                   
           }

       }
      //printf("yes\n");

      // In end of while loop
       L++;
       H = 2*H;
       S = S/2;
    }
    K_Radon rad;
    rad.T = T;
    rad.F = F;
    rad.H = H;
    rad.K = K;
    rad.L = L;
    rad.N = N;
    rad.S = S;
    return rad;
    // Implementing Inverse Radon Transform i.e. filtered back propogation
    // printf("Successfully finished dynamic programming !!!* \n now step to output image \n");
}

byte *** plotRadonTransform(K_Radon rad)
{
byte ***output_image = malloc3d(rad.N, rad.N, 3);
    //printf("ok \n");
    int pixel_value = 255;
	for (int y=0; y< rad.N; y++){
		for (int x=0; x<rad.N; x++) {
			// if(!(rad.T[L-1][0][y][x].longest.sum))
            //      output_image[y][x] = 0;
            // else{
            //     if(rad.T[L-1][0][y][x].longest.sum != 0)
            //         printf(" img[%d][%d]: rad.T[L-1][0][y][x].start: %d , rad.T[L-1][0][y][x].end - %d, %d\n",y,x, rad.T[L-1][0][y][x].start, rad.T[L-1][0][y][x].end, rad.T[L-1][0][y][x].longest.sum );
            //    
                if(rad.T[rad.L-1][0][y][x].longest.sum >255) {
                    output_image[y][x][0] = pixel_value;
                    output_image[y][x][1] = pixel_value;
                    output_image[y][x][2] = pixel_value;
                } else {    
                    output_image[y][x][0] =  rad.T[rad.L-1][0][y][x].longest.sum;
                    output_image[y][x][1] =  rad.T[rad.L-1][0][y][x].longest.sum;
                    output_image[y][x][2] =  rad.T[rad.L-1][0][y][x].longest.sum;
                }
            // }
		}
        
	}
    for(int i=0; i<10; i++)
    {
        for(int j=0; j<10; j++)
        {
         output_image[i][j][0] = 255;
         output_image[i][j][1] = 0;
         output_image[i][j][2] = 0;
        }     
    }
   
    return output_image;
}

byte **mirror_image_x(byte **img, int rows, int cols)
{
    int x,y;
    byte **out = malloc2d(rows,cols);
    for (y=0; y<rows; y++) {
        for (x=0; x<cols; x++) {
            out[y][cols-x-1] = img[y][x];
        }
    }
    return out;
}

byte **transpose_image(byte **img, int rows, int cols)
{
    int x,y;
    byte **out = malloc2d(cols,rows);
    for (y=0; y<rows; y++) {
        for (x=0; x<cols; x++) {
            out[x][y] = img[y][x];
        }
    }
    return out;
}
XY orientCoordinate(int x, int y, int image_number, int rows, int cols)
    {
        XY peak;
        switch(image_number)
        {
            // grays[0] = gray;
            // grays[1] = mirror_image_x(gray, rows, cols);
            // grays[2] = transpose_image(gray, rows, cols);
            // grays[3] = mirror_image_x(grays[2], rows, cols);
            case 0: 
                peak.x = x;
                peak.y = y;
                break;
            case 1:
                peak.x = rows - x - 1;
                peak.y = y;
                break;
            case 2:
                peak.x = y;
                peak.y = x;
                break;
            case 3:
                peak.x = y;
                peak.y = cols - x - 1;
                break;
        }
        return peak;
        
    }
XY computingPeaks(K_Radon rad, byte*** output_image, byte*** img, int image_number)
{
    // Create a blur kernel and normalize
    int nBlur_kernel = 15;
    float blur_kernel[15] = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1};

    // normalize the kernel
    float sum=0.0;
    for (int i=0; i<nBlur_kernel; i++)
        sum += blur_kernel[i];
    for (int i=0; i<nBlur_kernel; i++)
        blur_kernel[i] /= sum;

    // blur in x
    float **blur_x = malloc2d_float(rad.N, rad.N);
    for (int y=0; y<rad.N; y++) {
        for (int x=0; x<rad.N; x++) {
            float sum = 0.0;
            int start = x - nBlur_kernel/2;
            for (int k=0; k<nBlur_kernel; k++)
                if (start+k>=0 && start+k<rad.N)
                    sum += blur_kernel[k] * (float)rad.T[rad.L-1][0][y][start+k].longest.sum;
            blur_x[y][x] = sum;
        }
    }

    // blur in y
    float **blur = malloc2d_float(rad.N, rad.N);
    for (int y=0; y<rad.N; y++) {
        for (int x=0; x<rad.N; x++) {
            float sum = 0.0;
            int start = y - nBlur_kernel/2;
            for (int k=0; k<nBlur_kernel; k++)
                if (start+k>=0 && start+k<rad.N)
                    sum += blur_kernel[k] * blur_x[start+k][x];
            blur[y][x] = sum;
        }
    }

    // XY computingPeaks(K_Radon k, float** blur, byte*** output_image)
    
    XY *peaks = (XY*)malloc(rad.N*rad.N*sizeof(XY));
    int nPeaks = 0;

    //-----------------------------------
    // Non-maximal suppression to highlight peaks
    //-----------------------------------
    for (int y=1; y<rad.N-1; y++) {
        for (int x=1; x<rad.N-1; x++) {
            /*if (
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x-1].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x+1].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y][x-1].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y][x+1].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x-1].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x].longest.sum &&
                rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x+1].longest.sum
            )*/
            if (
                blur[y][x] >= 50 &&
                blur[y][x] >= blur[y-1][x-1] &&
                blur[y][x] >= blur[y-1][x] &&
                blur[y][x] >= blur[y-1][x+1] &&
                blur[y][x] >= blur[y][x-1] &&
                blur[y][x] >= blur[y][x+1] &&
                blur[y][x] >= blur[y+1][x-1] &&
                blur[y][x] >= blur[y+1][x] &&
                blur[y][x] >= blur[y+1][x+1]
            )
            {
                output_image[y][x][0] = 0;
                output_image[y][x][1] = 0;
                output_image[y][x][2] = 255;

                // Record the peak that we have observed
                peaks[nPeaks].x = x;
                peaks[nPeaks].y = y;
                nPeaks++;
            }
        }
    }
    int p;
    int iter;

    // Optimize each peak independently
    for (p=0; p<nPeaks; p++)
    {
        // Perform simulated annealing for the peak
        int best_x = peaks[p].x;
        int best_y = peaks[p].y;
        int best_val = rad.T[rad.L-1][0][best_y][best_x].longest.sum;

        // Find the best peak
        float window_y = 80.0;
        float window_x = 40.0;
        float falloff = 1.0 / pow(40, 1.0/10.0);
        for (iter=0; iter<10; iter++)
        {
            // Guess a location around the peak
            //   find a region around the peak
            int minx = best_x - window_x;
            int maxx = minx + 2*window_x;
            int miny = best_y - window_y;
            int maxy = miny + 2*window_y;
            //   make sure the region is on the map
            minx = MAX(minx,0);
            maxx = MIN(maxx, rad.N-1);
            miny = MAX(miny,0);
            maxy = MIN(miny, rad.N-1);

            int curr_x;
            int curr_y;
/*
            // Is the value any better?
            int curr_x = (rand() % (maxx-minx+1)) + minx;
            int curr_y = (rand() % (maxy-miny+1)) + miny;
            int curr_val = rad.T[L-1][0][curr_y][curr_x].longest.sum;

            if (curr_val > best_val) {
                best_x = curr_x;
                best_y = curr_y;
                best_val = curr_val;
            }
*/
            for (curr_y=miny; curr_y<=maxy; curr_y++) {
                for (curr_x=minx; curr_x<=maxx; curr_x++) {
                    int curr_val = rad.T[rad.L-1][0][curr_y][curr_x].longest.sum;
                    if (curr_val > best_val) {
                        best_x = curr_x;
                        best_y = curr_y;
                        best_val = curr_val;
                    }
                }
            }

            //window *= falloff;
        }

        peaks[p].x = best_x;
        peaks[p].y = best_y;
        peaks[p].npeaks = nPeaks;
    }
     for (int p=0; p<peaks[p].npeaks; p++) {
        output_image[peaks[p].y][peaks[p].x][0] = 255;
        output_image[peaks[p].y][peaks[p].x][1] = 0;
        output_image[peaks[p].y][peaks[p].x][2] = 0;
    }
    for (int i=0; i<peaks[i].npeaks; i++)
    {
        //parameters of the line
            int f = peaks[i].x;
            int k = peaks[i].y;
            int start = rad.T[rad.L-1][0][k][f].longest.first;
            int end = rad.T[rad.L-1][0][k][f].longest.last;

        // end points of the line
            float d1 = (float)start/(float)rad.N;
            float d2 = (float)end/(float)rad.N;
            int Ax = f;
            int Ay = 0;
            int Bx = f - k;
            int By = rad.N - 1;
            int ax = d1 * ((Bx-Ax)) + Ax;
            int ay = d1 * ((By - Ay)) + Ay;
            int bx = d2 * ((Bx-Ax)) + Ax;
            int by = d2 * ((By - Ay)) + Ay;

        // Convert the endpoints of the line
            XY a = orientCoordinate(ax, ay, image_number, rad.N, rad.N);
            XY b = orientCoordinate(bx, by, image_number, rad.N, rad.N);
        //Draw the line 
            DrawLine(a.x,a.y,b.x,b.y,img,rad.N,rad.N);
        }

}
    
    void markingPeaks(K_Radon rad, XY peaks, byte*** output_image, byte*** img)
    {
    //     for (int p=0; p<peaks.npeaks; p++) {
    //     output_image[peaks.y][peaks.x][0] = 255;
    //     output_image[peaks.y][peaks.x][1] = 0;
    //     output_image[peaks.y][peaks.x][2] = 0;
    // }
    // for (int i=0; i<peaks.npeaks; i++)
    // {
    //     //parameters of the line
    //         int f = peaks.x;
    //         int k = peaks.y;
    //         int start = rad.T[rad.L-1][0][k][f].longest.first;
    //         int end = rad.T[rad.L-1][0][k][f].longest.last;

    //     // end points of the line
    //         float d1 = (float)start/(float)rad.N;
    //         float d2 = (float)end/(float)rad.N;
    //         int Ax = f;
    //         int Ay = 0;
    //         int Bx = f - k;
    //         int By = rad.N - 1;
    //         int ax = d1 * ((Bx-Ax)) + Ax;
    //         int ay = d1 * ((By - Ay)) + Ay;
    //         int bx = d2 * ((Bx-Ax)) + Ax;
    //         int by = d2 * ((By - Ay)) + Ay;
    //     //Draw the line 
    //         DrawLine(ax,ay,bx,by,img,rad.N,rad.N);
    //     }
    }
    // //------------------
    // // Draw the final peaks as Red
    // //------------------
    // 


int main()
{
    //------------------------------
    //Reading the image and converting it to Grayscale
    //------------------------------
    
    //loading a image file
    int y,x,i,k;
	int rows, cols, chan;

    byte ***img = LoadRgb(".\\TestImage_2.bmp", &rows, &cols, &chan);
    //printf("img %p rows %d cols %d chan %d\n", img, rows, cols, chan);

	byte **gray = malloc2d(rows, cols);
	for (y=0; y<rows; y++){
		for (x=0; x<cols; x++) {
			int r = img[y][x][0];
			int g = img[y][x][1];
			int b = img[y][x][2];
            int a = img[y][x][3];
			gray[y][x] =  (r+g+b+a) / 3;  //Creating a grayscale image from RGBA format image.
		}
	}    

    //-------------------------------
    // Make 4 grayscale images with different orientations
    //-------------------------------
    byte **grays[4];
    grays[0] = gray;
    grays[1] = mirror_image_x(gray, rows, cols);
    grays[2] = transpose_image(gray, rows, cols);
    grays[3] = mirror_image_x(grays[2], rows, cols);
    // grays[0] = tranpose_image(grays[1], rows,cols);
    // grays[1] = mirror_image_x(gray, rows, cols);
    // grays[2] = transpose_image(grays[2],rows,cols);
    // grays[4] = transpose_image(grays[2], rows,cols);
    // grays[5] = mirror_image_x(grays[3], rows, cols);
    // grays[6] = transpose_image(grays[4], rows,cols);
    // grays[7] = mirror_image_x(grays[5], rows, cols);

    K_Radon rads[4]; 
    byte*** outputImage[4];
    XY peakss[4];
    //----------------------------------
    //Running the Dynamic programming algorithm 
    //-----------------------------------
    for(int i=0; i<4; i++)
    {
        //printf("okay \n");
        rads[i] = K_radon_transform(rows, grays[i]);
        //printf("second\n");
        outputImage[i] = plotRadonTransform(rads[i]);
        computingPeaks(rads[i], outputImage[i], img, i);
        // peakss[i] = computingPeaks(rads[i], outputImage[i], img);
        //markingPeaks(rads[i], peakss[i], outputImage[i], img);
        char path[4096];
        sprintf(path, "result_%05d.png", i);
        SaveRgbPng(outputImage[i], path, rows, cols);
    }
    K_Radon rad = rads[0];
    byte*** output_image = outputImage[0];
    XY peaks = peakss[0];

    SaveRgbPng(img, "Resultnew.png",rows,cols);
    SaveRgbPng(output_image, "result_new.png", rows, cols);

	//SaveRgbPng(output_image, "result_new.png", rows, cols);
    // generating ouput file


    //---------------------------------------
    // Generate the output image of the Radon transform
    //---------------------------------------
    
    //-----------------------------------
    // Blur the output image to suppress noise in peaks 
    //-----------------------------------

    // // Create a blur kernel and normalize
    // int nBlur_kernel = 15;
    // float blur_kernel[15] = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1};

    // // normalize the kernel
    // float sum=0.0;
    // for (i=0; i<nBlur_kernel; i++)
    //     sum += blur_kernel[i];
    // for (i=0; i<nBlur_kernel; i++)
    //     blur_kernel[i] /= sum;

    // // blur in x
    // float **blur_x = malloc2d_float(rows, cols);
    // for (y=0; y<rows; y++) {
    //     for (x=0; x<cols; x++) {
    //         float sum = 0.0;
    //         int start = x - nBlur_kernel/2;
    //         for (k=0; k<nBlur_kernel; k++)
    //             if (start+k>=0 && start+k<cols)
    //                 sum += blur_kernel[k] * (float)rad.T[rad.L-1][0][y][start+k].longest.sum;
    //         blur_x[y][x] = sum;
    //     }
    // }

    // // blur in y
    // float **blur = malloc2d_float(rows, cols);
    // for (y=0; y<rows; y++) {
    //     for (x=0; x<cols; x++) {
    //         float sum = 0.0;
    //         int start = y - nBlur_kernel/2;
    //         for (k=0; k<nBlur_kernel; k++)
    //             if (start+k>=0 && start+k<rows)
    //                 sum += blur_kernel[k] * blur_x[start+k][x];
    //         blur[y][x] = sum;
    //     }
    // }

    // XY computingPeaks(K_Radon k, float** blur, byte*** output_image)

    // XY *peaks = (XY*)malloc(rows*cols*sizeof(XY));
    // int nPeaks = 0;

    // //-----------------------------------
    // // Non-maximal suppression to highlight peaks
    // //-----------------------------------
    // for (y=1; y<rows-1; y++) {
    //     for (x=1; x<cols-1; x++) {
    //         /*if (
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x-1].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y-1][x+1].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y][x-1].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y][x+1].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x-1].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x].longest.sum &&
    //             rad.T[L-1][0][y][x].longest.sum >= rad.T[L-1][0][y+1][x+1].longest.sum
    //         )*/
    //         if (
    //             blur[y][x] >= 50 &&
    //             blur[y][x] >= blur[y-1][x-1] &&
    //             blur[y][x] >= blur[y-1][x] &&
    //             blur[y][x] >= blur[y-1][x+1] &&
    //             blur[y][x] >= blur[y][x-1] &&
    //             blur[y][x] >= blur[y][x+1] &&
    //             blur[y][x] >= blur[y+1][x-1] &&
    //             blur[y][x] >= blur[y+1][x] &&
    //             blur[y][x] >= blur[y+1][x+1]
    //         )
    //         {
    //             output_image[y][x][0] = 0;
    //             output_image[y][x][1] = 0;
    //             output_image[y][x][2] = 255;

    //             // Record the peak that we have observed
    //             peaks[nPeaks].x = x;
    //             peaks[nPeaks].y = y;
    //             nPeaks++;
    //         }
    //     }
    // }

    //---------------------
    // Optimize the peaks location
    //---------------------
//     int p;
//     int iter;

//     // Optimize each peak independently
//     for (p=0; p<nPeaks; p++)
//     {
//         // Perform simulated annealing for the peak
//         int best_x = peaks[p].x;
//         int best_y = peaks[p].y;
//         int best_val = rad.T[rad.L-1][0][best_y][best_x].longest.sum;

//         // Find the best peak
//         float window_y = 80.0;
//         float window_x = 40.0;
//         float falloff = 1.0 / pow(40, 1.0/10.0);
//         for (iter=0; iter<10; iter++)
//         {
//             // Guess a location around the peak
//             //   find a region around the peak
//             int minx = best_x - window_x;
//             int maxx = minx + 2*window_x;
//             int miny = best_y - window_y;
//             int maxy = miny + 2*window_y;
//             //   make sure the region is on the map
//             minx = MAX(minx,0);
//             maxx = MIN(maxx, cols-1);
//             miny = MAX(miny,0);
//             maxy = MIN(miny, rows-1);

//             int curr_x;
//             int curr_y;
// /*
//             // Is the value any better?
//             int curr_x = (rand() % (maxx-minx+1)) + minx;
//             int curr_y = (rand() % (maxy-miny+1)) + miny;
//             int curr_val = rad.T[L-1][0][curr_y][curr_x].longest.sum;

//             if (curr_val > best_val) {
//                 best_x = curr_x;
//                 best_y = curr_y;
//                 best_val = curr_val;
//             }
// */
//             for (curr_y=miny; curr_y<=maxy; curr_y++) {
//                 for (curr_x=minx; curr_x<=maxx; curr_x++) {
//                     int curr_val = rad.T[rad.L-1][0][curr_y][curr_x].longest.sum;
//                     if (curr_val > best_val) {
//                         best_x = curr_x;
//                         best_y = curr_y;
//                         best_val = curr_val;
//                     }
//                 }
//             }

//             //window *= falloff;
//         }

//         peaks[p].x = best_x;
//         peaks[p].y = best_y;
//     }


//     //------------------
//     // Draw the final peaks as Red
//     //------------------
    // for (int p=0; p<peaks.npeaks; p++) {
    //     output_image[peaks.y][peaks.x][0] = 255;
    //     output_image[peaks.y][peaks.x][1] = 0;
    //     output_image[peaks.y][peaks.x][2] = 0;
    // }
    // for (int i=0; i<peaks.npeaks; i++)
    // {
    //     //parameters of the line
    //         int f = peaks.x;
    //         int k = peaks.y;
    //         int start = rad.T[rad.L-1][0][k][f].longest.first;
    //         int end = rad.T[rad.L-1][0][k][f].longest.last;

    //     // end points of the line
    //         float d1 = (float)start/(float)rows;
    //         float d2 = (float)end/(float)rows;
    //         int Ax = f;
    //         int Ay = 0;
    //         int Bx = f - k;
    //         int By = rows - 1;
    //         int ax = d1 * ((Bx-Ax)) + Ax;
    //         int ay = d1 * ((By - Ay)) + Ay;
    //         int bx = d2 * ((Bx-Ax)) + Ax;
    //         int by = d2 * ((By - Ay)) + Ay;
    //     //Draw the line 
    //         DrawLine(ax,ay,bx,by,img,rows,cols);
    // }

   
    // y=0;
    //  for (int x=0; x< cols; x++){
    //     printf("rad.T[L-1][0][0][x].start: %d , rad.T[L-1][0][y][x].end - %d, %d\n", rad.T[rad.L-1][0][y][x].start, rad.T[rad.L-1][0][y][x].end, rad.T[rad.L-1][0][y][x].longest.sum );
    //     output_image[y][x] =  rad.T[rad.L-1][0][y][x].longest.sum;
    //  }
    
    // printf("image is generated \n");


}
