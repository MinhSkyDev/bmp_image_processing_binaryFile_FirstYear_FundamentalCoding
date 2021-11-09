#include "MyLib.h"

int readBMP(bmp*& picture, const char* filename) // put the file bmp into a bmp structure 
{
	fstream input;
	input.open(filename, ios::in | ios::binary); // the filename should be ended with .bmp
	if (!input.is_open())
	{
		return 0;
	}
	picture = new bmp;
	input.read((char*)&picture->header, 14);
	input.read((char*)&picture->dib, 40);
	picture->colorpalette = new char[picture->header.DataOffset - 54];

	//Ban chat la doc phan con lai cua DIB
	input.read(picture->colorpalette, picture->header.DataOffset - 54);
	//***************************************

	input.seekg(picture->header.DataOffset, ios::beg); // move the pointer to the DataOffset's position
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;
	unsigned int BitPerPixel = (picture->dib.bpp) / 8; // determine whether the bmp file is 8pp, 24pp or 32pp
	unsigned int padding =(4-(w % 4))%4;
	size = w * h * BitPerPixel + padding * h;
	picture->data = new char[size];
	input.read(picture->data, size);
	input.close();
	return 1;
}
void writeBMP(bmp* picture, const char* filename) 
{
	fstream output;
	output.open(filename, ios::out | ios::binary);
	output.write((char*)&picture->header, sizeof(Header));
	output.write((char*)&picture->dib, sizeof(DIB));

	//Ban chat that ra la viet phan con thua cua DIB
	output.write(picture->colorpalette, picture->header.DataOffset - 54); 
	
	// Print out the picture file
	output.seekp(picture->header.DataOffset, ios::beg); // Move the pointer to the place starting to begin the data pixel
	output.write(picture->data, picture->header.FileSize);
	output.close();
}
void grayscale_24bit(bmp*& picture, const char* output)
{
	bmp* temp = new bmp; // remember to delete
	*temp = *picture;

	// Get the data pixel size
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;
	unsigned int BitPerPixel = (picture->dib.bpp) / 8;
	unsigned int padding = (4-(w * BitPerPixel % 4))%4;
	size = w * h * BitPerPixel + padding * h;

	//**************
	//Create a temporary pointer to go through every element of the pixeldata array
	char* temp_ = temp->data;
	//**************

	//Create a Pixel-Image Array ( 24Bit ) ( this will be a 2D Array for easier manipulation )
	Pixel_24bit* image = new Pixel_24bit [w*h];
	//**************

	//Get the Data of 2D Pixel-Image
	int count = 0;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			image[count].Blue = *(temp_++);
			image[count].Green = *(temp_++);
			image[count].Red = *(temp_++);
			count++;
		}
		for (int k = 0; k < padding; k++)
		{
			temp_++;
		}
	}

	//Create the new Dynamically Allocated GrayScaleArray
	int padding_new = (4-(w % 4))%4;
	int size_new = w * h + h * padding_new;
	temp->data = new char[size_new];
	count = 0;
	temp_ = temp->data;

	//gray-scaling process
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			//Caculating the avarage value
			int Average = image[count].Blue / 3 + image[count].Green / 3 + image[count].Red / 3;
			image[count].Blue = image[count].Green = image[count].Red = Average;
			//Put the value into the pixel data
			*(temp_++) = Average;
			//increase the index
			count++;
		}
		for (int j = 0; j < padding_new; j++)
			*(temp_++) = 0;
	}

	//write 1024 color of the colortable
	temp->colorpalette = new char[256 * 4];
	temp_ = temp->colorpalette; // reuse the temp_ pointer
	count = 0;
	for (int i = 0; i <= 255; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			*(temp_++) = i;
		}
		*(temp_++) = 0;
	}

	temp->header.FileSize = 54 + 1024 + size_new;
	temp->dib.bpp = 8;
	temp->dib.DIBsize = 40;
	temp->dib.imageSize = size_new;
	temp->header.DataOffset = 54 + 1024;
	writeBMP(temp, output);


	//delete Dynamically allocated array
	delete[] image;
	delete[] temp->data;
	delete[] temp->colorpalette;
	delete temp;
}
void grayscale_32bit(bmp*& picture, const char* output)
{
	bmp* temp = new bmp; // remember to delete
	*temp = *picture;

	// Get the data pixel size
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;
	unsigned int BitPerPixel = (picture->dib.bpp) / 8;
	unsigned int padding = (4 - (w * BitPerPixel % 4)) % 4;
	size = w * h * BitPerPixel + padding * h;

	//**************
	//Create a temporary pointer to go through every element of the pixeldata array
	char* temp_ = temp->data;
	//**************

	//Create a Pixel-Image Array ( 24Bit ) ( this will be a 2D Array for easier manipulation )
	Pixel_32bit** image = new Pixel_32bit * [h];
	for (int i = 0; i < h; i++)
	{
		image[i] = new Pixel_32bit[w];
	}
	//**************

	//Get the Data of 2D Pixel-Image
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			image[i][j].A = *(temp_++);
			image[i][j].Blue = *(temp_++);
			image[i][j].Green = *(temp_++);
			image[i][j].Red = *(temp_++);
		}
	}

	//Create the new Dynamically Allocated GrayScaleArray
	int padding_new = (4-(w % 4))%4;
	int size_new = w * h + h * padding_new;
	temp->data = new char[size_new];
	int count = 0;

	//gray-scaling process
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			int Average = image[i][j].Blue / 4 + image[i][j].Green / 4 + image[i][j].Red / 4 + image[i][j].A / 4;
			image[i][j].Blue = image[i][j].Green = image[i][j].Red = Average;
		}
	}

	//put all the pixel into array and transform into 8bit
	temp_ = temp->data;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			*(temp_++) = image[i][j].Red; // just take a random blue red or green or A
		}
		for (int k = 0; k < padding_new; k++)
		{
			*(temp_++) = 0;
		}
	}

	//write 1024 color of the colortable
	temp->colorpalette = new char[256 * 4];
	temp_ = temp->colorpalette; // reuse the temp_ pointer
	count = 0;
	for (int i = 0; i <= 255; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			*(temp_++) = i;
		}
		*(temp_++) = 0;
	}

	temp->header.FileSize = 54 + 1024 + size_new;
	temp->dib.bpp = 8;
	temp->dib.DIBsize = 40;
	temp->dib.imageSize = size_new;
	temp->header.DataOffset = 54 + 1024;
	writeBMP(temp, output);


	//delete Dynamically allocated array
	for (int i = 0; i < h; i++)
	{
		delete[] image[i];
	}
	delete[] image;
	delete[] temp->data;
	delete[] temp->colorpalette;
	delete temp;
}
Pixel_24bit countAverage24bit(Pixel_24bit** a, int i_cur, int j_cur, int i_increase, int j_increase)
{
	unsigned char divide = unsigned char(i_increase) * unsigned char(j_increase);
	Pixel_24bit temp;
	int Sum_Blue = 0, Sum_Green = 0, Sum_Red = 0;
	for (int i = i_cur; i < i_cur + i_increase; i++)
	{
		for (int j = j_cur; j < j_cur + j_increase; j++)
		{
			Sum_Blue += a[i][j].Blue;
			Sum_Green += a[i][j].Green;
			Sum_Red += a[i][j].Red;
		}
	}
	temp.Blue = Sum_Blue / divide;
	temp.Green = Sum_Green / divide;
	temp.Red = Sum_Red / divide;
	return temp;
}
void rescale_24bit(bmp*& picture, const char* output, int s) // remember to add output directory for the 5th question
{
	bmp* temp = new bmp; // remember to delete
	*temp = *picture;

	// Get the data pixel size
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;
	unsigned int BitPerPixel = (picture->dib.bpp) / 8;
	unsigned int padding = (4 - (w * BitPerPixel % 4)) % 4;
	size = w * h * BitPerPixel + padding * h;

	//**************
	//Create a temporary pointer to go through every element of the pixeldata array
	char* temp_ = temp->data;
	//**************

	//Create a Pixel-Image Array ( 24Bit ) ( this will be a 2D Array for easier manipulation )
	Pixel_24bit** image = new Pixel_24bit * [h];
	for (int i = 0; i < h; i++)
	{
		image[i] = new Pixel_24bit[w];
	}
	//**************

	//Get the Data of 2D Pixel-Image
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			image[i][j].Blue = *(temp_++);
			image[i][j].Green = *(temp_++);
			image[i][j].Red = *(temp_++);
		}
		for (int k = 0; k < padding; k++)
		{
			temp_++;
		}
	}

	//Caculating the new size of the new image
	unsigned int h_new, w_new, padding_new;
	if (h % s == 0)
	{
		h_new = h / s;
	}
	else
	{
		h_new = h / s + 1;
	}
	if (w % s == 0)
	{
		w_new = w / s;
	}
	else w_new = w / s + 1;
	padding_new = (4-(w_new*3 % 4))%4;
	int size_new = w_new * h_new * 3 + padding_new * h_new; // 24bit 

	//Creating the rescale image pixel data 2D Array
	Pixel_24bit* image_new = new Pixel_24bit[w_new * h_new];
	//******************

	int count = 0;
	for (int i = 0; i < h; i += s)
	{
		for (int j = 0; j < w; j += s)
		{
			if (i == (h - (h % s)) && h / s != 0) // h/s !=0 in oder to not get the error from c/0 
			{
				image_new[count++] = countAverage24bit(image, i, j, h % s, s); // leftover byte
				continue;
			}
			if (j == (w - (w % s)) && w / s != 0)
			{
				image_new[count++] = countAverage24bit(image, i, j, s, w % s); // leftover byte
				continue;
			}
			image_new[count++] = countAverage24bit(image, i, j, s, s);
		}
	}

	//Put all the value into the temp->data
	temp->data = new char[size_new];
	temp_ = temp->data;
	count = 0;
	for (int i = 0; i < h_new; i++)
	{
		for (int j = 0; j < w_new; j++)
		{
			*(temp_++) = image_new[count].Blue;
			*(temp_++) = image_new[count].Green;
			*(temp_++) = image_new[count].Red;
			count++;
		}
		for (int k = 0; k < padding_new; k++)
		{
			*(temp_++) = 0;
		}
	}
	temp->dib.height = h_new;
	temp->dib.width = w_new;
	temp->header.FileSize = 14 + picture->dib.DIBsize + size_new;
	writeBMP(temp, output);

	//delete dynamic array
	delete[] image_new;
	for (int i = 0; i < h; i++)
	{
		delete[] image[i];
	}
	delete image;
	delete[] temp->data;
	delete temp;
}
Pixel_32bit countAverage32bit(Pixel_32bit** a, int i_cur, int j_cur, int i_increase, int j_increase)
{
	unsigned char divide = unsigned char(i_increase) * unsigned char(j_increase);
	Pixel_32bit temp;
	int Sum_Blue = 0, Sum_Green = 0, Sum_Red = 0, Sum_A = 0;
	for (int i = i_cur; i < i_cur + i_increase; i++)
	{
		for (int j = j_cur; j < j_cur + j_increase; j++)
		{
			Sum_A += a[i][j].A;
			Sum_Blue += a[i][j].Blue;
			Sum_Green += a[i][j].Green;
			Sum_Red += a[i][j].Red;
		}
	}
	temp.Blue = Sum_Blue / divide;
	temp.Green = Sum_Green / divide;
	temp.Red = Sum_Red / divide;
	temp.A = Sum_A / divide;
	return temp;
}
void rescale_32bit(bmp*& picture, const char* output, int s)
{
	bmp* temp = new bmp; // remember to delete
	*temp = *picture;

	// Get the data pixel size
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;

	//**************
	//Create a temporary pointer to go through every element of the pixeldata array
	char* temp_ = temp->data;
	//**************

	//Create a Pixel-Image Array ( 24Bit ) ( this will be a 2D Array for easier manipulation )
	Pixel_32bit** image = new Pixel_32bit * [h];
	for (int i = 0; i < h; i++)
	{
		image[i] = new Pixel_32bit[w];
	}
	//**************

	//Get the Data of 2D Pixel-Image
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			image[i][j].A = *(temp_++);
			image[i][j].Blue = *(temp_++);
			image[i][j].Green = *(temp_++);
			image[i][j].Red = *(temp_++);
		}
	}

	//Caculating the new size of the new image
	unsigned int h_new, w_new, padding_new;
	if (h % s == 0)
	{
		h_new = h / s;
	}
	else
	{
		h_new = h / s + 1;
	}
	if (w % s == 0)
	{
		w_new = w / s;
	}
	else w_new = w / s + 1;
	int size_new = w_new * h_new * 4; // 32bit 

	//Creating the rescale image pixel data 2D Array
	Pixel_32bit* image_new = new Pixel_32bit[w_new * h_new];
	//******************

	int count = 0;
	for (int i = 0; i < h; i += s)
	{
		for (int j = 0; j < w; j += s)
		{
			if (i == (h - (h % s)) && h / s != 0)
			{
				image_new[count++] = countAverage32bit(image, i, j, h % s, s); // leftover byte
				continue;
			}
			if (j == (w - (w % s)) && w / s != 0)
			{
				image_new[count++] = countAverage32bit(image, i, j, s, w % s); // leftover byte
				continue;
			}
			image_new[count++] = countAverage32bit(image, i, j, s, s);
		}
	}

	//Put all the value into the temp->data
	temp->data = new char[size_new];
	temp_ = temp->data;
	count = 0;
	for (int i = 0; i < h_new; i++)
	{
		for (int j = 0; j < w_new; j++)
		{
			*(temp_++) = image_new[count].A;
			*(temp_++) = image_new[count].Blue;
			*(temp_++) = image_new[count].Green;
			*(temp_++) = image_new[count].Red;
			count++;
		}
	}
	temp->dib.height = h_new;
	temp->dib.width = w_new;
	temp->header.FileSize = 14 + temp->dib.DIBsize + size_new;
	writeBMP(temp, output);

	//delete dynamic array
	delete[] image_new;
	for (int i = 0; i < h; i++)
	{
		delete[] image[i];
	}
	delete image;
	delete[] temp->data;
	delete temp;
}
char countAverage8bit(char** a, int i_cur, int j_cur, int i_increase, int j_increase)
{
	int divide = i_increase * j_increase;
	int temp;
	int sum = 0;
	for (int i = i_cur; i < i_cur + i_increase; i++)
	{
		for (int j = j_cur; j < j_cur + j_increase; j++)
		{
			sum += a[i][j];
		}
	}
	temp = sum / divide;
	return temp;
}
void rescale_8bit(bmp*& picture, const char* output, int s)
{
	bmp* temp = new bmp; // remember to delete
	*temp = *picture;

	// Get the data pixel size
	unsigned int w, h, size;
	w = picture->dib.width;
	h = picture->dib.height;
	unsigned int BitPerPixel = (picture->dib.bpp) / 8;
	unsigned int padding = w * BitPerPixel % 4;
	size = w * h * BitPerPixel + padding * h;

	//**************
	//Create a temporary pointer to go through every element of the pixeldata array
	char* temp_ = temp->data;
	//**************

	//Create a Pixel-Image Array ( 24Bit ) ( this will be a 2D Array for easier manipulation )
	char** image = new char* [h];
	for (int i = 0; i < h; i++)
	{
		image[i] = new char[w];
	}
	//**************

	//Get the Data of 2D Pixel-Image
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			image[i][j] = *(temp_++);
		}
		for (int k = 0; k < padding; k++)
		{
			*(temp_)++;
		}
	}

	//Caculating the new size of the new image
	unsigned int h_new, w_new, padding_new;
	if (h % s == 0)
	{
		h_new = h / s;
	}
	else
	{
		h_new = h / s + 1;
	}
	if (w % s == 0)
	{
		w_new = w / s;
	}
	else w_new = w / s + 1;
	padding_new = (4-(w_new % 4))%4;
	int size_new = w_new * h_new + padding_new * h_new; // 8bit 

	//Creating the rescale image pixel data 2D Array
	char* image_new = new char[w_new * h_new];
	//******************

	int count = 0;
	for (int i = 0; i < h; i += s)
	{
		for (int j = 0; j < w; j += s)
		{
			if (i == (h - (h % s)) && h / s != 0) // h/s !=0 in oder to not get the error from c/0 
			{
				image_new[count++] = countAverage8bit(image, i, j, h % s, s); // leftover byte
				continue;
			}
			if (j == (w - (w % s)) && w / s != 0)
			{
				image_new[count++] = countAverage8bit(image, i, j, s, w % s); // leftover byte
				continue;
			}
			image_new[count++] = countAverage8bit(image, i, j, s, s);
		}
	}

	//Put all the value into the temp->data
	temp->data = new char[size_new];
	temp_ = temp->data;
	count = 0;
	for (int i = 0; i < h_new; i++)
	{
		for (int j = 0; j < w_new; j++)
		{
			*(temp_++) = image_new[count++];
		}
		for (int k = 0; k < padding_new; k++)
		{
			*(temp_++) = 0;
		}
	}

	

	//caculate new Header and DIB
	temp->dib.height = h_new;
	temp->dib.width = w_new;
	temp->header.FileSize = 54 + 1024 + size_new;
	temp->dib.imageSize = size_new;
	writeBMP(temp, output);

	//delete dynamic array
	delete[] image_new;
	for (int i = 0; i < h; i++)
	{
		delete[] image[i];
	}
	delete image;
	delete[] temp->data;
	delete temp;
}