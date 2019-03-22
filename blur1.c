/**************************************************************

	The program reads an BMP image file and creates a new
	image that is the blurry of the input file.

**************************************************************/

#include "qdbmp.h"
#include <stdio.h>
#include <stdlib.h>

/* Creates a negative image of the input bitmap file */
int main( int argc, char* argv[] )
{
	UCHAR*	r, *g, *b;
    //use float to prevent overflow
    float     final_r, final_g, final_b;
	UINT	width, height;
	UINT	x, y;
    int     m, n, k, j;
    int     llim, rlim, ulim, dlim;
	BMP*	bmp;
    BMP*    copy_bmp;
    int     bsize;

	/* Check arguments */
	if ( argc != 4 )
	{
		fprintf( stderr, "Usage: %s <input file> <output file>\n", argv[ 0 ] );
		return 0;
	}
    if(atoi(argv[3])<=0){
        printf("error size!\n");
        return 0;
    }
	/* Read an image file */
    //original file
	bmp = BMP_ReadFile( argv[ 1 ] );
    BMP_CHECK_ERROR( stdout, -1 );
    //copy file
    copy_bmp = BMP_ReadFile( argv[ 1 ] );
	BMP_CHECK_ERROR( stdout, -1 );
    bsize = atoi(argv[3]);
    r = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));
    g = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));
    b = malloc((2*bsize+1)*(2*bsize+1)*sizeof(UCHAR));

	/* Get image's dimensions */
	width = BMP_GetWidth( bmp );
	height = BMP_GetHeight( bmp );

	/* Iterate through all the image's pixels */
	for ( x = 0 ; x < width ; x++ )
	//for(x=0;x<1;x++)
	{
		for ( y = 0 ; y < height ; y++ )
		//for(y=0;y<1;y++)
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

	/* Save result */
	BMP_WriteFile( copy_bmp, argv[ 2 ] );
	BMP_CHECK_ERROR( stdout, -2 );


	/* Free all memory allocated for the image */
    free(r);
    free(g);
    free(b);
	BMP_Free( bmp );
    BMP_Free( copy_bmp );

	return 0;
}