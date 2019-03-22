/**************************************************************

	The program reads an BMP image file and creates a new
	image that is the blurry of the input file.

**************************************************************/

#include "qdbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int  bsize, num_threads;
BMP* bmp;
BMP* copy_bmp;

void* blur(void* number){
    UCHAR   *r, *g, *b;
    int     final_r, final_g, final_b; 
	UINT	width, height;
	UINT	x, y;
    int     m, n, k, j;
    int     llim, rlim, ulim, dlim;
    r = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));
    g = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));
    b = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));
    int num = *(int*) number;
	/* Get image's dimensions */
	width = BMP_GetWidth( bmp );
	height = BMP_GetHeight( bmp );
	/* Iterate through all the image's pixels */
	for ( x = width*num/(num_threads) ; x < width*(num+1)/(num_threads) ; ++x )
	{
		for ( y = 0 ; y < height ; ++y )
		{
			/* Get surrounding pixel's RGB values */
            k = 0;
            llim = (int)(x - bsize);
            rlim = (int)(x + bsize);
            ulim = (int)(y - bsize);
            dlim = (int)(y + bsize);
            if(llim<0) llim = 0;
            if(rlim>width) rlim = width;
            if(dlim>height) dlim = height;
            if(ulim<0) ulim = 0;
            for( m = llim; m <= rlim; m++){
                for(n = ulim; n <= dlim; n++){
                    BMP_GetPixelRGB( bmp, m, n, r+k, g+k, b+k);
                    k++;
                }
            }
			/* Blur RGB values */
            //calculate RGB
            final_r = (float)r[0];
            final_g = (float)g[0];
            final_b = (float)b[0];
            for(j = 1; j < k; j++){
                final_r = final_r+(float)r[j];
                final_g = final_g+(float)g[j];
                final_b = final_b+(float)b[j];
            }
			BMP_SetPixelRGB( copy_bmp, x, y, (UCHAR)(final_r/k), (UCHAR)(final_g/k), (UCHAR)(final_b/k) );
		}
	}
    free(r);
    free(g);
    free(b);
    void* unused=NULL;
    return unused;
}

/* Creates a negative image of the input bitmap file */
int main( int argc, char* argv[] )
{
	pthread_t* t;
    int j;
    //sequence letting each thread process part of data
    int* sequence;
    void* unused;
    
	/* Check arguments */
	if ( argc != 5 )
	{
		fprintf( stderr, "Usage: %s <input file> <output file>\n", argv[ 0 ] );
		return 0;
	}
    if(atoi(argv[3])<=0){
        printf("error size!\n");
        return 0;
    }
    if(atoi(argv[4])<=0){
        printf("must has at least one threads!\n");
        return 0;
    }
	/* Read an image file */
    //original file
	bmp = BMP_ReadFile( argv[ 1 ] );
    BMP_CHECK_ERROR( stdout, -1 );
    //copy file
    copy_bmp = BMP_ReadFile( argv[1] );
	BMP_CHECK_ERROR( stdout, -1 );
    
    //read box size
    bsize = atoi(argv[3]);
    
    //read threads number
    num_threads = atoi(argv[4]);
    
    //build sequence
    sequence = malloc(num_threads*sizeof(int));
    for(j = 0; j < num_threads; j++){
        sequence[j] = j;
    }
    
    t = malloc(num_threads*sizeof(pthread_t));
    for(j=0;j<num_threads;j++){
        pthread_create(&t[j], NULL, &blur, &sequence[j]);
    }
    for(j=0;j<num_threads;j++){
        //just to meet the format request of pthread_join
        pthread_join(t[j], &unused);
    }
	/* Save result */
	BMP_WriteFile( copy_bmp, argv[ 2 ] );
	BMP_CHECK_ERROR( stdout, -2 );

	/* Free all memory allocated for the image */
    free(sequence);
    free(t);
	BMP_Free( bmp );
    BMP_Free( copy_bmp );

	return 0;
}