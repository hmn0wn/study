#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
using namespace std;

typedef struct
{
	unsigned int    bfType;
	unsigned long   bfSize;
	unsigned int    bfReserved1;
	unsigned int    bfReserved2;
	unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
	unsigned int    biSize;
	int             biWidth;
	int             biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompression;
	unsigned int    biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	unsigned int    biClrUsed;
	unsigned int    biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
	int   rgbBlue;
	int   rgbGreen;
	int   rgbRed;
	int   rgbReserved;
} RGBQUAD;


static unsigned short read_u16(FILE *fp);
static unsigned int   read_u32(FILE *fp);
static int            read_s32(FILE *fp);




int** read_bmp(const char *filename)
{
	FILE * pFile;
	errno_t err;
	err = fopen_s(&pFile, filename, "rb");
	if (err != 0){
		printf("File was not opened\n");
		return 0;
	}

	// считываем заголовок файла
	BITMAPFILEHEADER header;

	header.bfType = read_u16(pFile);
	header.bfSize = read_u32(pFile);
	header.bfReserved1 = read_u16(pFile);
	header.bfReserved2 = read_u16(pFile);
	header.bfOffBits = read_u32(pFile);

	// считываем заголовок изображения
	BITMAPINFOHEADER bmiHeader;

	bmiHeader.biSize = read_u32(pFile);
	bmiHeader.biWidth = read_s32(pFile);
	//printf("width: %d\n", bmiHeader.biWidth);
	bmiHeader.biHeight = read_s32(pFile);
	//printf("height: %d\n", bmiHeader.biHeight);
	bmiHeader.biPlanes = read_u16(pFile);
	bmiHeader.biBitCount = read_u16(pFile);

	bmiHeader.biCompression = read_u32(pFile);
	bmiHeader.biSizeImage = read_u32(pFile);
	bmiHeader.biXPelsPerMeter = read_s32(pFile);
	bmiHeader.biYPelsPerMeter = read_s32(pFile);
	bmiHeader.biClrUsed = read_u32(pFile);
	bmiHeader.biClrImportant = read_u32(pFile);

	int padding = ((24 / bmiHeader.biBitCount) * bmiHeader.biWidth) % 4;
	//printf("padding: %d", padding);

	RGBQUAD **rgb = new RGBQUAD*[bmiHeader.biHeight];
	for (int i = 0; i < bmiHeader.biHeight; i++) {
		rgb[i] = new RGBQUAD[bmiHeader.biWidth];
	}

	for (int i = 0; i < bmiHeader.biHeight; i++) {
		for (int j = 0; j < bmiHeader.biWidth; j++) {
			rgb[i][j].rgbBlue = getc(pFile);
			rgb[i][j].rgbGreen = getc(pFile);
			rgb[i][j].rgbRed = getc(pFile);
		}

		// пропускаем последний байт в строке
		for(int i=0;i<padding;i++)getc(pFile);
	}
	
	// выводим результат
	/*for (int i = 0; i < bmiHeader.biHeight; i++) {
		printf("\n");
		for (int j = 0; j < bmiHeader.biWidth; j++) {
			//printf("%d %d %d\n", rgb[i][j].rgbRed, rgb[i][j].rgbGreen, rgb[i][j].rgbBlue);
			if (rgb[i][j].rgbBlue == 0 && rgb[i][j].rgbGreen == 0 && rgb[i][j].rgbRed == 0) printf("%d ", 1);
			else printf("%d ", 0);
		}
		
	}
	printf("\n");*/
	fclose(pFile);
	
	//Создаем массив нулей и единиц для вывода, заодно зеркально отражаем изображение относительно оси OX.
	int** img = new int*[bmiHeader.biHeight];
	for (int i = bmiHeader.biHeight-1; i >= 0; i--){
		img[bmiHeader.biHeight-i-1] = new int[bmiHeader.biWidth];
		for (int j = 0; j < bmiHeader.biWidth; j++){
			img[bmiHeader.biHeight - i-1][j] = (rgb[i][j].rgbBlue == 0 && rgb[i][j].rgbGreen == 0 && rgb[i][j].rgbRed == 0) ? 1 : 0;
		}
		
		delete[] rgb[i];
	}

	delete[] rgb;
	return img;
}




static unsigned short read_u16(FILE *fp)
{
	unsigned char b0, b1;

	b0 = getc(fp);
	b1 = getc(fp);

	return ((b1 << 8) | b0);
}


static unsigned int read_u32(FILE *fp)
{
	unsigned char b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


static int read_s32(FILE *fp)
{
	unsigned char b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}