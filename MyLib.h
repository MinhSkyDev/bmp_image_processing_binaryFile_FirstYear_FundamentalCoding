#pragma once
#pragma pack(2)
#include <iostream>
#include <fstream>
using namespace std;

struct Header { // 14 bytes
	unsigned char signature[2];
	unsigned int FileSize;
	unsigned int reserved;
	unsigned int DataOffset; // contain the address of the point begin to save the picture
};

struct DIB { // 40 bytes
	unsigned int DIBsize;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bpp; // bits per pixel
	unsigned int compression;
	unsigned int imageSize;
	unsigned int Xpixels;
	unsigned int Ypixels;
	unsigned int colorsUsed;
	unsigned int importantColor;
};


struct bmp { // one picture will have a struct like this
	Header header;
	DIB dib;
	char* colorpalette;
	char* data; // contain the pixel data
};

struct Pixel_24bit {
	unsigned char Blue; // Thuc su la em da thu moi cach la de kieu du lieu la char,int,usigned int nhung ma no ko co ra duoc cai mau dep nhu de unsigned char
	unsigned char Green;
	unsigned char Red;
};

struct Pixel_32bit {
	unsigned char A;
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
};

int readBMP(bmp*& picture, const char* filename); // put the file bmp into a bmp structure 
void writeBMP(bmp* picture, const char* filename);
void rescale_24bit(bmp*& picture, const char* output, int s); 
void rescale_32bit(bmp*& picture, const char* output, int s);
void grayscale_24bit(bmp*& picture, const char* output);
void writeBMP_8bit(bmp* picture, const char* filename);
void grayscale_32bit(bmp*& picture, const char* output);
void rescale_8bit(bmp*& picture, const char* output, int s);






