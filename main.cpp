#include "MyLib.h"

int main(int argc, char* argv[])
{
	bmp* a = nullptr;
	int check = readBMP(a, argv[2]);
	if (check == 0)
	{
		cout << "The image is not loaded successfully" << endl;
		return 0;
	}
	else
	{
		cout << "The image is loaded successfully" << endl;
	}
	short bpp = a->dib.bpp;
	if (strcmp(argv[1], "-conv") == 0 && argc == 4)
	{
		if (bpp == 24)
		{
			grayscale_24bit(a, argv[3]);
		}
		else if (bpp == 32)
		{
			grayscale_32bit(a, argv[3]);
		}
		else if (bpp == 8)
		{
			cout << "Ban khong the convert anh 8bpp ve 8bpp";
		}
	}
	else if (strcmp(argv[1], "-zoom") == 0 && argc == 5)
	{
		int scale = atoi(argv[4]);
		if (bpp == 8)
		{
			rescale_8bit(a, argv[3], scale);
		}
		else if (bpp == 24)
		{
			rescale_24bit(a, argv[3], scale);
		}
		else if (bpp == 32)
		{
			rescale_32bit(a, argv[3], scale);
		}
	}
	else
	{
		cout << "Command Line is not right, pls try again";
		system("pause");
		return 0;
	}
	delete[] a->colorpalette;
	delete[] a->data;
	delete a;
	system("pause");
	return 0;
}

