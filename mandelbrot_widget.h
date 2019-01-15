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
		resize(120, 96);
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
	int maxiter = 50;
	QColor getColor(int iteration)
	{
		if(iteration < maxiter)
		{
			int color = iteration % 16;
		    QColor mapping[16];
		    mapping[0].setRgb(66, 30, 15);
		    mapping[1].setRgb(25, 7, 26);
		    mapping[2].setRgb(9, 1, 47);
		    mapping[3].setRgb(4, 4, 73);
		    mapping[4].setRgb(0, 7, 100);
		    mapping[5].setRgb(12, 44, 138);
		    mapping[6].setRgb(24, 82, 177);
		    mapping[7].setRgb(57, 125, 209);
		    mapping[8].setRgb(134, 181, 229);
		    mapping[9].setRgb(211, 236, 248);
		    mapping[10].setRgb(241, 233, 191);
		    mapping[11].setRgb(248, 201, 95);
		    mapping[12].setRgb(255, 170, 0);
		    mapping[13].setRgb(204, 128, 0);
		    mapping[14].setRgb(153, 87, 0);
		    mapping[15].setRgb(106, 52, 3);
			// std::cout << "INSIDE: " << c_re << " + " << c_im << "i   " << color << std::endl;
			// img.setPixelColor(x, y, Qt::white);
			return mapping[color];
		}
		return Qt::white;
	}
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
				int i{0};
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
				QColor color = getColor(i);
				img.setPixelColor(x, y, color);
			}
		}
		return img;
	}
};

#endif // MANDELBROT_WIDGET_H_