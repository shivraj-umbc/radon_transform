#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define u32 unsigned int

u32 N;
char *line = "   xxx     xxxxxxxx   xx xxxx   xx xxxxx  xxxxxxx   xxx  xxxx   ";
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

void PrintLineSeg(LineSeg s)
{
	int i;
	for (i=0; i<N+1; i++)
	{
		printf("%02d: ", i);
		printf("%c", line[i]);

		// start
		if (i == s.start.first)
			printf("s1 ");
		if (i == s.start.last)
			printf("s2 sum: %d  ", s.start.sum);

		// end
		if (i == s.end.first)
			printf("e1 ");
		if (i == s.end.last)
			printf("e2 sum: %d  ", s.end.sum);

		// longest
		if (i == s.longest.first)
			printf("L1 ");
		if (i == s.longest.last)
			printf("L2 sum: %d  ", s.longest.sum);

		printf("\n");
	}
	
	printf("press enter to continue\n");
	fgetc(stdin);
}

int main()
{
	u32 i,p,L;

	int k;
    printf("Enter the value of k: ");
	k = getchar();
	//printf("k : %c", k);
	printf("FUBAR\n");

	// How long is the string
	N = strlen(line);
//	N = 16;

	// How many levels
	u32 nLev = 1;
	u32 subsize = 1;
	while (subsize < N) {
		subsize *= 2;
		nLev++;
	}
	printf("nlev %d\n", nLev);
	
	// Allocate table
	LineSeg **T;
	T = (LineSeg**)malloc(nLev * sizeof(LineSeg*));
	u32 nProb = N;
	for (L=0; L<nLev; L++) {
		T[L] = (LineSeg*)malloc(nProb * sizeof(LineSeg));
		printf("Lev %d nprob %d\n", L, nProb);
		nProb /= 2;
	}
	
	// Copy problems of size 1
	for (p=0; p<N; p++)
	{
		if (line[p] == 'x')    // We have a length 1 line
		{
			T[0][p].start.first = p;
			T[0][p].start.last = p+1;
			T[0][p].start.sum = 1;
			T[0][p].longest.first  = p;
			T[0][p].longest.last  = p+1;
			T[0][p].longest.sum = 1;
			T[0][p].end.first   = p;
			T[0][p].end.last   = p+1;
			T[0][p].end.sum = 1;
		}
		else                   // We do not have a line
		{
			T[0][p].start.first = p;
			T[0][p].start.last = p;
			T[0][p].start.sum = 0;
			T[0][p].longest.first  = p;
			T[0][p].longest.last  = p;
			T[0][p].longest.sum = 0;
			T[0][p].end.first   = p+1;
			T[0][p].end.last   = p+1;
			T[0][p].end.sum = 0;
		}

//		PrintLineSeg(T[0][p]);
	}
	
	// For every subproblem
	subsize = 2;

	nProb = N/2;
	L=1;
	while (subsize <= N)
	{
		printf("subsize %d N %d\n", subsize, N);
		printf("nProb %d\n", nProb);
		
		for (p=0; p<nProb; p++)
		{
//			printf("------------------------\n");
//			printf("p: %d\n", p);
			LineSeg A = T[L-1][2*p];
			LineSeg B = T[L-1][2*p+1];
			LineSeg C;

			//printf("-----\n");
			//printf("A:\n");
			//PrintLineSeg(A);
			
			//printf("-----\n");
			//printf("B:\n");
			//PrintLineSeg(B);			
			
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

			//printf("==== segs ====\n");
			//for (i=0; i<nSegs; i++)
				//printf("segs[%d] idx1 %d idx2 %d sum %d\n",
					//i, segs[i].first, segs[i].last, segs[i].sum);
			
			// Remove Duplicates
			ContigSeg nodup_segs[6];
			int nNodup_segs=1;
			nodup_segs[0] = segs[0];
			for (i=1; i<6; i++)
			{
				if ( (segs[i].first!=nodup_segs[nNodup_segs-1].first ||
					  segs[i].last!=nodup_segs[nNodup_segs-1].last) )
					nodup_segs[nNodup_segs++] = segs[i];
			}

			//printf("==== nodup_segs ====\n");
			//for (i=0; i<nNodup_segs; i++)
				//printf("nodup_segs[%d] idx1 %d idx2 %d sum %d\n",
					//i, nodup_segs[i].first, nodup_segs[i].last, nodup_segs[i].sum);
			
			// Merge the segments into a single list of combined segments
			
			ContigSeg merged_segs[6];
			int nMerged_segs=0;
			for (i=0; i<nNodup_segs; i++)
			{
				if (i<nNodup_segs-1 && nodup_segs[i].last == nodup_segs[i+1].first)
				{
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
			//for (i=0; i<nMerged_segs; i++)
				//printf("merged_segs[%d] idx1 %d idx2 %d sum %d\n",
					//i, merged_segs[i].idx1, merged_segs[i].idx2, merged_segs[i].sum);
		
			// Keep the start, longest, and end into C
			C.start = merged_segs[0];                  // What if these are length 0 ??
			C.end   = merged_segs[nMerged_segs-1];
			
			int longest_idx = 0;
			for (i=1; i<nMerged_segs; i++)
			{
				if (merged_segs[i].sum > merged_segs[longest_idx].sum)
					longest_idx = i;
			}
			C.longest = merged_segs[longest_idx];
			
			T[L][p] = C;

			//printf("-----\n");
			//printf("C:\n");
			//PrintLineSeg(C);
		}
		
		nProb /= 2;
		subsize *= 2;
		L++;
	}
	
	LineSeg res = T[nLev-1][0];
	PrintLineSeg(res);
	
	printf("longest segment:\n");
	printf(" restult start %d end %d sum %d\n", res.longest.first, res.longest.last, res.longest.sum);
	
	
	printf("%s\n", line);
	
	printf("len %d\n", strlen(line));
	
	return 0;
}




/*			segs[nSegs++] = A.start;
			if (A.longest.idx1 > segs[nSegs-1].idx1)
				segs[nSegs++] = A.longest;
			if (A.end.idx1 > segs[nSegs-1].idx1)
				segs[nSegs++] = A.end;
			if (B.start.idx1 > segs[nSegs-1].idx1)
				segs[nSegs++] = B.start;
			if (B.longest.idx1 > segs[nSegs-1].idx1)
				segs[nSegs++] = B.longest;
			if (B.end.idx1 > segs[nSegs-1].idx1)
				segs[nSegs++] = B.end;
*/
/*
			segs[nSegs++] = A.start;
			if ( (A.longest.idx1!=A.start.idx1 || A.longest.idx2!=A.start.idx2) &&
			     (A.longest.idx1!=A.end.idx1 || A.longest.idx2!=A.end.idx2) )
				 segs[nSegs++] = A.longest;
			segs[nSegs++] = A.end;
			segs[nSegs++] = B.start;
			if ( (B.longest.idx1!=B.start.idx1 || B.longest.idx2!=B.start.idx2) &&
			     (B.longest.idx1!=B.end.idx1 || B.longest.idx2!=B.end.idx2) )
				 segs[nSegs++] = A.longest;
			segs[nSegs++] = B.end;			
*/
			