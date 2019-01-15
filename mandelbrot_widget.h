#ifndef MANDELBROT_WIDGET_H_
#define MANDELBROT_WIDGET_H_ 

#include <chrono>
#include <iostream>
#include <cmath>

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QApplication>
#include <QKeyEvent>
#include <QFont>


class MandelbrotWidget : public QWidget
{
	Q_OBJECT

public:
	MandelbrotWidget(QWidget *parent = nullptr): QWidget(parent)
	{
		// QScreen *screen = QGuiApplication::primaryScreen();
		// QRect  screenGeometry = screen->geometry();
		// int height = screenGeometry.height();
		// int width = screenGeometry.width();
		// resize(height, width);
		 // QTimer *timer = new QTimer(this);
		 // connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
		 // timer->start(1000 / 60);
	}

protected:
	void keyPressEvent(QKeyEvent* event) override {
		if (event->key() == Qt::Key_Escape)
		{	
			QApplication::quit();
		}
		else if(event->key() == Qt::Key_Plus)
		{
			zoom(re_min, re_max, zf);
			zoom(im_min, im_max, zf);
			repaint();
		}
		else if(event->key() == Qt::Key_Minus)
		{
			zoom(re_min, re_max, izf);
			zoom(im_min, im_max, izf);;
			repaint();
		}
		else if(event->key() == Qt::Key_W)
		{

			im_min += 0.1 * (im_max - im_min);
			im_max += 0.1 * (im_max - im_min);
			repaint();			
		}
		else if(event->key() == Qt::Key_A)
		{
			re_min -= 0.1 * (re_max - re_min);
			re_max -= 0.1 * (re_max - re_min);
			repaint();
		}
		else if(event->key() == Qt::Key_S)
		{
			im_min -= 0.1 * (im_max - im_min);
			im_max -= 0.1 * (im_max - im_min);
			repaint();
		}
		else if(event->key() == Qt::Key_D)
		{
			re_min += 0.1 * (re_max - re_min);
			re_max += 0.1 * (re_max - re_min);
			repaint();
		}
		else if(event->key() == Qt::Key_Up)
		{
			maxiter += 10;
		}
		else if(event->key() == Qt::Key_Down)
		{
			maxiter -= 10;
		}
		else if(event->key() == Qt::Key_Return)
		{
			std::cout << "ENTER\n";
			repaint();
		}
	}
	void paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);

		// faster to display
		QPixmap pixmap;

		pixmap = QPixmap::fromImage(mandelbrot());
		painter.drawPixmap(0, 0, pixmap);
		painter.setFont(QFont("times",28));
		painter.setPen(Qt::red);

		painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, QString::number(im_max));
		painter.drawText(rect(), Qt::AlignBottom | Qt::AlignHCenter, QString::number(im_min));
		painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, QString::number(re_min));
		painter.drawText(rect(), Qt::AlignRight | Qt::AlignVCenter, QString::number(re_max));
		painter.drawText(rect(), Qt::AlignRight | Qt::AlignTop, QString::number(maxiter));
		painter.drawText(rect(), Qt::AlignCenter, tr("O"));
	}
private:
	// double zf = 0.5;
	double zf = 0.7;
	// double izf = 2;
	double izf = 1.4285714285714286;
	int maxiter = 20;
	double re_min{-2}, re_max{1.0};
	double im_min{-1.2}, im_max{1.2};
	void zoom(double &minimum, double &maximum, double zf)
	{
		double diff = maximum - minimum;
		double diff_new = diff * zf;
		double add = (diff - diff_new) * 0.5;
		minimum += add;
		maximum -= add;
	}
	QColor getColor(int iteration)
	{
		if(iteration < maxiter)
		{
			// int i = (int) (iteration * 255*255*255) / maxiter;
			// i = iteration;
			// int R = (i & 0x000000FF);
			// int G = (i & 0x0000FF00) >> 8;
			// int B = (i & 0x00FF0000) >> 16;
			// int A = (i & 0xFF000000) >> 24;
			// return QColor(R,G,B,A);

			int color = iteration % 16;
			int a = iteration % 255;
		    QColor mapping[16];
		    mapping[0].setRgb( 66,  30,  15 );
		    mapping[1].setRgb( 25,  7,   26 );
		    mapping[2].setRgb( 9,   1,   47 );
		    mapping[3].setRgb( 4,   4,   73 );
		    mapping[4].setRgb( 0,   7,   100);
		    mapping[5].setRgb( 12,  44,  138);
		    mapping[6].setRgb( 24,  82,  177);
		    mapping[7].setRgb( 57,  125, 209);
		    mapping[8].setRgb( 134, 181, 229);
		    mapping[9].setRgb( 211, 236, 248);
		    mapping[10].setRgb(241, 233, 191);
		    mapping[11].setRgb(248, 201, 95 );
		    mapping[12].setRgb(255, 170, 0  );
		    mapping[13].setRgb(204, 128, 0  );
		    mapping[14].setRgb(153, 87,  0  );
		    mapping[15].setRgb(106, 52,  3  );
			return mapping[color];
		}
		return Qt::white;
	}
	QImage mandelbrot()
	{
		int w = width();
		int h = height();

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