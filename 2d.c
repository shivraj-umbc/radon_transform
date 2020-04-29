#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dc_image.h"
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )

#define u32 unsigned int

u32 N; // N = width =height  populate this with row/height of the image
int L; //level
int K; //skew

int S; // stack
int F; //Offset

int H; //height=2^L

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

int main()
{
    //loading a image file
    int y,x;
	int rows, cols, chan;

    byte ***img = LoadRgb(".\\TestImage.bmp", &rows, &cols, &chan);
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

    N = rows;

    u32 nLev = 1;
	u32 subsize = 1;
	while (subsize < N) {
		subsize *= 2;
		nLev++;
	}
	printf("nlev %d\n", nLev);

    LineSeg**** T;
    T = (LineSeg****)malloc(nLev* sizeof(LineSeg***));

    //Initialisation of these variables before performing loops
    
    // Copy problems of size 1 Take a look at this
	// for (int p=0; p<N; p++)
	// {
	// 	if (line[p] == 'x')    // We have a length 1 line
	// 	{
	// 		T[0][p].start.first = p;
	// 		T[0][p].start.last = p+1;
	// 		T[0][p].start.sum = 1;
	// 		T[0][p].longest.first  = p;
	// 		T[0][p].longest.last  = p+1;
	// 		T[0][p].longest.sum = 1;
	// 		T[0][p].end.first   = p;
	// 		T[0][p].end.last   = p+1;
	// 		T[0][p].end.sum = 1;
	// 	}
	// 	else                   // We do not have a line

	// 	{
	// 		T[0][p].start.first = p;
	// 		T[0][p].start.last = p;
	// 		T[0][p].start.sum = 0;
	// 		T[0][p].longest.first  = p;
	// 		T[0][p].longest.last  = p;
	// 		T[0][p].longest.sum = 0;
	// 		T[0][p].end.first   = p+1;
	// 		T[0][p].end.last   = p+1;
	// 		T[0][p].end.sum = 0;
	// 	}
    //		PrintLineSeg(T[0][p]);
    //}

	


    ////////
    L = 1;
    S = N/2;
    H = 2;
    while(H<=N){
        
       T[L] = (LineSeg***)malloc(S * sizeof(LineSeg**));
       for(int s = 0; s < S ; s++){
           K = H;
           T[L][s] = (LineSeg**)malloc(K * sizeof(LineSeg*));
           for(int k = 0; k< K; K++){
               F = N+k;
                T[L][s][k] = (LineSeg*)malloc(F * sizeof(LineSeg));
                for(int f = 0; f <F; f++){
                  
                    
                    //calculate A and B and combine into C

                    LineSeg A = T[L-1][2*s][k/2][f];
			        LineSeg B = T[L-1][2*s+1][k/2][f-(k+1)/2];
			        LineSeg C;
                   
                    // Append the segments to a list of segments
                    //  xx xxxx xx|xxx xx xxxx
                    ContigSeg segs[6];   // appended segments
                    int nSegs=6;
                    segs[0]=A.start;
                    segs[1]=A.longest;
                    segs[2]=A.end;
                    segs[3]=B.start;
                    segs[4]=B.longest;
                    segs[5]=B.end;

                    printf("==== segs ====\n");
                    for (int i=0; i<nSegs; i++)
                        printf("segs[%d] idx1 %d idx2 %d sum %d\n",
                            i, segs[i].first, segs[i].last, segs[i].sum);
                    
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
                    printf("%d\n", numnonzerosegs);
                    printf("====non zero segs ====\n");
                    for (int i=0; i<numnonzerosegs; i++)
                        printf("noZerosegs[%d] first  %d last %d sum %d\n",
                            i, noZerosegs[i].first, noZerosegs[i].last, noZerosegs[i].sum);

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

                        printf("==== nodup_segs ====\n");
                        for (int i=0; i<nNodup_segs; i++)
                            printf("nodup_segs[%d] idx1 %d idx2 %d sum %d\n",
                                i, nodup_segs[i].first, nodup_segs[i].last, nodup_segs[i].sum);
                        
                        // Merge the segments into a single list of combined segments
                        printf("%d\n", k);
                        ContigSeg merged_segs[6];
                        int nMerged_segs=0;
                        for (int i=0; i<nNodup_segs; i++)
                        {
            //				if (i<nNodup_segs-1 && nodup_segs[i].last == nodup_segs[i+1].first)
                            if (i<nNodup_segs-1 && nodup_segs[i+1].first - nodup_segs[i].last <= k)
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
                        for (int i=0; i<nMerged_segs; i++)
                            printf("merged_segs[%d] idx1 %d idx2 %d sum %d\n",
                                i, merged_segs[i].first, merged_segs[i].last, merged_segs[i].sum);
                    
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
                    T[L][s][k][f] = C;
                }

           }

       }
      // In end of while loop
       L++;
       H = 2*H;
       S = S/2;
    }
}
