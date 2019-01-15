#ifndef MANDELBROT_WIDGET_H_
#define MANDELBROT_WIDGET_H_ 

#include <chrono>
#include <iostream>

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QTimer>


class MandelbrotWidget : public QWidget
{
	Q_OBJECT

public:
	MandelbrotWidget(QWidget *parent = nullptr): QWidget(parent)
	{
		resize(1200, 960);
		 // QTimer *timer = new QTimer(this);
		 // connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
		 // timer->start(1000 / 60);
	}

protected:
	void paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);
		// painter.fillRect(rect(), Qt::black);

		// faster to display
		QPixmap pixmap;

		// Allows pixel access
		// QImage img(width(), height(), QImage::Format_RGB888);
		// for(int w=0; w<width(); w++)
		// {
		// 	for(int h=0; h<height(); h++)
		// 	{
		// 		auto p = std::chrono::system_clock::now();
		// 		auto duration_in_seconds = std::chrono::duration<double>(p.time_since_epoch());
		// 		double seconds = duration_in_seconds.count() + 0.5;
		// 		int s = (int) seconds;
		// 		img.setPixelColor (
		// 			w, h,
		// 			QColor((w + s) % 255,
		// 				   (h + s) % 255,
		// 				   (w * h + s) % 255));
		// 	}
		// }

		pixmap = QPixmap::fromImage(mandelbrot());
		painter.drawPixmap(0, 0, pixmap);
	}
private:
	QImage mandelbrot()
	{
		int w = width();
		int h = height();

		double re_min{-2.0}, re_max{1.0};
		double im_min{-1.2}, im_max{1.2};

		QImage img(w, h, QImage::Format_RGB888);

		for(int x=0; x<w; x++)
		{
			for(int y=0; y<h; y++)
			{
				double c_re = re_min + x * ((re_max - re_min) / (w - 1) );
				double c_im = im_max - y * ((im_max - im_min) / (h - 1));
				double Z_re = c_re;
				double Z_im = c_im;
				bool is_inside = true;
				for(int i=0; i<50; i++)
				{
					if(Z_re*Z_re + Z_im*Z_im > 4)
					{
						is_inside = false;
						break;
					}
					double Z_im2 = Z_im * Z_im;
					Z_im = 2 * Z_re * Z_im + c_im;
					Z_re = Z_re * Z_re - Z_im2 + c_re;
				}
				if(is_inside)
				{
					// std::cout << "INSIDE: " << c_re << " + " << c_im << "i" << std::endl;
					img.setPixelColor(x, y, QColor(200, 0, 0));
				}
				else
				{
					// std::cout << "OUTSIDE: " << c_re << " + " << c_im << "i" << std::endl;
					img.setPixelColor(x, y, QColor(0, 200, 0));
				}
			}
		}
		return img;
	}
};

#endif // MANDELBROT_WIDGET_H_