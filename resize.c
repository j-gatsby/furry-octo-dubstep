/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Accepts exactly 3 command-line arguments
 *		- the first must be a positive int <= 100
 *		- the second must be the name of the file to be resized
 *		- the third must be the name of the resized version to be written
 *
 *				 ex. -  ./resize n infile outfile
 *						-  ./resize 4 small.bmp large.bmp
 *
 * Must support n=1, the result of which is an outfile with dimensions identical to infile's
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

	// remember desired outfile size & change argv[1]  to type int
	int resize = atoi(argv[1]);       
	
	// validate argv[1] is a positive int between 0-100
	if (resize < 1 || resize > 100)   
	{
		printf("Usage: ./resize n infile outfile\n");
		printf(" - n must be of value 1-100\n");
		return 2;
	}
	
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    //  Change header info
    int oldWidth = bi.biWidth;
    int oldHeight = bi.biHeight;
   bi.biWidth = oldWidth * resize;			
   bi.biHeight = oldHeight * resize;			
    
	// determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;																														
	
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);		

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);     

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(oldHeight); i < biHeight; i++)	
    {
    	//  get position of cursor while it is at the start of the scanline
    	fpos_t position;									
    	fgetpos(inptr, &position);									
    	//  Thanks to cplusplus.com/reference/cstdio/fsetpos
    	
    	for(int printline = 0; printline < resize; printline++)
    	{
    		// set cursor position of infile back to the beginning of the scanline
    		fsetpos(inptr, &position);
    		
		    // iterate over pixels in scanline
		    for (int j = 0; j < (oldWidth); j++)				 
		    {																						
		        // temporary storage
		        RGBTRIPLE triple;       
		        
		        // read RGB triple from infile 
		        fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
		        
		        //  Iterate over each pixel read in, resize times
				for (int printpix = 0; printpix < resize; printpix++)
				{
		        	// write RGB triple to outfile
		        	fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
		    	} 
	    	}
    	 	// skip over padding, if any
	    	fseek(inptr, padding, SEEK_CUR);

		   	// then add it back (to demonstrate how)
	    	for (int k = 0; k < padding; k++)
	    	{
	       	 fputc(0x00, outptr);
	    	}  
		}
    }
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
