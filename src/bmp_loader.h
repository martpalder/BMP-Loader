#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ROWS SHRT_MAX / 3 * 2
#define COLUMNS SHRT_MAX / 3 * 2

// Bitmap file header
struct Bitmap_header {
	uint16_t identity;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;	
};

// DIB header
struct Dib_header {
	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t colorPlanes;
	uint16_t bpp;
	uint32_t compression;
	uint32_t imageSize;
	int32_t hres;
	int32_t vres;
	uint32_t totalColors;
	uint32_t usedColors;
};

// image file
typedef struct {
	struct Bitmap_header bitmapHeader;
	struct Dib_header dibHeader;
	uint32_t pixelArray[PTRDIFF_MAX / 8];
} Image;

uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8)&0xFF00FF00) | ((val >> 8)&0xFF00FF); 
    return (val << 16) | (val >> 16);
}

Image* loadBmp(const char* FILE_PATH) {
	// open an .OBJ file
	FILE* fp = fopen(FILE_PATH, "rb");
	if (!fp){
		fprintf(stderr, "\nUnable to open file at path \"%s\": %s\n", FILE_PATH, strerror(errno));
		exit(0);
	}
	else if (!strstr(FILE_PATH, ".bmp") && !strstr(FILE_PATH, ".BMP")) {
		unsigned char filePos = strcspn(FILE_PATH, ".");
		fprintf(stderr, "\nIncorrect file format: %s", FILE_PATH + filePos);
		fprintf(stderr, "\nA .BMP file is required\n");
		exit(0);
	}
	
	// instance of image file
	static Image image;
	Image* pImage = &image;
	
	// bitmap header buffer
	unsigned char header[54];
	
	// read headers
	fread(&header, 54, 1, fp);
	// if file identifier doesn't match
	if (header[0x00] != 0x42 || header[0x01] != 0x4D) {
		// close the file
		fclose(fp);
		puts("\nNot a correct bitmap file");
		exit(0);
	}
	
	// add to bitmap file header
	pImage->bitmapHeader.identity = header[0x00];
	pImage->bitmapHeader.fileSize = header[0x02];
	pImage->bitmapHeader.reserved1 = header[0x06];
	pImage->bitmapHeader.reserved2 = header[0x08];
	pImage->bitmapHeader.offset = header[0x0A];
	
	// add to DIB header
	pImage->dibHeader.headerSize = header[0x0E];
	pImage->dibHeader.width = header[0x12];
	pImage->dibHeader.height = header[0x16];
	pImage->dibHeader.colorPlanes = header[0x1A];
	pImage->dibHeader.bpp = header[0x1C];
	pImage->dibHeader.compression = header[0x1E];
	pImage->dibHeader.imageSize = header[0x22];
	pImage->dibHeader.hres = header[0x26];
	pImage->dibHeader.vres = header[0x2A];
	pImage->dibHeader.totalColors = header[0x2E];
	pImage->dibHeader.usedColors = header[0x32];
	// if DIB header size is not 40
	if (pImage->dibHeader.headerSize != 40) {
		// close the file
		fclose(fp);
		fprintf(stderr, "\nIncorrect header");
		fprintf(stderr, "\nA file with BITMAPINFOHEADER required\n");
		exit(0);
	}
	else if (pImage->dibHeader.imageSize == 0) {
		// close the file
		fclose(fp);
		fprintf(stderr, "\nImage size information is missing\n");
		exit(0);
	}
	
	// check color depth
	unsigned char padding;
	if (pImage->dibHeader.bpp <= 8) {
		fprintf(stderr, "\nThis image has 8-bit or less colors\n");
		exit(0);
	}
	else {
		if (pImage->dibHeader.width % 4 == 1) padding = 3;
		else if (pImage->dibHeader.width % 4 == 2) padding = 2;
		else if (pImage->dibHeader.width % 4 == 3) padding = 1;
		else padding = 0;
	}
	
	// calculate array size
	unsigned short rowSize = (pImage->dibHeader.bpp * abs(pImage->dibHeader.width)) / 8;
	unsigned int pixelArraySize = rowSize * abs(pImage->dibHeader.height);
	
	// calculate number of pixels
	unsigned int numPixels = pixelArraySize / (pImage->dibHeader.bpp / 8);
	
	// data buffer
	unsigned char data[pImage->dibHeader.imageSize];
	
	// read pixel arrays
	fread(&data, pImage->dibHeader.imageSize, 1, fp);
	
	// add each pixel to array
	unsigned int pos = 0;
	for (unsigned int i = 0; i < numPixels; ++i) {
		pImage->pixelArray[i] = swap_uint32(*(uint32_t*)&data[pos]);
		printf("\nPixel: %u", pImage->pixelArray[i]);
		printf("\nPosition: %u\n", pos);
		pos += (pImage->dibHeader.bpp / 8);
		
		if (pos != 0 && pos % rowSize == 0) {
			pos += rowSize / (pImage->dibHeader.bpp / 8);
			printf("\nPadding");
		}
	}
	
	// close the file
	fclose(fp);
	
	return pImage;
}

#endif