#include "mandelbrot.h"
#include <iostream>
#include <fstream>

// https://de.wikipedia.org/wiki/Portable_Anymap#Pixmap
void toFile(int w, int h, int maxiter, int* map)
{
	std::ofstream img("mandelbrot.ppm");
	if(!img.is_open())
	{
		std::cout << "Could not open the file";
		return;
	}
	img << "P3\n" << h << " " << w << " 255\n";
	for(int x=0; x<w; x++)
	{
		for(int y=0; y<h; y++)
		{
			int val = (map[x * h  + y] % 255);
			img << val << ' ' << 0 << ' ' << 0 << "\n";
		}
	}
	img.close();
}


int main()
{
	int w = 3000;
	int h = 2400;
	int maxiter = 500;
	double re_min = -2;
	double re_max = 1;
	double im_min = -1.2;
	double im_max = 1.2;

	int *map = new int[w*h];

	calcAll(w, h, maxiter, re_min, re_max, im_min, im_max, map);
	// toFile(w, h, maxiter, map);
	delete[] map;
}