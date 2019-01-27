#ifndef MANDELBROT_H_
#define MANDELBROT_H_
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

// https://de.wikipedia.org/wiki/Portable_Anymap#Pixmap
void toFile(int w, int h, int maxiter,
	        int *map, std::string fileName,
	        bool transpose=false) {
  if(transpose)
  {
  	int tmp = w;
  	w = h;
  	h = tmp;
  }
  std::ofstream img(fileName + ".ppm");
  if (!img.is_open()) {
    std::cout << "Could not open the file";
    return;
  }
  img << "P3\n" << h << " " << w << " 255\n";
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int val = (map[x * h + y] % 255);
      img << val << ' ' << val << ' ' << val << "\n";
    }
  }
  img.close();
}

// QImage mandelbrot1(int w, int h, int maxiter,
// 		   double re_min, double re_max,
// 		   double im_min, double im_max)
// {
// 	QImage img(w, h, QImage::Format_RGB888);


// 	double ref = ((re_max - re_min) / (w - 1));
// 	double imf =  ((im_max - im_min) / (h - 1));
// 	for(int x=0; x<w; x++)
// 	{
// 		double c_re = re_min + x * ref;
// 		for(int y=0; y<h; y++)
// 		{
// 			double c_im = im_max - y * imf;
// 			double Z_re = c_re;
// 			double Z_im = c_im;
// 			int i;
// 			for(i=0; i<maxiter; i++)
// 			{
// 				if(Z_re*Z_re + Z_im*Z_im > 4)
// 				{
// 					break;
// 				}
// 				double Z_im2 = Z_im * Z_im;
// 				Z_im = 2 * Z_re * Z_im + c_im;
// 				Z_re = Z_re * Z_re - Z_im2 + c_re;
// 			}
// 			// img.setPixelColor(x, y, getColor(i));
// 		}
// 	}
// 	return img;
// }


int calcMandelbrot0(double c_re, double c_im, int maxiter)
{
	double Z_re = c_re;
	double Z_im = c_im;
	int i;
	for(i=0; i<maxiter; i++)
	{
		if(Z_re*Z_re + Z_im*Z_im > 4)
		{
			break;
		}
		double Z_im2 = Z_im * Z_im;
		Z_im = 2 * Z_re * Z_im + c_im;
		Z_re = Z_re * Z_re - Z_im2 + c_re;
	}
	return i;
}
int calcMandelbrot1(double c_re, double c_im, int maxiter)
{
	double z_re = c_re;
	double z_im = c_im;
	int i=0;
	while(z_re * z_re + z_im * z_im <= 4 &&  i < maxiter)
	{
		double tmp = z_re * z_re - z_im * z_im + c_re;
		z_im = 2 * z_re * z_im + c_im;
		z_re = tmp;
		i++;
	}
	return i;
}

int calcMandelbrot2(double c_re, double c_im, int maxiter)
{
	double z_re = c_re;
	double z_im = c_im;
	double z_re_sqr = z_re * z_re;
	double z_im_sqr = z_im * z_im;
	int i=0;
	while(z_re_sqr + z_im_sqr <= 4 && i < maxiter)
	{
		z_im = z_re * z_im;
		z_im += z_im;  // z_im = z_im * 2
		z_im += c_im;
		z_re = z_re_sqr - z_im_sqr + c_re;
		z_re_sqr = z_re * z_re;
		z_im_sqr = z_im * z_im;
		i++;
	}
	return i;
}

void mandelbrot0(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot0(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}
void mandelbrot1(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot1(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}
void mandelbrot2(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	double c_re = re_min;
	double c_im = im_max;
	for(int x=0; x<w; x++)
	{
		c_im = im_max;
		for(int y=0; y<h; y++)
		{
			int val = calcMandelbrot2(c_re, c_im, maxiter);
			// std::cout << x << " " << y << " "
			// 		  << c_re << " " << c_im << " "
			// 		  << val << std::endl;
			map[x*h + y] = val;
			c_im -= im_step;
		}
		c_re += re_step;
	}
}

void mandelbrot_OMP_col(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	#pragma omp parallel for schedule(dynamic, 1)
	for(int x=0; x<w; x++)
	{
		double c_re = re_min + x * re_step;
		for(int y=0; y<h; y++)
		{
			double c_im = im_min + y * im_step;
			map[x*h + y] = calcMandelbrot1(c_re, c_im, maxiter);
		}
	}
}
void mandelbrot_OMP_row(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	// #pragma omp parallel for ordered schedule(dynamic)
	// #pragma omp parallel for
	// #pragma omp parallel for num_threads(4)
	#pragma omp parallel for schedule(dynamic, 1)
	for(int y=0; y<h; y++)
	{
		double c_im = im_min + y * im_step;
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * re_step;
			map[y*w + x] = calcMandelbrot1(c_re, c_im, maxiter);
		}
	}
}


void mandelbrot_OMP_test(int w, int h, int maxiter,
			 double re_min, double re_max,
			 double im_min, double im_max,
			 int *map, int *threadIDs)
{
	double re_step = ((re_max - re_min) / (w - 1));
	double im_step = ((im_max - im_min) / (h - 1));

	// #pragma omp parallel for ordered schedule(dynamic)
	// #pragma omp parallel for schedule(dynamic, 8)
	#pragma omp parallel for collapse(2)
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			double c_re = re_min + x * re_step;
			int tid = omp_get_thread_num();
			double c_im = im_min + y * im_step;
			map[y*w + x] = calcMandelbrot1(c_re, c_im, maxiter);
			threadIDs[y*w + x] = (255/4)*tid;
		}
	}
}

#endif // MANDELBROT_H_