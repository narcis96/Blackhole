#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>

#define SIZE  8
#define BLOCK 5

int convCoord(int i, int j) {
	int c = 0;
	while (i) {
		i--;
		c += SIZE-i;
	}
	return c+j;
}

int main() {
	bool bd[SIZE][SIZE];
	bool conn[SIZE*(SIZE+1)/2][SIZE*(SIZE+1)/2];
	
	struct timeval time; 
	gettimeofday(&time,NULL);
	
	srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
	
	bool retry;
	
	do {
		retry = false;
		for (int i=0; i<SIZE; i++) {
			for (int j=0; j<SIZE-i; j++) {
				bd[i][j] = true;
			}
		}
		
		for (int i=0; i<BLOCK; i++) {
			int tileSel = rand()%((SIZE)*(SIZE+1)/2 - i);
			int x = 0;
			int y = 0;
			while (tileSel || !bd[x][y]) {
				if (bd[x][y])
					tileSel--;
				x++;
				if (x == SIZE-y) {
					x = 0;
					y++;
				}
			}
			
			bd[x][y] = false;
		}
		
		int tiles = SIZE*(SIZE+1)/2;
		for (int i=0; i<tiles; i++) {
			for (int j=0; j<tiles; j++) {
				conn[i][j] = false;
			}
		}
		
		for (int i=0; i<SIZE; i++) {
			for (int j=0; j<SIZE-i; j++) {
				if (!bd[i][j])
					continue;
				
				conn[convCoord(i,j)][convCoord(i,j)] = true;
				
				if (i > 0 && bd[i-1][j])
					conn[convCoord(i-1,j)][convCoord(i,j)] = true;
				if (i > 0 && j < SIZE-i && bd[i-1][j+1])
					conn[convCoord(i-1,j+1)][convCoord(i,j)] = true;
				if (j < SIZE-i-1 && bd[i][j+1])
					conn[convCoord(i,j+1)][convCoord(i,j)] = true;
				if (j < SIZE-i-1 && i < SIZE-1 && bd[i+1][j])
					conn[convCoord(i+1,j)][convCoord(i,j)] = true;
				if (j > 0 && i < SIZE-1 && bd[i+1][j-1])
					conn[convCoord(i+1,j-1)][convCoord(i,j)] = true;
				if (j > 0 && bd[i][j-1])
					conn[convCoord(i,j-1)][convCoord(i,j)] = true;
			}
		}
		
		for (int k=0; k<tiles; k++) {
			for (int i=0; i<tiles; i++) {
				for (int j=0; j<tiles; j++) {
					if (conn[i][k] && conn[k][j]) conn[i][j] = true;
				}
			}
		}
		
		for (int i=0; i<tiles; i++) {
			for (int j=0; j<tiles; j++) {
				if (!conn[i][i] || !conn[j][j])
					continue;
				
				if (!conn[i][j])
					retry = true;
			}
		}
	} while (retry);
	
	FILE *f = fopen("blocks.txt", "w");
	//FILE *f = stdout;
	assert(f);
	
	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE-i; j++) {
			if (!bd[i][j])
				fprintf(f, "%c%d ", (char)('A'+i), j+1);
		}
	}
	fprintf(f, "\n");
	return 0;
}
