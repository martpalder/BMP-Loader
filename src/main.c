#include <stdio.h>
#include "bmp_loader.h"

void printBitmapHeader(Image*);
void printDibHeader(Image*);
void printPixelArray(Image*);

int main(void) {
	// open a BMP file
	Image* pImage = loadBmp("images/Colors.bmp");
	
	// print its contents
	printBitmapHeader(pImage);
	printDibHeader(pImage);
	printPixelArray(pImage);
	
	return 0;
}

void printBitmapHeader(Image* pImage)
{
	printf("\nBitmap file header");
	printf("\nFile identity:\t%u", pImage->bitmapHeader.identity);
	printf("\nFile size:\t%u", pImage->bitmapHeader.fileSize);
	printf("\nReserved 1:\t%u", pImage->bitmapHeader.reserved1);
	printf("\nReserved 2:\t%u", pImage->bitmapHeader.reserved2);
	printf("\nOffset:\t%u\n", pImage->bitmapHeader.offset);
}

void printDibHeader(Image* pImage)
{
	printf("\nDIB header");
	printf("\nHeader size:\t%u", pImage->dibHeader.headerSize);
	printf("\nImage width:\t%d", pImage->dibHeader.width);
	printf("\nImage height:\t%d", pImage->dibHeader.height);
	printf("\nColor planes:\t%u", pImage->dibHeader.colorPlanes);
	printf("\nColor depth:\t%u", pImage->dibHeader.bpp);
	printf("\nCompression:\t%u", pImage->dibHeader.compression);
	printf("\nImage size:\t%u", pImage->dibHeader.imageSize);
	printf("\nH-resolution:\t%d", pImage->dibHeader.hres);
	printf("\nV-resolution:\t%d", pImage->dibHeader.vres);
	printf("\nTotal colors:\t%u", pImage->dibHeader.totalColors);
	printf("\nImportant used colors:\t%u\n", pImage->dibHeader.usedColors);
}

void printPixelArray(Image* pImage)
{
	// calculate array size
	unsigned short rowSize = pImage->dibHeader.bpp * pImage->dibHeader.width / 8;
	unsigned int pixelArraySize = rowSize * pImage->dibHeader.height;
	
	// calculate number of pixels
	unsigned int numPixels = pixelArraySize / (pImage->dibHeader.bpp / 8);
	
	unsigned int pos = 0;
	for (unsigned int i = 0; i < numPixels; ++i) {
		printf("\nPixel %u:\t", ++pos);
		printf("Value: %X\t", pImage->pixelArray[i]);
		printf("R: %u\t", pImage->pixelArray[i]&0xFF + 1);
		printf("G: %u\t", pImage->pixelArray[i]>>8&0xFF + 1);
		printf("B: %u\t", pImage->pixelArray[i]>>16&0xFF + 1);
	}
	printf("\n");
}